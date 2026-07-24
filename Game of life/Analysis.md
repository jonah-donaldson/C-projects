# Architecture & Performance Analysis

## MPI Domain Decomposition Strategy

In Conway's Game of Life, calculating the next state of a cell requires information about its adjacent neighbors. In a distributed memory model, most cells can be updated independently within their sub-grid. However, cells on the boundaries require neighboring state data that resides on different processes. 

To resolve this, the system must perform an exchange of boundary information—often referred to as "halo cells"—at the end of each generation step. Two primary domain decomposition strategies were evaluated for this implementation.

### Approach 1: 1D Row/Column Decomposition
The global grid (N x M) is divided evenly along one axis across `P` processes. Each process handles a sub-grid of `(N/P) x M` dimensions. 
* **Communication Overhead:** Communication is only required for the top and bottom rows of each sub-grid. Each process executes two send and two receive operations. 
* **Data Transfer:** The size of each transfer is equal to the column length `M`. Thus, `4M` cell states are sent and received per step (except for edge blocks, which transfer `2M`).

### Approach 2: 2D Block Decomposition
The global grid is divided into `P` rectangular sub-blocks arranged in a `P_r x P_c` grid (where `P = P_r * P_c`). Each process is assigned a sub-block of size `(N/P_r) x (M/P_c)`. 
* **Communication Overhead:** To compute updates at the boundaries, the system uses an optimized **2D Halo Exchange** pattern. Left/Right edges are swapped first, and the resulting corner cells hitch a ride on the subsequent Top/Bottom exchanges. This restricts the network traffic to exactly four send and four receive operations per process, completely eliminating the latency of separate diagonal routing.
* **Data Transfer:** For a single step, the total data transferred (sent and received) per process is `4(N/P_r + M/P_c + 2)` cells.

### Architectural Decision
**2D Block Decomposition** was selected for the final MPI implementation. 

Because synchronization and network communication create the primary bottleneck in distributed cellular automata, the objective is to minimize message sizes per process. In Block Decomposition, as `P` grows larger, the communication surface area shrinks significantly relative to the computation volume per process. This yields a much better surface-to-volume ratio than 1D decomposition, heavily optimizing large-scale parallel runs. *(Note: Leftover rows/columns from uneven grid splits are distributed dynamically among the sub-grids).*

---

## Parallel Scaling & Benchmarks

Performance was evaluated on both Strong Scaling and Weak Scaling models to determine the communication overhead and parallel efficiency of the application.

### 1. Strong Scaling
**Goal:** Solve a fixed problem size faster by providing more compute resources.  
**Theoretical Expectation:** Linear speedup. Doubling the number of threads should halve the execution time `(Speedup = P)`.

**Test Parameters:**
* Global Grid: 2048 x 2048
* Generations: 50

| Threads | Execution Time (s) | Scaling Observation |
| :---: | :--- | :--- |
| **2** | 0.8313 | Baseline |
| **4** | 0.3916 | ~2.12x speedup (Expected linear scaling) |
| **8** | 0.3238 | ~1.20x speedup (Diminishing returns) |
| **12** | 0.3228 | ~1.00x speedup (Saturated) |

**Analysis:**
The implementation achieves near-perfect linear scaling from 2 to 4 threads. However, performance saturates beyond 8 threads. Because the overall grid size is fixed, adding more threads decreases the computational work per thread. Eventually, the synchronization and communication overhead between the threads overshadows the computation time, causing the performance gains to flatline.

### 2. Weak Scaling
**Goal:** Solve increasingly larger problems in the same amount of time by scaling threads proportionally with the problem size.  
**Theoretical Expectation:** Constant execution time. As threads `N` and problem size `P` increase, the workload per thread remains identical.

**Test Parameters:**
* Workload per thread maintained proportionally to grid size.
* Generations: 50

| Threads | Grid Size | Execution Time (s) | Scaling Observation |
| :---: | :--- | :--- | :--- |
| **2** | 724 x 724 | 0.0983 | Baseline |
| **4** | 1024 x 1024 | 0.1068 | ~8.5% increase (Near ideal) |
| **8** | 1448 x 1448 | 0.1864 | ~74.5% increase (Overhead impact) |
| **12** | 1773 x 1773 | 0.2931 | ~57.2% increase (Overhead impact) |

**Analysis:**
Weak scaling holds steady up to 4 threads, with execution time remaining nearly constant as the problem size expands. However, moving to 8 and 12 threads introduces an execution time jump of roughly ~100ms per step. This drift from the ideal constant time is due to the inherent overhead of managing a larger pool of threads—while the theoretical computation *per thread* is the same, the aggregate cost of system-level thread synchronization and data bus contention introduces measurable latency at higher thread counts.