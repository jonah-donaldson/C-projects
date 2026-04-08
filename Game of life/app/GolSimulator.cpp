#include <omp.h>
#include <iostream>
#include <fstream> // For std::ifstream, std::ofstream
#include <sstream> 
#include <memory> // For std::unique_ptr
#include <random> // For std::random_device, std::mt19937
#include <chrono> // For std::chrono
#include <thread> // For std::this_thread::sleep_for

#include "GolGrid.h"
#include "GolIterator.h"

// Help message 
void PrintHelp(const std::string &AppName)
{
    std::cout << "Usage:\n"
              << "  " << AppName << " --file <input_filpath> --generations <num_generations> [--output <output_filename>] \n"
              << "  " << AppName << " --random <grid_rows> <grid_cols> <num_alive_cells> [--seed <seed_value>] --generations <num_generations> [--output <output_filename>]\n\n"
              << "Description:\n"
              << "  Conway's Game of Life Simulator\n"
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
    std::string AppName = argv[0]; // Application name for help messages

    // Display help if no arguments are passed
    if (argc == 1) 
    {
        PrintHelp(AppName);
        return 0;
    }

    // --- Command Line Argument Parsing ---//

    bool IsFileMode = false;
    bool IsRandomMode = false;
    std::string FileName;
    size_t rows = 0;
    size_t cols = 0;
    size_t NumAliveCells = 0;
    bool SeedSpecified = false;
    size_t SeedVal = 0;
    bool generations_specified = false; // Track if -g was seen
    size_t generations = 0;
    bool IsOutputFile = false;
    std::string OutputFileName;

    // Parse command line arguments 
    for (int i = 1; i < argc; ++i) 
    {
        // Parse arguments as strings
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") 
        {
            PrintHelp(AppName);
            return 0;
        }

    // File mode check
    else if (arg == "-f" || arg == "--file")
    {
        if (IsRandomMode) 
        {
            std::cerr << "--file and --random options are mutually exclusive\n";
            return 1;
        } 
        else if (i + 1 < argc && argv[i + 1][0] != '-')
        {
            FileName = argv[++i];
            IsFileMode = true;
        } 
        else 
        {
            std::cerr << "Missing filename after --file\n";
            return 1;
        }
    }

    // Random mode check
    else if (arg == "-r" || arg == "--random") 
    {
        if (IsFileMode) 
        {
            std::cerr << "--file and --random options are mutually exclusive\n";
            return 1;
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
            return 1;
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
            return 1;
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
            return 1;
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
            return 1;
        }
    }
} // --- End of command line parsing --- //

// Validate 
if (!IsFileMode && !IsRandomMode)
{
    std::cerr << "Error: Either --file or --random must be specified\n";
    return 1;
}

else if (!generations_specified) 
{
    std::cerr << "Error: --generations must be specified\n";
    return 1;
} 

else if (IsFileMode && SeedSpecified)
{
    std::cerr << "Error: --seed is not valid with --file\n";
    return 1;
} 

if (IsRandomMode)
{
    if (NumAliveCells > rows * cols) 
    {
        std::cerr << "Error: Number of alive cells cannot exceed grid size\n";
        return 1;
    } 

    else if (rows == 0 || cols == 0) 
    {
        std::cerr << "Error: Grid size must be greater than 0\n";
        return 1;
    }
}

// --- Run the simulation --- //

// Initialise the grid based on given options
std::unique_ptr<gol::Grid> grid_ptr; 

// Initialise the grid based on the mode
if (IsFileMode)
{
    // Create GolGrid object from file
    grid_ptr = std::make_unique<gol::Grid>(FileName); // Assign the returned pointer
}
else if (IsRandomMode)
{
    // Create GolGrid object with random initialisation
    std::random_device rd;
    std::mt19937 rng(SeedSpecified ? SeedVal : rd());
    grid_ptr = std::make_unique<gol::Grid>(rows, cols, NumAliveCells, rng); // Assign the returned pointer
}

// Create the iterator
gol::Iterator iterator(*grid_ptr); // Pass the grid to the iterator

// Print initial grid state
std::cout << "Generation: 0 \n";
iterator.printGrid();

// Iterate through generations
for (size_t i = 0; i < generations; ++i) 
{
    iterator.takeStep();
    std::cout << "Generation: " << i + 1 << "\n";
    iterator.printGrid();

    // Optional: Add a delay for better visualisation
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
} 

// Write final grid state to file if specified
if (IsOutputFile) 
{
    // Make destination file
    std::ofstream OutputFile(OutputFileName);
    if (OutputFile)
    {
        // Turn print output into string
        std::stringstream ss;
        auto CoutBuffer = std::cout.rdbuf(); // save original buffer
        std::cout.rdbuf(ss.rdbuf()); // redirect cout to ss

        iterator.printGrid();

        std::cout.rdbuf(CoutBuffer); // restore original buffer
        std::string output = ss.str();
        // Write to file
        OutputFile << output;

        OutputFile.close();
    }
} 

} // End of main function
