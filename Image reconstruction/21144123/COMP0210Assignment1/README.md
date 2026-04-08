# Image Reconstruction
## COMP0210 Assignment 1

# Image Reconstruction
## COMP0210 Assignment 1

### Overview
This project implements an image reconstruction algorithm using optimization techniques. The main components include:
- **Reconstruction Algorithm:** Uses gradient decent and iterative proximal steps to reach a minimum.
- **Operators:** Such as convolution and Fourier transforms.
- **Testing Framework:** Unit tests based on Catch2 for verifying functionality.
- **Convergence Criteria:** Custom convergence function for iterative minimization.

### Project Structure
```
/workspaces/COMP0210Assignment1/
├── app/              # Application source code (e.g., ImageReconstructor.cpp)
├── test/             # Unit tests and testing utilities
├── Optimisation/     # Optimisation library source and header files
├── data/             # Data files (images, measurements)
├── Responses.md      # Project responses and reflections
└── README.md         # This file
```

### Build Instructions
Compile the project using the following command:
```bash
g++ -std=c++17 -IOptimisation/include -IOptimisation/src \
    app/ImageReconstructor.cpp \
    Optimisation/src/ImageUtils.cpp \
    Optimisation/src/OptimisationUtils.cpp \
    -lfftw3 -lm \
    -o app/ImageReconstructor
```

### Input key
<image_path> <alpha> <beta> <sigma> <tol> (-k <kernel_size> | -i <indices_file_path>)

### Run instrcutions 
Convolution Example:
app/ImageReconstructor "data/measurements/UtahTeapot_convolved.dat" 1.0 1e4 1.0 0.1 "-k" 3

Sub Sampling Example:
app/ImageReconstructor "data/measurements/UtahTeapot_subSampledMeasurements.dat" 1.0 1e4 1.0 0.1 "-i" "data/operators/UtahTeapot_sampledFrequencies.dat"

### Run tests
Build build file

mkdir build  
cmake -B build

Compiling  
cmake --build build

Run code  
./build/bin/TestOptimisation