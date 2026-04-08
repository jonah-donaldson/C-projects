# Game Of Life

## COMP0210 2024-25 Assignment 2
This project implements Conway's Game of Life, a cellular automaton devised by the British mathematician John Horton Conway in 1970. It is a zero-player game, meaning its evolution is determined by its initial state, requiring no further input. One interacts with the Game of Life by creating an initial configuration and observing how it evolves.

This project has three executables in the app folder GolSimulator.cpp, GolSimulatorMPI.cpp which simulate a game of life for given input parmaters the latter encorporating MPI functionality and GolStillLifeFinder which runs through a series of randomly generated grids searching for Still Life patterns, defined as base patterns which remain constant from one generation to the next. 

## Build instructions
This project was with Cmake and already includes necessary .txt files for existing code.

-- Make a Cmake build file
mkdir build
cmake -B build 

-- Compile project
cmake --build build

## Run instructions
### GolSimulator

This program simulates the Game of Life sequentially.

### GolSimulator

*   **From a file:**
    ```sh
    ./app/GolSimulator --file <input_filepath> --generations <num_generations> [--output <output_filename>]
    ```
    *Example:*
    ```sh
    ./app/GolSimulator --file ../patterns/glider.txt --generations 100 --output final_glider.txt
    ```

*   **With a random grid:**
    ```sh
    ./app/GolSimulator --random <rows> <cols> <num_alive> [--seed <seed_value>] --generations <num_generations> [--output <output_filename>]
    ```
    *Example:*
    ```sh
    ./app/GolSimulator --random 20 20 50 --generations 50
    ```
    *Example with seed:*
    ```sh
    ./app/GolSimulator --random 10 10 15 --seed 12345 --generations 20 --output random_seeded.txt
    ```

### GolSimulatorMPI

This program simulates the Game of Life in parallel using MPI.

**Usage:**

Requires `mpirun` or a similar MPI launcher. Replace `<num_processes>` with the desired number of parallel processes.

*   **From a file:**
    ```sh
    mpirun -np <num_processes> ./app/GolSimulatorMPI --file <input_filepath> --generations <num_generations> [--output <output_filename>]
    ```
    *Example:*
    ```sh
    mpirun -np 4 ./app/GolSimulatorMPI --file ../patterns/glider.txt --generations 100
    ```

*   **With a random grid:**
    ```sh
    mpirun -np <num_processes> ./app/GolSimulatorMPI --random <rows> <cols> <num_alive> [--seed <seed_value>] --generations <num_generations> [--output <output_filename>]
    ```
    *Example:*
    ```sh
    mpirun -np 8 ./app/GolSimulatorMPI --random 50 50 500 --generations 200 --output mpi_random_final.txt
    ```

### GolStillLifeFinder

This program searches for Still Life patterns in parallel using MPI.

**Usage:**

Requires `mpirun` or a similar MPI launcher. Replace `<num_processes>` with the desired number of parallel processes.

```sh
mpirun -np <num_processes> ./app/GolStillLifeFinder --random <rows> <cols> <num_alive> --stillLifes <num_to_find> --trials <max_trials> --generations <max_gen_per_trial> [--seed <seed_value>]
```

### Testing and bechmarking
Test and bechmark code for overall methods
Can be found by running
build/bin/GolTests and build/bin/GolBenchmark -n<list of threads> respectively.
eg build/bin/GolBenchmark -n 1,2,4,8: Which benchmarks strong and weak caling for 1,2,4 and 8 threads.


### Overall strucutre
.
├── app
│   ├── CMakeLists.txt
│   ├── GolSimulator.cpp
│   ├── GolSimulatorMPI.cpp
│   ├── GolStillLifeFinder.cpp
│   └── outputs
│       ├── 7x7_15_420_rand_4_MPI.txt
│       ├── 7x7_15_420_rand_4.txt
│       ├── glider26_MPI.txt
│       ├── glider_26.txt
│       ├── p138-oscillator_MPI_138.txt
│       └── p138-oscillator_MPI_69.txt
├── benchmark
│   ├── CMakeLists.txt
│   └── GolBenchmarks.cpp
├── CMakeLists.txt
├── GolLib
│   ├── CMakeLists.txt
│   ├── include
│   │   ├── GolGrid.h
│   │   └── GolIterator.h
│   └── source
│       ├── CMakeLists.txt
│       ├── GolGrid.cpp
│       └── GolIterator.cpp
├── README.md
├── Responses.md
├── test
│   ├── CMakeLists.txt
│   ├── data
│   │   ├── convert.sh
│   │   ├── dart.txt
│   │   ├── glider_badcell.txt
│   │   ├── glider_badgrid.txt
│   │   ├── glider.txt
│   │   ├── gosper_glider_gun.txt
│   │   ├── input.txt
│   │   ├── oscillators.txt
│   │   ├── p138_oscillators.txt
│   │   ├── p40894_oscillators.txt
│   │   └── still_lifes.txt
│   └── GolTests.cpp
└── tree.md

8 directories, 35 files














