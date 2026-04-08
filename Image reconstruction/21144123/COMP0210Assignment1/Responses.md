# Respones

- Please place your responses in this file. 
- You should use clear headings for each response so that they are easy to find. 

Part 2.3 
The limitations of the relevant IOA test on the x^2 function are as follow.

- **Triviality:**  
  The quadratic function is very well-behaved and convex, which does not reflect the complexity of real-world optimisation problems.

- **Lack of Complexity:**  
  This test does not consider non-linearities, noise, or irregularities present in more realistic scenarios such as image reconstruction.

- **Parameter Sensitivity:**  
  The performance of the test is highly sensitive to the choice of the step size α and the tolerance. Tuning these for a trivial function may not translate to proper behavior in more challenging problems.

- **Singular Scenario:**  
  Testing with only one type of function does not guarantee that the algorithm will perform well with other types.

Part 4.3 
By nature of C(x) its output will be a double since the two functions Gaussian and DCL1NORM both produce abs nomalisations.

For other functions the output may be checked by if (std::is_same<T, std::complex<double>>::value) and an error throw. 

Part 5 part 1

- **Compile code:**  
g++ -std=c++17 -IOptimisation/include -IOptimisation/src \
    app/ImageReconstructor.cpp \
    Optimisation/src/ImageUtils.cpp \
    Optimisation/src/OptimisationUtils.cpp \
    -lfftw3 -lm \
    -o app/ImageReconstructor

- **Run Convolution:**  
app/ImageReconstructor "data/measurements/UtahTeapot_convolved.dat"  1.0 1e4 1.0 0.1 "-k" 3

- **Run Sub Sampling:**  
app/ImageReconstructor "data/measurements/UtahTeapot_subSampledMeasurements.dat" 1.0 1e4 1.0 0.1 "-i" "data/operators/UtahTeapot_sampledFrequencies.dat"

Part 6.2







