# Responses

## Part 2.4 Running the application
### For random 7 by 7 grid with 15 alive cells and 4 generations
Command line input: build/bin/GolSimulator -r 7 7 15 -s 420 -g 4

Output:
Generation: 0 
- o o - - - o 
- - - - - - - 
o o - - - - o 
- - o - - - o 
o o - - - o o 
o - - - o - - 
- - - - o - - 

Generation: 1
- - - - - - - 
o - o - - - - 
- o - - - - - 
- - o - - - o 
o o - - - o o 
o o - - o - - 
- - - - - - - 

Generation: 2
- - - - - - - 
- o - - - - - 
- o o - - - - 
o - o - - o o 
o - o - - o o 
o o - - - o - 
- - - - - - - 

Generation: 3
- - - - - - - 
- o o - - - - 
o - o - - - - 
o - o o - o o 
o - o - o - - 
o o - - - o o 
- - - - - - - 

Generation: 4
- - - - - - - 
- o o - - - - 
o - - - - - - 
o - o - o o - 
o - o - o - - 
o o - - - o - 
- - - - - - - 

### For Glider file input
Command line input: build/bin/GolSimulator -f test/data/glider.txt -g 26

Output:
Generation: 0 
- - - - - - - - - - 
- - o - - - - - - - 
o - o - - - - - - - 
- o o - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 1
- - - - - - - - - - 
- o - - - - - - - - 
- - o o - - - - - - 
- o o - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 2
- - - - - - - - - - 
- - o - - - - - - - 
- - - o - - - - - - 
- o o o - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 3
- - - - - - - - - - 
- - - - - - - - - - 
- o - o - - - - - - 
- - o o - - - - - - 
- - o - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 4
- - - - - - - - - - 
- - - - - - - - - - 
- - - o - - - - - - 
- o - o - - - - - - 
- - o o - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 5
- - - - - - - - - - 
- - - - - - - - - - 
- - o - - - - - - - 
- - - o o - - - - - 
- - o o - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 6
- - - - - - - - - - 
- - - - - - - - - - 
- - - o - - - - - - 
- - - - o - - - - - 
- - o o o - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 7
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - o - o - - - - - 
- - - o o - - - - - 
- - - o - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 8
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - o - - - - - 
- - o - o - - - - - 
- - - o o - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 9
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - o - - - - - - 
- - - - o o - - - - 
- - - o o - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 10
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - o - - - - - 
- - - - - o - - - - 
- - - o o o - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 11
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - o - o - - - - 
- - - - o o - - - - 
- - - - o - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 12
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - o - - - - 
- - - o - o - - - - 
- - - - o o - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 13
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - o - - - - - 
- - - - - o o - - - 
- - - - o o - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 14
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - o - - - - 
- - - - - - o - - - 
- - - - o o o - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 15
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - o - o - - - 
- - - - - o o - - - 
- - - - - o - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 16
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - o - - - 
- - - - o - o - - - 
- - - - - o o - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 17
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - o - - - - 
- - - - - - o o - - 
- - - - - o o - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 18
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - o - - - 
- - - - - - - o - - 
- - - - - o o o - - 
- - - - - - - - - - 
- - - - - - - - - - 

Generation: 19
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - o - o - - 
- - - - - - o o - - 
- - - - - - o - - - 
- - - - - - - - - - 

Generation: 20
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - o - - 
- - - - - o - o - - 
- - - - - - o o - - 
- - - - - - - - - - 

Generation: 21
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - o - - - 
- - - - - - - o o - 
- - - - - - o o - - 
- - - - - - - - - - 

Generation: 22
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - o - - 
- - - - - - - - o - 
- - - - - - o o o - 
- - - - - - - - - - 

Generation: 23
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - o - o - 
- - - - - - - o o - 
- - - - - - - o - - 

Generation: 24
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - o - 
- - - - - - o - o - 
- - - - - - - o o - 

Generation: 25
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - o - - 
- - - - - - - - o o 
- - - - - - - o o - 

Generation: 26
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - o - 
- - - - - - - - - o 
- - - - - - - o o o

## Part 3.2 Strong weak scaling
### Strong Scaling
Strong Scaling refers to how the solution time varies with the number of threads for a fixed total problem size. The goal is to solve the same problem faster by using more threads.

Expectation: 
Ideally, doubling the number of threads should halve the execution time, resulting in linear speedup. Speedup is calculated as `T(1) / T(P)`, where `T(P)` is the time with `P` threads. Thus for linear speedup, Speedup(P) = P.

### Weak Scaling
Weak Scaling refers to how the solution time varies with the number of threads for a fixed problem size per processor. The goal is to solve increasingly larger total problems in roughly the same amount of time by scaling threads proportionally with problem size.

Expectation: 
Ideally, as the number of threads `N` and the total problem size `P` increase such that `P/N` remains constant, where the *weight* on each overall thread is the same.

### Results:
Command line input: build/bin/GolBenchmarks -n 2,4,8,12

Output:
Running benchmarks with the following numbers of threads: 2 4 8 12 

Performing warm-up run for benchmark testing...
Warm-up complete.

Benchmarking Strong Scaling with 2 threads.
Time = 0.831379
Info: Grid: 2048x2048, Gens: 50

Benchmarking Strong Scaling with 4 threads.
Time = 0.391651
Info: Grid: 2048x2048, Gens: 50

Benchmarking Strong Scaling with 8 threads.
Time = 0.323859
Info: Grid: 2048x2048, Gens: 50

Benchmarking Strong Scaling with 12 threads.
Time = 0.322815
Info: Grid: 2048x2048, Gens: 50

Benchmarking Weak Scaling with 2 threads.
Time = 0.0983903
Info: Grid: 724x724, Gens: 50

Benchmarking Weak Scaling with 4 threads.
Time = 0.106836
Info: Grid: 1024x1024, Gens: 50

Benchmarking Weak Scaling with 8 threads.
Time = 0.186475
Info: Grid: 1448x1448, Gens: 50

Benchmarking Weak Scaling with 12 threads.
Time = 0.293126
Info: Grid: 1773x1773, Gens: 50

### Observations
Strong Scaling:
- Showed expected scaling speedup for 2 -> 4 threads, roughly linear.
- Poor scaling 4 -> 8 threads, far less tha expected linear speedup.
- Saturated 8 -> 12 threads, Almost no performance gain.

Weak Scaling:
- Showed close to expected scaling 2 -> 4 threads, nearly same processing time
- Poor effiency for 4 -> 8 -> 12 threads, process time jumped nearly 100 ms for each jump

Reasoning
*Strong Scaling*: 
Performance saturated because, for a fixed grid size, the communication/synchronisation overhead between threads became dominant compared to the decreasing computational work per thread as more threads were added.

*Weak Scaling:*
Execution time increased because of thread ability to effectively parallelise for problem size which introduces overhead on top of overall process time.

## Part 4.1 Grid division discussion.
Adjacent neighbours are required to calculate the state of any given cell after each step. Most points can meet this condition in a sub-grid except the ones on boundaries between cells. To remedy this, the overall method must allow grids to exchange boundary information to account for these "halo cells" at each step. This exchange for any boundary would require that each sub-grid send its "halo cell" information and receive halo cell information to and from the relevant block. 

-- Approach the first. 
Row decomposition.
The Grid could be evenly split up row-wise into P processes. For N rows, each process would handle N/P rows, each containing M cols for an NxM global grid. Communication between processes would only be needed for each subgrid's top and bottom rows, respectively (bar the top and bottom subgrids). Each process has two send and two receive messages for four transfers. The size of each transfer is equal to the col length. Thus, 4M cell states are sent and received per step. 
(In the case of the top or bottom row, it is 2M, as only one edge needs to communicate).

(This approach can also be done by swapping rows and cols in the above description.)

-- Approach the second.
Block decomposition. 
The N x M grid is divided into P rectangular sub-blocks, arranged in a P_r by P_c grid (where P = P_r * P_c). Each process is assigned one sub-block of size (N/P_r) * (M/P_c). To compute updates at boundaries, four send and four receive messages must occur per process for an internal block (blocks on the edges and corners would only have 6 and 4, respectively). For a single step, a transfer of 8(N/P_r + M/P_c + 1) cells would be required, where the + 1 comes from the 4 diagonal terms. 

-- Choice. 
Given that each process can be run in parallel and all processes bottleneck the overall generation step, we want smaller processes and smaller message sizes per process for best results. One can immediately note that P is inversely proportional to the overall message size in block decomposition. Thus, for large P block decomposition, minimises communication surface area relative to computation volume per process, a benefit not seen my row or col decomposition. Therefore, Block decomposition was chosen for the MPI program.  
Note: In cases where Grid could not be split evenly leftover rows and cols were split up evenly among cells.

## Part 4.5 Running MPI application 
### Glider (Same as before)
Command Line input: mpirun -np 4 build/bin/GolSimulatorMPI -f test/data/glider.txt -g 26

Output:
Generation 0:
- - - - - - - - - - 
- - o - - - - - - - 
o - o - - - - - - - 
- o o - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 

Final Grid:
Generation 26:
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - - - 
- - - - - - - - o - 
- - - - - - - - - o 
- - - - - - - o o o 

### Random 7 by 7 grid (Same as before)
Command line input: mpirun -np 4 build/bin/GolSimulatorMPI -r 7 7 15 -s 420 -g 4

Output:
Generation 0:
- o o - - - o 
- - - - - - - 
o o - - - - o 
- - o - - - o 
o o - - - o o 
o - - - o - - 
- - - - o - - 

Final Grid:
Generation 4:
- - - - - - - 
- o o - - - - 
o - - - - - - 
o - o - o o - 
o - o - o - - 
o o - - - o - 
- - - - - - - 

### P-138 Oscillator, half oscillation g = 69
Command line input: mpirun -np 4 build/bin/GolSimulatorMPI -f test/data/p138_oscillators.txt -g 69

Output:
Generation 0:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - o o o - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - o - - o - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - o - - - o - - - - - - - - - - - - - 
- - - - - - - - - - - - o - - - - - o o o - - - - - - - - - - - - - - 
- - - - - - - - - - - - - o - - - - - o - - - - - - - - - - - - - - - 
- - - - - - - - - - o o - o o - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - o - - o - - - - - - - - - o - - - - - - - - - - - 
- - - - - - - - - - o - o - - - - - - - - - o - o - - - - - - - - - - 
- - - - - - - - - - - o - - - - - - - - - o - - o - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - o o - o o - - - - - - - - - - 
- - - - - - - - - - - - - - - o - - - - - o - - - - - - - - - - - - - 
- - - - - - - - - - - - - - o o o - - - - - o - - - - - - - - - - - - 
- - - - - - - - - - - - - o - - - o - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - o - - o - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - o o o - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Final Grid:
Generation 69:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - o o o - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - o - - o - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - o - - - o - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - o o o - - - - - o - - - - - - - - - - - - 
- - - - - - - - - - - - - - - o - - - - - o - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - o o - o o - - - - - - - - - - 
- - - - - - - - - - - o - - - - - - - - - o - - o - - - - - - - - - - 
- - - - - - - - - - o - o - - - - - - - - - o - o - - - - - - - - - - 
- - - - - - - - - - o - - o - - - - - - - - - o - - - - - - - - - - - 
- - - - - - - - - - o o - o o - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - o - - - - - o - - - - - - - - - - - - - - - 
- - - - - - - - - - - - o - - - - - o o o - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - o - - - o - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - o - - o - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - o o o - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

### P-138 Oscillator, full oscillation g = 138
Command line input: mpirun -np 4 build/bin/GolSimulatorMPI -f test/data/p138_oscillators.txt -g 138

Output:
Generation 0:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - o o o - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - o - - o - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - o - - - o - - - - - - - - - - - - - 
- - - - - - - - - - - - o - - - - - o o o - - - - - - - - - - - - - - 
- - - - - - - - - - - - - o - - - - - o - - - - - - - - - - - - - - - 
- - - - - - - - - - o o - o o - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - o - - o - - - - - - - - - o - - - - - - - - - - - 
- - - - - - - - - - o - o - - - - - - - - - o - o - - - - - - - - - - 
- - - - - - - - - - - o - - - - - - - - - o - - o - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - o o - o o - - - - - - - - - - 
- - - - - - - - - - - - - - - o - - - - - o - - - - - - - - - - - - - 
- - - - - - - - - - - - - - o o o - - - - - o - - - - - - - - - - - - 
- - - - - - - - - - - - - o - - - o - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - o - - o - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - o o o - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

Final Grid:
Generation 138:
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - o o o - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - o - - o - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - o - - - o - - - - - - - - - - - - - 
- - - - - - - - - - - - o - - - - - o o o - - - - - - - - - - - - - - 
- - - - - - - - - - - - - o - - - - - o - - - - - - - - - - - - - - - 
- - - - - - - - - - o o - o o - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - o - - o - - - - - - - - - o - - - - - - - - - - - 
- - - - - - - - - - o - o - - - - - - - - - o - o - - - - - - - - - - 
- - - - - - - - - - - o - - - - - - - - - o - - o - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - o o - o o - - - - - - - - - - 
- - - - - - - - - - - - - - - o - - - - - o - - - - - - - - - - - - - 
- - - - - - - - - - - - - - o o o - - - - - o - - - - - - - - - - - - 
- - - - - - - - - - - - - o - - - o - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - o - - o - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - o o o - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

## Part 5.3 Still life finder

### Random search 
Note: Detecs roations of same pattern as different still life.

Command line input: mpirun -np 4 build/bin/GolStillLifeFinder -r 4 4 8 -n 20 -t 100 -g 
30 -s 420

Output:

Process 1 is running with the following parameters:
Grid Size: 4 x 4
Number of Alive Cells: 8
Generations: 30
Still Lifes: 20
Max Trials: 100

Found 19 still lifes across all processes across.

Still Life: 1
- - - - - 
- o o - - 
- o - o - 
- - o - - 
- - - - - 

Still Life: 2
- - - - - - 
- - o o - - 
- o - - o - 
- o - - o - 
- - o o o - 
- - - - - - 

Still Life: 3
- - - - - - 
- - o - - - 
- o - o - - 
- o - - o - 
- - o o - - 
- - - - - - 

Still Life: 4
- - - - - - 
- o o - - - 
- o - - o - 
- - o o o - 
- - - - - - 

Still Life: 5
- - - - - - 
- - o o - - 
- o - - o - 
- - o o - - 
- - - - - - 

Still Life: 6
- - - - 
- o o - 
- o o - 
- - - - 

Still Life: 7
- - - - - - 
- - o o - - 
- o - - o - 
- - o - o - 
- - - o o - 
- - - - - - 

Still Life: 8
- - - - - 
- - o - - 
- o - o - 
- - o o - 
- - - - - 

Still Life: 9
- - - - - 
- - o - - 
- o - o - 
- o - o - 
- - o - - 
- - - - - 

Still Life: 10
- - - - - 
- - o o - 
- o - o - 
- o - o - 
- - o - - 
- - - - - 

Still Life: 11
- - - - - 
- o o - - 
- o - o - 
- o - o - 
- - o - - 
- - - - - 

Still Life: 12
- - - - - - 
- - o o - - 
- o - - o - 
- - o - o - 
- - - o - - 
- - - - - - 

Still Life: 13
- - - - - 
- - o o - 
- o - o - 
- - o - - 
- - - - - 

Still Life: 14 (Croissant)
- - - - - - 
- - o o o - 
- o - - o - 
- o - - - - 
- o o - - - 
- - - - - - 

Still Life: 15
- - - - - 
- - o - - 
- o - o - 
- o o - - 
- - - - - 

Still Life: 16
- - - - - 
- o o - - 
- o - - - 
- o - o - 
- - o o - 
- - - - - 

Still Life: 17
- - - - - 
- - o - - 
- o - o - 
- - o - - 
- - - - - 

Still Life: 18
- - - - - - 
- - o - - - 
- o - o - - 
- o - - o - 
- - o o o - 
- - - - - - 

Still Life: 19
- - - - - - 
- - o o o - 
- o - - o - 
- o - - o - 
- - o o - - 
- - - - - - 