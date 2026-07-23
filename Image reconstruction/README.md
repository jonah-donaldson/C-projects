```markdown
# C++ Image Reconstruction & Optimization Library

This project is a high-performance, template-driven C++ library designed to solve complex inverse imaging problems. It uses advanced optimization techniques to reconstruct corrupted, blurred, or heavily sub-sampled images back to their original state. 

By defining imaging issues as mathematical optimization problems, the engine minimizes a cost function using Gradient Descent alongside iterative proximal steps to enforce image smoothness and heal missing data.

---

## Key Features

* **Generic Template Architecture:** Built with heavily templated abstract classes capable of seamlessly processing both real numbers (`double`) and complex frequencies (`std::complex<double>`).
* **Advanced Linear Operators:** Includes custom composite operators, masking matrices, and Convolution operators.
* **Fourier Transforms:** Integrates the **FFTW3** library to map data between real space and frequency space.
* **Iterative Optimization:** Utilizes Proximal Gradient Descent, employing a Gaussian Log-Likelihood differentiable function and a Discrete Cosine Transform (DCT) L1-Norm prior.
* **Robust Testing:** Full suite of unit and integration tests powered by **Catch2**.

---

## The Mathematics

The core algorithm attempts to find an image $x$ that minimizes the following cost function:

$$C(x)=f(x,y,\Phi)+g(x)$$

* $f(x,y,\Phi)$ is the data fidelity term (Gaussian Log-Likelihood) comparing the measurement $y$ to the simulated measurement $\Phi(x)$.
* $g(x)$ is the non-differentiable prior (DCT L1-Norm) that heavily penalizes noise and mathematically enforces spatial smoothness to heal missing data.

---

## Project Structure

```text
/
├── app/              # Application executables (Image & Frequency reconstructors)
├── test/             # Catch2 Unit tests and testing utilities
├── Optimisation/     # Core library source and header files
├── data/             # Input data (corrupted images, frequency measurements, indices)
└── README.md         # Documentation

```

---

## Build Instructions

This project uses **CMake** for building. Ensure you have CMake (3.21+) and the **FFTW3** library installed on your system.

```bash
# 1. Generate the build files
cmake -B build

# 2. Compile the project
cmake --build build

```

---

## Command Line Interface

The generated executables accept the following flags to tweak the optimization algorithm:

| Flag | Description | Type |
| --- | --- | --- |
| `-f` | Path to the input measurements file (`.dat`) | String |
| `-i` | Path to the indices/mask file (Required for subsampling) | String |
| `-k` | Gaussian kernel size (Required for convolution) | Integer |
| `-alpha` | Gradient step size | Double |
| `-beta` | Proximal step size | Double |
| `-sigma` | Variance in the log-likelihood function | Double |
| `-delta` | Convergence tolerance threshold | Double |

---

## Usage Examples

Below are the commands to run the three primary reconstruction applications. All output images (`_dirty.pgm` and `_reconstructed.pgm`) will be routed to the `data/app_out/` directory.

### 1. Convolution (De-blurring)

Reconstructs an image that has been severely blurred by a point-spread function.

```bash
./build/bin/ImageReconstructor -f data/measurements/UtahTeapot_convolved.dat -alpha 1.0 -beta 10000 -sigma 1.0 -delta 0.01 -k 1

```

### 2. Spatial Sub-Sampling (Healing Data Loss)

Reconstructs an image where 80% of the physical pixel data has been completely lost.

```bash
./build/bin/ImageReconstructor -f data/measurements/UtahTeapot_subSampledMeasurements.dat -i data/operators/UtahTeapot_sampledPixels.dat -alpha 1.0 -beta 10000 -sigma 1.0 -delta 0.01

```

### 3. Frequency Reconstruction (Fourier Space)

Reconstructs an image from an incomplete set of complex wave frequencies rather than physical pixels.

```bash
./build/bin/FrequencyReconstructor -f data/measurements/UtahTeapot_frequencyLossMeasurements.dat -i data/operators/UtahTeapot_sampledFrequencies.dat -alpha 1.0 -beta 10000 -sigma 1.0 -delta 0.01

```

---

## Running Tests

To verify the core library math and operator logic, run the compiled Catch2 test suite:

```bash
./build/bin/TestOptimisation

```

---

## Results

### Frequency Reconstruction: Before
![Dirty Image](assets/UtahTeapot_frequencyLossMeasurements_dirty.png)

### Frequency Reconstruction: After
![Reconstructed Image](assets/UtahTeapot_frequencyLossMeasurements_reconstructed.png)

