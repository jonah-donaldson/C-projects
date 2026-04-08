#include <mpi.h> // For MPI
#include <omp.h> // For OpenMP
#include <iostream>
#include <fstream> // For std::ifstream, std::ofstream
#include <sstream> // For std::ostringstream
#include <vector> // For std::vector
#include <memory> // For std::unique_ptr
#include <random> // For std::random_device, std::mt19937

#include "GolGrid.h"
#include "GolIterator.h"

// Help message 
void PrintHelp(const std::string &AppName)
{
    std::cout << "Usage:\n"
                << "  " << AppName << " --file <input_filpath> --generations <num_generations> [--output <output_filename>] \n"
                << "  " << AppName << " --random <grid_rows> <grid_cols> <num_alive_cells> [--seed <seed_value>] --generations <num_generations> [--output <output_filename>]\n\n"
                << "Description:\n"
                << "  Conway's Game of Life Simulator (MPI version)\n"
                << "  Note that this version uses MPI for parallel processing.\n"
                << "  This program simulates Conway's Game of Life. You can initialise the grid either from a file or randomly.\n"
                << "  The '--generations' option specifies the number of evolution steps simulated after the initial state (Generation 0).\n"
                << "  The '--random' option requires grid dimensions (rows and columns) and the initial number of alive cells.\n"
                << "  The '--seed' option is used to specify a seed for random initialisation. If not specified, a random seed will be used.\n"
                << "  The '--output' option allows you to write final grid state to a .txt file. If not specified, the output will be printed to the console.\n\n"
                << "Options:\n"
                << "  -h, --help            Display this help message and exit\n"
                << "  -f, --file            Specify input file to initialise the grid\n"
                << "  -r, --random          Initialise grid randomly. Requires grid size and number of alive cells\n"
                << "  -s, --seed            Specify a seed for random initialisation (only valid with --random)\n"
                << "  -g, --generations     Number of generations to simulate \n"
                << "  -o, --output           Specify output filename to save the final grid state\n\n"
                << "Note: Both mode (--file or --random) and --generations are required.\n";
}

int main(int argc, char **argv)
{
    // Initialize MPI
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Process id
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Number of processes

    std::string AppName = argv[0]; // Application name for help messages
    
    // Command line vars
    size_t generations = 0; // Number of generations to simulate
    gol::Grid grid; // Grid object
    gol::Grid SubGrid; // Subgrid for each process
    size_t GridRows = 0; // Number of row wise Subgrids
    size_t GridCols = 0; // Number of column wise Subgrids

    bool IsOutputFile = false;
    std::string OutputFileName;


    // Display help if no arguments are passed
    if (argc == 1 && rank == 0) 
    {
        PrintHelp(AppName);
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    // --- Command Line Argument Parsing --//
    if (rank == 0) 
    {
        bool IsFileMode = false;
        bool IsRandomMode = false;
        std::string FileName;
        size_t rows = 0;
        size_t cols = 0;
        size_t NumAliveCells = 0;
        bool SeedSpecified = false;
        size_t SeedVal = 0;
        bool generations_specified = false; 

        // Parse command line arguments 
        for (int i = 1; i < argc; ++i) 
        {
            // Parse arguments as strings
            std::string arg = argv[i];

            if (arg == "-h" || arg == "--help") 
            {
                PrintHelp(AppName);
                MPI_Abort(MPI_COMM_WORLD, 0);
            }

            // File mode check
            else if (arg == "-f" || arg == "--file")
            {
                if (IsRandomMode) 
                {
                    std::cerr << "--file and --random options are mutually exclusive\n";
                    MPI_Abort(MPI_COMM_WORLD, 1);
                } 
                else if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    FileName = argv[++i];
                    IsFileMode = true;
                } 
                else 
                {
                    std::cerr << "Missing filename after --file\n";
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }

            // Random mode check
            else if (arg == "-r" || arg == "--random") 
            {
                if (IsFileMode) 
                {
                    std::cerr << "--file and --random options are mutually exclusive\n";
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }    
                else if (i + 2 < argc && argv[i + 1][0] != '-' && argv[i + 2][0] != '-' && 
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

        } // --- End of command line parsing --- //

        // Validate 
        if (!IsFileMode && !IsRandomMode)
        {
            std::cerr << "Error: Either --file or --random must be specified\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        else if (!generations_specified) 
        {
            std::cerr << "Error: --generations must be specified\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        } 

        else if (IsFileMode && SeedSpecified)
        {
            std::cerr << "Error: --seed is not valid with --file\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        } 

        if (IsRandomMode)
        {
            if (NumAliveCells > rows * cols) 
            {
                std::cerr << "Error: Number of alive cells cannot exceed grid size\n";
                MPI_Abort(MPI_COMM_WORLD, 1);
            } 

            else if (rows == 0 || cols == 0) 
            {
                std::cerr << "Error: Grid size must be greater than 0\n";
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }

        // --- Make Grid objects ---//

        // Initialise the grid based on the mode
        if (IsFileMode)
        {
            // Create GolGrid object from file
            grid = gol::Grid(FileName);
        }
        else if (IsRandomMode)
        {
            // Create GolGrid object with random initialisation
            std::random_device rd;
            std::mt19937 rng(SeedSpecified ? SeedVal : rd());
            grid = gol::Grid(rows, cols, NumAliveCells, rng);
        }
        
        std::cout << "Generation 0:\n";
        grid.print(); // Print initial grid
        std::cout << "\n";

        // Split up grids
        // Looks for most square split of the grid
        // Note for a prime number of processes this becomes a pure row wise split.
        GridRows = std::sqrt(size) + 1; // Catches upper value for non perfect squares
        GridCols = std::sqrt(size);

        if (size % GridRows != 0)
        {
            while (size % GridCols != 0) 
            {
                --GridCols;
            }
            GridRows = size / GridCols;
        }
        else 
        {
            GridCols = size / GridRows;
        }
            
        std::vector<gol::Grid> SubGrids = grid.Cleave(GridRows, GridCols);
        
        SubGrid = SubGrids[0]; // Rank 0 will keep its subgrid
        
        // Distribute the Subgrid data to all processes
        for (int i = 1; i < size; ++i) 
        {
            // Initialise Grid parameters to be sent
            size_t RowsToSend = SubGrids[i].GetRows();
            size_t ColsToSend = SubGrids[i].GetCols();

            // Send the row col data to each process
            MPI_Send(&RowsToSend, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);
            MPI_Send(&ColsToSend, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD);

            // Send the cells data to each process
            MPI_Send(SubGrids[i].GetCells().get(), RowsToSend * ColsToSend, MPI_CXX_BOOL, i, 0, MPI_COMM_WORLD);
        }  

        // Rank 0 will keep its subgrid
        SubGrid = std::move(SubGrids[0]);

    } // End of rank 0 parse

    // Create the subgrid for all other processes
    if (rank != 0) 
    {
        // Store in vars to get adresses (lvalues)
        size_t RowsToReceive;
        size_t ColsToReceive;

        // Receive the row col data from process 0
        MPI_Recv(&RowsToReceive, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&ColsToReceive, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Allocate memory for the cells
        std::unique_ptr<bool[]> cells(new bool[RowsToReceive * ColsToReceive]);

        // Receive the cells data from process 0
        MPI_Recv(cells.get(), RowsToReceive * ColsToReceive, MPI_CXX_BOOL, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        SubGrid = gol::Grid(RowsToReceive, ColsToReceive, cells.get());
    }
    // --- End of grid creation --- //
    
    // Broadcast necessary parameters to all processes (Number of generations).
    MPI_Bcast(&generations, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&GridRows, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&GridCols, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    
    // create 2D Cartesian grid of size GridRows x GridCols
    MPI_Comm cartComm;
    int dims[2] = {static_cast<int>(GridRows), static_cast<int>(GridCols)}; // dimensions of the grid
    int coords[2]; // coordinates of the process in the grid
    int periods[2] = {0, 0}; // non-periodic grid
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &cartComm);

    // Get the coordinates of the current process in the grid
    MPI_Cart_coords(cartComm, rank, 2, coords);
    int nbr_up, nbr_down, nbr_left, nbr_right;
    int nbr_ul, nbr_ur, nbr_dl, nbr_dr;

    // Get the ranks of cardinal neighboring processes
    MPI_Cart_shift(cartComm, 0, 1, &nbr_up, &nbr_down); // up/down neighbors
    MPI_Cart_shift(cartComm, 1, 1, &nbr_left, &nbr_right); // left/right neighbors

   // Calculate and get ranks of diagonal neighbors, handling boundaries
   int diag_coords[2];
   int* nbr_vars[] = {&nbr_ul, &nbr_ur, &nbr_dl, &nbr_dr};
   int offsets[][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}}; // UL, UR, DL, DR

   for (int i = 0; i < 4; ++i) 
   {
       diag_coords[0] = coords[0] + offsets[i][0];
       diag_coords[1] = coords[1] + offsets[i][1];

       if (diag_coords[0] >= 0 && diag_coords[0] < dims[0] && 
           diag_coords[1] >= 0 && diag_coords[1] < dims[1]) 
       {
           MPI_Cart_rank(cartComm, diag_coords, nbr_vars[i]);
       } 
       else 
       {
           *nbr_vars[i] = MPI_PROC_NULL;
       }
   }

    // print subgrids for debugging
    // std::cout << "Rank " << rank << " Subgrid:\n";
    // SubGrid.print();
    // std::cout << "\n";
    
    // // --- Run the simulation --- //
    int nrows = SubGrid.GetRows();
    int ncols = SubGrid.GetCols();

    // Prepare buffers for Sendrecv
    // To send
    std::unique_ptr<bool[]> TopBuf(new bool[ncols]());
    std::unique_ptr<bool[]> BottomBuf(new bool[ncols]());
    std::unique_ptr<bool[]> LeftBuf(new bool[nrows]());
    std::unique_ptr<bool[]> RightBuf(new bool[nrows]()); 

    // To receive
    std::unique_ptr<bool[]> RecvTop(new bool[ncols]()); // Initialise to false. Such that for MPI_Null, cells are dead.
    std::unique_ptr<bool[]> RecvBottom(new bool[ncols]());
    std::unique_ptr<bool[]> RecvLeft(new bool[nrows]());
    std::unique_ptr<bool[]> RecvRight(new bool[nrows]());

    // Corners to recieve. Initialised to false such that for MPI_Null, cells are dead.
    bool recvTL = false, recvTR = false, recvBL = false, recvBR = false;

    // Make sure all processes are ready before starting the simulation
    MPI_Barrier(MPI_COMM_WORLD);

    for (size_t i = 0; i < generations; ++i) 
    {
        // Fill in send buffers
        for (int j = 0; j < ncols; ++j) 
        {
            TopBuf[j] = SubGrid.get(0, j); // Top row
            BottomBuf[j] = SubGrid.get(nrows - 1, j); // Bottom row
        }
        for (int i = 0; i < nrows; ++i) 
        {
            LeftBuf[i] = SubGrid.get(i, 0); // Left column
            RightBuf[i] = SubGrid.get(i, ncols - 1); // Right column
        }

        // Corners
        bool sendTL = SubGrid.get(0, 0);
        bool sendTR = SubGrid.get(0, ncols - 1);
        bool sendBL = SubGrid.get(nrows - 1, 0);
        bool sendBR = SubGrid.get(nrows - 1, ncols - 1);

        // 1) exchange edges with Sendrecv
        MPI_Sendrecv(TopBuf.get(), ncols, MPI_CXX_BOOL, nbr_up, 0,
        RecvBottom.get(), ncols, MPI_CXX_BOOL, nbr_down, 0,
        cartComm, MPI_STATUS_IGNORE);

        MPI_Sendrecv(BottomBuf.get(), ncols, MPI_CXX_BOOL, nbr_down, 1,
        RecvTop.get(), ncols, MPI_CXX_BOOL, nbr_up, 1,
        cartComm, MPI_STATUS_IGNORE);

        MPI_Sendrecv(LeftBuf.get(), nrows, MPI_CXX_BOOL, nbr_left, 2,
        RecvRight.get(), nrows, MPI_CXX_BOOL, nbr_right, 2,
        cartComm, MPI_STATUS_IGNORE);

        MPI_Sendrecv(RightBuf.get(), nrows, MPI_CXX_BOOL, nbr_right, 3,
        RecvLeft.get(), nrows, MPI_CXX_BOOL, nbr_left, 3,
        cartComm, MPI_STATUS_IGNORE);

        // 2) exchange corners with Sendrecv
        MPI_Sendrecv(&sendTL, 1, MPI_CXX_BOOL, nbr_ul, 4,
        &recvBR, 1, MPI_CXX_BOOL, nbr_dr, 4,
        cartComm, MPI_STATUS_IGNORE);

        MPI_Sendrecv(&sendTR, 1, MPI_CXX_BOOL, nbr_ur, 5,
        &recvBL, 1, MPI_CXX_BOOL, nbr_dl, 5,
        cartComm, MPI_STATUS_IGNORE);

        MPI_Sendrecv(&sendBL, 1, MPI_CXX_BOOL, nbr_dl, 6,
        &recvTR, 1, MPI_CXX_BOOL, nbr_ur, 6,
        cartComm, MPI_STATUS_IGNORE);

        MPI_Sendrecv(&sendBR, 1, MPI_CXX_BOOL, nbr_dr, 7,
        &recvTL, 1, MPI_CXX_BOOL, nbr_ul, 7,
        cartComm, MPI_STATUS_IGNORE);

        // 3) build a padded buffer with halo cells
        int PaddedRows = nrows + 2;
        int PaddedCols = ncols + 2;

        // Create a new grid with padding
        gol::Grid PaddedGrid(PaddedRows, PaddedCols);

        // Fill the inner grid with the current subgrid
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < nrows; ++i) 
        {
            for (int j = 0; j < ncols; ++j) 
            {
                PaddedGrid.set(i + 1, j + 1, SubGrid.get(i, j));
            }
        }

        // Fill the top and bottom halo rows
        for (int j = 0; j < ncols; ++j) 
        {
            PaddedGrid.set(0, j + 1, RecvTop[j]); // Top row
            PaddedGrid.set(PaddedRows - 1, j + 1, RecvBottom[j]); // Bottom row
        }
        // Fill the left and right halo cols
        for (int i = 0; i < nrows; ++i) 
        {
            PaddedGrid.set(i + 1, 0, RecvLeft[i]); // Left column
            PaddedGrid.set(i + 1, PaddedCols - 1, RecvRight[i]); // Right column
        }
        // Fill the corners with the received corners
        PaddedGrid.set(0, 0, recvTL);
        PaddedGrid.set(0, PaddedCols - 1, recvTR);
        PaddedGrid.set(PaddedRows - 1, 0, recvBL);
        PaddedGrid.set(PaddedRows - 1, PaddedCols - 1, recvBR);

        // 4) Create an iterator and take a step
        gol::Iterator iterator(PaddedGrid);
        iterator.takeStep();

        // Copy the inner grid back to the subgrid and repeat
        #pragma omp parallel for collapse(2)
        for (int i = 0; i < nrows; ++i) 
        {
            for (int j = 0; j < ncols; ++j) 
            {
                SubGrid.set(i, j, PaddedGrid.get(i + 1, j + 1));
            }
        }
        // Print each generation for debugging
        // std::cout << "Rank " << rank << " Generation " << i + 1 << ":\n";
        // SubGrid.print();
        // std::cout << "\n";
    }
    // --- End of simulation --- //

    // Barrier to ensure all processes are done before sending data
    MPI_Barrier(MPI_COMM_WORLD);

    // Print the overall final grid 
    if (rank != 0) 
    {
        // send Subgrids back to rank 0
        size_t r = SubGrid.GetRows(), c = SubGrid.GetCols();
        MPI_Send(&r, 1, MPI_UNSIGNED_LONG, 0, 100, MPI_COMM_WORLD);
        MPI_Send(&c, 1, MPI_UNSIGNED_LONG, 0, 101, MPI_COMM_WORLD);
        MPI_Send(SubGrid.GetCells().get(), r * c, MPI_CXX_BOOL, 0, 102, MPI_COMM_WORLD);
    }
    else 
    {
        // Rank 0 will receive the subgrids from all other processes
        std::vector<gol::Grid> parts(size);
        parts[0] = std::move(SubGrid); // Keep original rank 0 subgrid
        for (int i = 1; i < size; ++i) 
        {
            // Receive the subgrid from each process
            size_t r, c;
            MPI_Recv(&r, 1, MPI_UNSIGNED_LONG, i, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&c, 1, MPI_UNSIGNED_LONG, i, 101, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::unique_ptr<bool[]> cells(new bool[r * c]);
            MPI_Recv(cells.get(), r * c, MPI_CXX_BOOL, i, 102, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            parts[i] = gol::Grid(r, c, cells.get());
        }
    
        // Merge all parts into a single grid
        size_t rows = grid.GetRows();
        size_t cols = grid.GetCols();

        // Calculate the base dimensions of each sub-grid
        size_t BaseRows = rows / GridRows;
        size_t BaseCols = cols / GridCols;
        size_t ExtraRows = rows % GridRows;
        size_t ExtraCols = cols % GridCols;

        // Loop through the grid and overwrite the cells in the full grid with the subgrid
        for (size_t i = 0; i < GridRows; ++i) 
        {
            for (size_t j = 0; j < GridCols; ++j) 
            {
                size_t StartRow = i * BaseRows + std::min(i, ExtraRows);
                size_t StartCol = j * BaseCols + std::min(j, ExtraCols);
                size_t SubGridRows = BaseRows + (i < ExtraRows ? 1 : 0);
                size_t SubGridCols = BaseCols + (j < ExtraCols ? 1 : 0);

                // Overwrite the cells in the full grid with the subgrid
                for (size_t r = 0; r < SubGridRows; ++r) 
                {
                    for (size_t c = 0; c < SubGridCols; ++c) 
                    {
                        grid.set(StartRow + r, StartCol + c, parts[i * GridCols + j].get(r, c));
                    }
                }
            }
        }

        // Print the final grid
        std::cout << "Final Grid:\n" << "Generation " << generations << ":\n";
        grid.print();
        std::cout << "\n";
        
        if (IsOutputFile) 
        {
            std::ofstream ofs(OutputFileName);
            if (!ofs) 
            {
                std::cerr << "Error: cannot open output file " << OutputFileName << "\n";
            } 
            else 
            {
                // capture `full.print()` into a string
                std::ostringstream oss;
                auto oldbuf = std::cout.rdbuf(oss.rdbuf());
                grid.print();
                std::cout.rdbuf(oldbuf);
        
                ofs << oss.str();
            }
        }
    } // End of rank 0

    MPI_Finalize(); // Finalize MPI
    return 0; // Return success

} // End of main

