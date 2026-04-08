#include "GolGrid.h"
#include "GolIterator.h"
#include <iostream> 
#include <omp.h> 
#include <cmath> // For std::sqrt
#include <chrono> // For high_resolution_clock
#include <sstream> // For std::stringstream

class BenchmarkData
{
public:
    BenchmarkData(std::string benchmarkName, unsigned int threads)
        : name(benchmarkName), numThreads(threads) {}
    
    double time;
    std::string name;
    unsigned int numThreads;
    std::string info;

    void start()
    {
        t1 = std::chrono::high_resolution_clock::now();
    }

    void finish()
    {
        std::chrono::high_resolution_clock::time_point t2 =
            std::chrono::high_resolution_clock::now();
        time = (t2 - t1).count() / 1e9;
    }

    std::chrono::high_resolution_clock::time_point t1;
};

inline void parseThreads(std::vector<unsigned int> &threads,
                         const std::string &input)
{
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, ','))
    {

        if (std::stol(token) < 0)
            throw std::invalid_argument(
                "Negative argument in --numthreads option: " + token);
        try
        {
            threads.push_back(std::stoul(token));
        }
        catch (...)
        {
        }
    }
}

std::ostream &operator<<(std::ostream &os, const BenchmarkData &b)
{
    std::cout << "Benchmarking " << b.name << " with " << b.numThreads
              << " threads." << std::endl;
    std::cout << "Time = " << b.time << std::endl;
    std::cout << "Info: " << b.info << std::endl;
    return os;
}

// Torun the benchmark, use the following command:
// build/bin/GolBenchmarks -n <num_threads> --numthreads <num_threads>
// Example: build/bin/GolBenchmarks -n 1,2,4,8
int main(int argc, char **argv)
{
    std::vector<unsigned int> numThreads = {1}; // Default to single thread

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if ((arg == "--numthreads" && (i + 1 < argc)) || (arg == "-n" && (i + 1 < argc)))
        {
            numThreads.clear();
            parseThreads(numThreads, argv[i + 1]);
            ++i;
        }
    }

    std::cout << "Running benchmarks with the following numbers of threads: ";
    for (unsigned int t : numThreads)
    {
        std::cout << t << " ";
    }
    std::cout << std::endl;

    // --- Benchmark Parameters ---

    // Strong scaling parameters
    const size_t strong_rows = 2048;
    const size_t strong_cols = 2048;
    const size_t strong_size = strong_rows * strong_cols;

    // Weak scaling parameters
    const size_t weak_base_rows = 512;
    const size_t weak_base_cols = 512;
    const size_t weak_base_size = weak_base_rows * weak_base_cols;
    size_t sqrt_num_threads = 0; // Used for weak scaling

    // Run params
    const int num_generations = 50; 
    const unsigned int random_seed = 456; // Fixed seed for reproducibility

    std::vector<BenchmarkData> results; // Store results if needed later

    // --- Strong Scaling Benchmark ---

    // Create and initialise random grid *once* 
    std::mt19937 rng(random_seed);
    gol::Grid strong_grid(strong_rows, strong_cols, strong_size*0.25, rng);
    gol::Iterator strong_iter(strong_grid);

    // Make a copy original grid such that each test starts with the same grid
    gol::Grid base_grid = strong_grid;

    // Warm up the OpenMP threads
    std::cout << "\nPerforming warm-up run for benchmark testing..." << std::endl;
    omp_set_num_threads(numThreads[0]); // Use the first thread count for warm-up
    strong_iter.takeStep(); // Run one generation as warm-up
    std::cout << "Warm-up complete.\n" << std::endl;

    for (unsigned int threads : numThreads)
    {
        strong_grid = base_grid; // Reset grid to original state for each test
        gol::Iterator strong_iter(strong_grid); // Re-initialise iterator
    
        omp_set_num_threads(threads); // Set number of threads for OpenMP

        BenchmarkData bench("Strong Scaling", threads);
        bench.info = "Grid: " + std::to_string(strong_rows) + "x" + std::to_string(strong_cols) +
                     ", Gens: " + std::to_string(num_generations);
        
        
        // Run the benchmark
        bench.start();
        for (int gen = 0; gen < num_generations; ++gen)
        {
            strong_iter.takeStep();
        }
        bench.finish();

        results.push_back(bench);
        std::cout << bench << std::endl; // Print result immediately and add new line
    }

    // --- Weak Scaling Benchmark ---

    for (unsigned int threads : numThreads)
    {
        // Get scaled grid size
        sqrt_num_threads = std::sqrt(threads);    
        size_t weak_size = weak_base_size * threads; 
        size_t weak_rows = std::sqrt(weak_size); // sqrt for square grid
        size_t weak_cols = std::sqrt(weak_size);

        // Create and initialise scaled grid
        gol::Grid weak_grid(weak_rows, weak_cols, weak_size*0.25, rng);
        gol::Iterator weak_iter(weak_grid);
        omp_set_num_threads(threads); // Set number of threads for OpenMP

        BenchmarkData bench("Weak Scaling", threads);
        bench.info = "Grid: " + std::to_string(weak_rows) + "x" + std::to_string(weak_cols) +
                     ", Gens: " + std::to_string(num_generations);

        // Run the benchmark 
        bench.start();
        for (int gen = 0; gen < num_generations; ++gen)
        {
            weak_iter.takeStep();
        }
        bench.finish();

        results.push_back(bench);
        std::cout << bench << std::endl; // Print result immediately and add new line
    }

    return 0;
}