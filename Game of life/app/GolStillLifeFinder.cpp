#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <random>
#include <memory> 
#include <vector>

#include <GolGrid.h>
#include <GolIterator.h>

// MPI tags
#define TAG_SL_FOUND 0
#define TAG_SL_PATTERN 1
#define TAG_FINISHED 2

// Help message 
void PrintHelp(const std::string &AppName)
{
    std::cout << "Usage:\n"
    << "  " << AppName << " --random <grid_rows> <grid_cols> <num_alive_cells> --stillLifes <num_still_lifes> --trials <max_trials> --generations <num_generations> [--seed <seed_value>]\n\n"
            << "Description:\n"
            << "  Conway's Game of Life Still Life Finder (MPI version)\n"
            << "  This program searches for still life patterns in Conway's Game of Life using random initial grids.\n"
            << "  The search is parallelised using MPI and OpenMP. The program will attempt to find the specified number of still lifes\n"
            << "  within the maximum number of trials, simulating up to the specified number of generations per trial.\n\n"
            << "Options:\n"
            << "  -h, --help              Display this help message and exit\n"
            << "  -r, --random            Initialise grid randomly. Requires grid size and number of alive cells\n"
            << "  -n, --stillLifes        Target number of still life grids to find\n"
            << "  -t, --trials            Maximum number of trials for searching still lifes (across all processes)\n"
            << "  -g, --generations       Maximum number of generations to simulate on each process\n"
            << "  -s, --seed              Specify a seed for random initialisation (optional)\n\n"
            << "Note: All options except --seed are required.\n";
}

int main(int argc, char **argv) 
{   
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Process id
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Number of processes
    
    std::string AppName = argv[0]; // Application name for help messages

    // Display help if no arguments are passed
    if (argc == 1) 
    {
        PrintHelp(AppName);
        MPI_Abort(MPI_COMM_WORLD, 0);
        return 0;
    }

    // Variables for command line arguments
    size_t generations = 0;
    size_t NumStillLifes = 0;
    size_t MaxTrials = 0;
    std::string OutputFileName;
    bool IsOutputFile = false;
    size_t rows = 0;
    size_t cols = 0;
    size_t NumAliveCells = 0;
    bool SeedSpecified = false;
    size_t SeedVal = 0;

    // Parse command line arguments
    if (rank == 0)
    {
        // --- Start of command line parsing --- //

        // Flag checks
        bool IsRandomMode = false;
        bool generations_specified = false;
        bool stillLifes_specified = false;
        bool trials_specified = false;
     

        for (int i = 1; i < argc; ++i) 
        {
            // Parse arguments as strings
            std::string arg = argv[i];

            if (arg == "-h" || arg == "--help") 
            {
                PrintHelp(AppName);
                MPI_Abort(MPI_COMM_WORLD, 0);
            }

            // Random mode check
            else if (arg == "-r" || arg == "--random") 
            {
                if (i + 2 < argc && argv[i + 1][0] != '-' && argv[i + 2][0] != '-' && 
                        argv[i + 3][0] != '-')
                {
                    // Parse grid size and number of alive cells
                    rows = std::stoul(argv[++i]);
                    cols = std::stoul(argv[++i]);
                    NumAliveCells = std::stoul(argv[++i]);
                    IsRandomMode = true;
                } 
                else 
                {
                    std::cerr << "Missing grid size and/or number of alive cells after --random\n";
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }

            // Seed check 
            else if (arg == "-s" || arg == "--seed") 
            {
                if (i + 1 < argc && argv[i + 1][0] != '-') 
                {
                    SeedVal = std::stoul(argv[++i]);
                    SeedSpecified = true;
                }
                else 
              {
                  std::cerr << "Missing seed value after --seed\n";
                  MPI_Abort(MPI_COMM_WORLD, 1);
              }
          }

            // Generations check
            else if (arg == "-g" || arg == "--generations") 
            {
                if (i + 1 < argc && argv[i + 1][0] != '-') 
                {
                    generations = std::stoul(argv[++i]);
                    generations_specified = true;
                } 
                else 
                {
                    std::cerr << "Missing number of generations after --generations\n";
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }

            // Still lifes check
            else if (arg == "-n" || arg == "--stillLifes") 
            {
                if (i + 1 < argc && argv[i + 1][0] != '-') 
                {
                    NumStillLifes = std::stoul(argv[++i]);
                    stillLifes_specified = true;
                } 
                else 
                {
                    std::cerr << "Missing number of still lifes after --stillLifes\n";
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }

            // Max trials check
            else if (arg == "-t" || arg == "--trials") 
            {
                if (i + 1 < argc && argv[i + 1][0] != '-') 
                {
                    MaxTrials = std::stoul(argv[++i]);
                    trials_specified = true;
                } 
                else 
                {
                    std::cerr << "Missing maximum number of trials after --trials\n";
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }

            // Output file check
            else if (arg == "-o" || arg == "--output") 
            {
                if (i + 1 < argc && argv[i + 1][0] != '-') 
                {
                    OutputFileName = argv[++i];
                    IsOutputFile = true;
                } 
                else 
                {
                    std::cerr << "Missing output filename after --output\n";
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }
            }
        // --- End of command line parsing --- //

        // Validate command line arguments
        if (!IsRandomMode)
        {
            std::cerr << "Error: --random must be specified\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        else if (!stillLifes_specified)
        {
            std::cerr << "Error: --stillLifes must be specified\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        else if (!trials_specified)
        {
            std::cerr << "Error: --trials must be specified\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        else if (!generations_specified)
        {
            std::cerr << "Error: --generations must be specified\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        else if (NumAliveCells > rows * cols)
        {
            std::cerr << "Error: Number of alive cells cannot exceed grid size\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        else if (rows == 0 || cols == 0)
        {
            std::cerr << "Error: Grid size cannot be zero\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
         // Create a random number generator
         std::random_device rd;
         std::mt19937 rng(SeedSpecified ? SeedVal : rd());
    
    } // End of rank 0 command line parsing

    // Broadcast the parameters to all processes
    MPI_Bcast(&rows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&cols, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&NumAliveCells, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&generations, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&NumStillLifes, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&MaxTrials, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&SeedSpecified, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD); 
    MPI_Bcast(&SeedVal, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD); 

    // -- Run the simulation -- //

    // Information about the process
    if (rank == 1)
    {
        std::cout << "Process " << rank << " is running with the following parameters:\n"
                    << "Grid Size: " << rows << " x " << cols << "\n"
                    << "Number of Alive Cells: " << NumAliveCells << "\n"
                    << "Generations: " << generations << "\n"
                    << "Still Lifes: " << NumStillLifes << "\n"
                    << "Max Trials: " << MaxTrials << "\n\n";
    }

    // Set up trackers
    std::vector<gol::Grid> GlobalKnownSL; // Global list of known still lifes
    GlobalKnownSL.reserve(NumStillLifes); 
    std::vector<gol::Grid> LocalKnownSL; // Local list of known still lifes, for ranks > 0
    size_t StillLivesFound = 0;
    size_t TrialsPerProcess = MaxTrials / size;
    size_t LeftoverTrials = MaxTrials % size;
    size_t TrialsToRun = TrialsPerProcess + (rank < LeftoverTrials? 1 : 0);

    // Random number generator for each process
    std::random_device rd;
    
    for (size_t i = 0; i < TrialsToRun; ++i)
    {
        // Create a random grid
        unsigned int TrialSeed = ((SeedVal + ((TrialsToRun + 1) * rank)) + i); // Unique but reproducible seed per trial
        std::mt19937 rng(SeedSpecified ? TrialSeed : rd()); // Get a new seed for each trial
        gol::Grid grid(rows, cols, NumAliveCells, rng);

        // Create an iterator for the grid
        gol::Iterator iterator(grid);

        for (size_t j = 0; j < generations; ++j)
        {
            // Take a step and check for still life
            if (iterator.StepSL())
            {
                // Extract the still life pattern
                std::vector<gol::Grid> SLPatterns = grid.ExtractSL();
                // Local uniqueness check
                for (const auto& pattern : SLPatterns)
                {
                    if (rank == 0 && pattern.IsUnique(GlobalKnownSL))
                    {
                        // Let Global rank (rank 0) handle its own checks
                        GlobalKnownSL.emplace_back(std::move(pattern));
                        StillLivesFound++;
                    }
                    else if (pattern.IsUnique(LocalKnownSL))
                    {
                        // Update local list of known still lifes
                        LocalKnownSL.emplace_back(std::move(pattern));

                        // Send the still life pattern to the main process
                        size_t PatternRows = pattern.GetRows();
                        size_t PatternCols = pattern.GetCols();
                        size_t PatternSize = PatternRows * PatternCols;
                        std::unique_ptr<bool[]> PatternCells;
                        PatternCells = pattern.GetCells();

                        // Check that sent data has been received  
                        MPI_Request request[3];

                        MPI_Isend(&PatternRows, 1, MPI_UNSIGNED_LONG, 0, TAG_SL_PATTERN, MPI_COMM_WORLD, &request[0]);
                        MPI_Isend(&PatternCols, 1, MPI_UNSIGNED_LONG, 0, TAG_SL_PATTERN, MPI_COMM_WORLD, &request[1]);
                        MPI_Isend(PatternCells.get(), PatternSize, MPI_CXX_BOOL, 0, TAG_SL_PATTERN, MPI_COMM_WORLD, &request[2]);
                    }
                }
                break; // Exit the loop if a still life is found
            }
        } // End of generations loop

         // Check if stop command
         int stop = 0;
         MPI_Iprobe(0, TAG_FINISHED, MPI_COMM_WORLD, &stop, MPI_STATUS_IGNORE);
         if (stop) 
         {
             // Quota met, stop searching
             break;
         }

        if (rank == 0)
        {
            int flag = 0; // Flag to check if a message is available
            MPI_Status status; // Status object to hold message info

            // Check for messages from any source with the specific tag
            MPI_Iprobe(MPI_ANY_SOURCE, TAG_SL_PATTERN, MPI_COMM_WORLD, &flag, &status);

            if (flag) 
            {
                // A message with TAG_SL_PATTERN is available from status.MPI_SOURCE
                size_t ReceivedRows, ReceivedCols;
                MPI_Recv(&ReceivedRows, 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, TAG_SL_PATTERN, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                MPI_Recv(&ReceivedCols, 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, TAG_SL_PATTERN, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                
                // Create a new grid object with the received data
                std::unique_ptr<bool[]> ReceivedCells = std::make_unique<bool[]>(ReceivedRows * ReceivedCols);
                MPI_Recv(ReceivedCells.get(), ReceivedRows * ReceivedCols, MPI_CXX_BOOL, status.MPI_SOURCE, TAG_SL_PATTERN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Make Grid
                gol::Grid RecievedPattern (ReceivedRows, ReceivedCols, ReceivedCells.get());
                
                // Check Global uniqueness
                if (RecievedPattern.IsUnique(GlobalKnownSL))
                {
                    // Add the still life pattern to the global list
                    GlobalKnownSL.emplace_back(std::move(RecievedPattern));
                    StillLivesFound++;
                }
            }

            if (StillLivesFound >= NumStillLifes)
            {
                // Send message to all processes to stop searching
                int stop = 1;
                for (int p = 1; p < size; ++p)
                {
                    MPI_Send(&stop, 1, MPI_INT, p, TAG_FINISHED, MPI_COMM_WORLD);
                }
                break; // Exit the loop
            }

        } // End of rank 0 check for messages
    } // End of trials loop

    // Barrier to ensure all processes are done printing off their results
    MPI_Barrier(MPI_COMM_WORLD);

    // Print the found still lifes
    if (rank == 0)
    {
        std::cout << "Found " << StillLivesFound << " still lifes across all processes across.\n\n";
        size_t index = 0;
        for (const auto& SL : GlobalKnownSL)
        {
            std::cout << "Still Life: " << index + 1 << "\n";
            SL.print();
            std::cout << "\n";
            index++;
        }
    }

    // Finalse process
    MPI_Finalize();
    return 0;
}
