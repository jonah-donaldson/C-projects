#pragma once

#include <vector>
#include <stdexcept>
#include <string>
#include "DataPack.h"
#include "ImageUtils.h"
#include "fftw3.h"
#include "VectorUtils.h"

using std::vector;
using std::size_t;

/**
 * @brief Base class for linear operators.
 * 
 * Provides interfaces for the forward transform and its adjoint.
 */
template <typename T, typename O>
class LinearOperator 
{
public:
    // Forward transform: y = Φ(x)
    virtual vector<O> operator()(const vector<T>& x) = 0;

    // Adjoint transform: x = Φ^(dagger)(y)
    virtual vector<T> adjoint(const vector<O>& y) = 0;

    // Virtual destructor ensures proper cleanup for derived classes.
    virtual ~LinearOperator() {}
};

/**
 * @brief Identity linear operator.
 * 
 * Simply returns the input vector for both operator() and adjoint.
 */
class Identity : public LinearOperator<double,double>
{
public:
    vector<double> operator()(const vector<double>& v) override 
    { 
        return v; 
    }

    vector<double> adjoint(const vector<double>& v) override 
    { 
        return v; 
    }
};

/**
 * @brief SubSampler linear operator.
 * 
 * Extracts a subset of the input vector using the provided indices.
 * The adjoint operator maps the subsampled vector back to its original full space.
 */
//TODO: Impliment error handling properly.

class SubSampler : public LinearOperator<double,double>
{
private:
    // Indices specifying the positions to extract from the input vector.
    std::vector<size_t> indices;
    
    // Expected size of the full input vector for the adjoint operation.
    size_t full_size = 0;

public:
    //Constructor for SubSampler.
    //Takes a vector of indices to subsample.
    SubSampler(const std::vector<size_t>& indices_in) 
        : indices(indices_in) {}

    //Forward transform: y = Φ(x)
    vector<double> operator()(const vector<double>& x) override 
    {
        // Overwrite the full_size.
        full_size = x.size();
        
        // Initialize the subsampled vector.
        vector<double> y;
        y.reserve(indices.size());

        // Loop over the indices and extract the corresponding elements.
        for (size_t idx : indices) 
        {
            if (idx >= x.size()) 
            {
                // Throw an error if the index is out of range.
                throw std::out_of_range("Index " + std::to_string(idx) + " is out of range for the input vector.");
            }
            y.push_back(x[idx]);
        }
        return y;
    }
    
    //Adjoint transform: x' = Φ^(dagger)(y)
    vector<double> adjoint(const vector<double>& y) override 
    {
        if (full_size == 0)
        {
            throw std::runtime_error("Full size of the input vector is not set.");
        }

        // Initialize full vector with default zeros.
        vector<double> x_prime(full_size, 0.0);

        // Map each element from the subsampled vector back to the corresponding position.
        for (size_t i = 0; i < indices.size(); ++i) 
        {
            size_t idx = indices[i];
            if (idx >= full_size) 
            {
                throw std::out_of_range("Index " + std::to_string(idx) + " is out of range for the full vector.");
            }
            x_prime[idx] = y[i];
        }
        return x_prime;
    }

};

/**
 * @brief Convolution linear operator.
 * 
 * Applies a convolution operation to the input image using a given image kernel.
 */
class Convolution : public LinearOperator<double,double>
{
private:
    // Kernel data, height and width.
    vector<double> kernel_data;
    size_t image_width;
    size_t image_height;

    // Initialise Buffers buffer
    fftw_complex * input_buffer;
    fftw_complex * output_buffer;
    fftw_complex * kernel_fdata;

    // Initialise plans 
    fftw_plan forward_plan;
    fftw_plan backward_plan;

    // Epsilon value for numerical stability.
    double epsilon;

public:
    // Constructor for Convolution operator.
    // Takes in kernel data, image width and height, and optional epsilon value (default = 1e-2).
    Convolution(vector<double> kernel_data, size_t image_width, size_t image_height, double epsilon = 1e-2)
        : kernel_data(kernel_data), image_width(image_width), image_height(image_height), epsilon(epsilon) 
        {
            // Check if kernel size is compatible with image size.
            if (kernel_data.size() != image_width * image_height)
            {
                throw std::runtime_error("Kernel size " + std::to_string(kernel_data.size()) + " does not match image size " + std::to_string(image_width * image_height));
            }

            // Initialise input output buffers as complex arrays.
            input_buffer = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * kernel_data.size());
            output_buffer = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * kernel_data.size());

            // Initialise forward plan.
            forward_plan = fftw_plan_dft_2d(
                image_height,
                image_width,
                input_buffer,
                output_buffer,
                FFTW_FORWARD,
                FFTW_MEASURE);

            // Initialise backward plan.
            backward_plan = fftw_plan_dft_2d(
                image_height,
                image_width,
                input_buffer,
                output_buffer,
                FFTW_BACKWARD,
                FFTW_MEASURE);

            // Compute kernel buffer.
            for (size_t i = 0; i < kernel_data.size(); ++i)
            {
                input_buffer[i][0] = kernel_data[i];
                input_buffer[i][1] = 0.0;
            }

            // Perform the forward Fourier transform.
            kernel_fdata = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * kernel_data.size());
            fftw_execute(forward_plan);
            
            // Copy the output buffer to kernel_fdata.
            for (size_t i = 0; i < kernel_data.size(); ++i)
            {
                kernel_fdata[i][0] = output_buffer[i][0];
                kernel_fdata[i][1] = output_buffer[i][1];
            }
        }

    // Destructor for Convolution operator.
    ~Convolution()
    {
        // Free the memory.
        fftw_free(kernel_fdata);
        fftw_free(input_buffer);
        fftw_free(output_buffer);
        fftw_destroy_plan(forward_plan);
        fftw_destroy_plan(backward_plan);
    }

    // Forward Convolution: x' = Φ(x)
    vector<double> operator()(const vector<double>& x) override
    {
        // Check if the input size is compatible with the image size.
        if (x.size() != image_width * image_height)
        {
            throw std::runtime_error("Input size " + std::to_string(x.size()) + " does not match image size " + std::to_string(image_width * image_height));
        }

        // Initalise the input buffer as x and zeros.
        for (size_t i = 0; i < x.size(); ++i)
        {
            input_buffer[i][0] = x[i];
            input_buffer[i][1] = 0.0;
        }
        
        // Perform the forward Fourier transform.
        // x -> x_f
        fftw_execute(forward_plan);

        // Perform the convolution in the Fourier domain.
        // xf' = xf * kernel_f
        // Where xf is the output_buffer and xf_prime is input buffer for backwards step.
        for (size_t i = 0; i < x.size(); ++i)
        {
            double a = output_buffer[i][0];
            double b = output_buffer[i][1];
            double c = kernel_fdata[i][0];
            double d = kernel_fdata[i][1];

            input_buffer[i][0] = a * c - b * d;
            input_buffer[i][1] = a * d + b * c;
        }
        
        // Perform the backward Fourier transform.
        // xf' -> x'
        fftw_execute(backward_plan);

        // Return x_prime as real vector.
        vector<double> x_prime(x.size());
        for (size_t i = 0; i < x.size(); ++i)
        {
            x_prime[i] = output_buffer[i][0];
            x_prime[i] = x_prime[i]/(x.size());
        }
        
        return x_prime;
    }

    // Adjoint transform: x = Φ^(dagger)(x')
    vector<double> adjoint(const vector<double>& x_prime) override
    {
        if (x_prime.size() != image_width * image_height)
        {
            throw std::runtime_error("Input size " + std::to_string(x_prime.size()) + " does not match image size " + std::to_string(image_width * image_height));
        }

        // Initalise the input buffer as x' and zeros.
        for (size_t i = 0; i < x_prime.size(); ++i)
        {
            input_buffer[i][0] = x_prime[i];
            input_buffer[i][1] = 0.0;
        }
        
        // Perform the forward Fourier transform.
        // x' -> xf'
        fftw_execute(forward_plan);

        // Perform the deconvolution in the Fourier domain.
        // xf = xf' / (kernel_fdata + epsilon)
        // Where x_f is the output_buffer and xf_prime is input buffer for backwards step.
        for (size_t i = 0; i < x_prime.size(); ++i)
        {
            // kernel_fdata + epsilon for numerical stability.
            double a = output_buffer[i][0];
            double b = output_buffer[i][1];
            double c = kernel_fdata[i][0];
            double d = kernel_fdata[i][1];
            double denom = c * c + d * d + epsilon;

            input_buffer[i][0] = (a * c + b * d) / denom;
            input_buffer[i][1] = (b * c - a * d) / denom;
        }
        
        // Perform the backward Fourier transform.
        // xf -> x
        fftw_execute(backward_plan);

        // Return x_prime as real vector
        // Remember to normalise by input_size.
        vector<double> x(x_prime.size());
        for (size_t i = 0; i < x_prime.size(); ++i)
        {
            x[i] = output_buffer[i][0]/x_prime.size();
        }
        
        return x;
    }
};

/**
 * @brief Fourier Transform linear operator.
 * 
 * Applies a Fourier transform to the input image. FFTW3 is used for the implementation.
 */
class FourierTransform : public LinearOperator<double,std::complex<double>>
{
private:
    // Image width and height.
    size_t image_width;
    size_t image_height;

    // Initialise Buffers buffer
    fftw_complex * input_buffer;
    fftw_complex * output_buffer;

    // Initialise plans 
    fftw_plan forward_plan;
    fftw_plan backward_plan;

public:
    // Constructor for FourierTransform operator.
    // Takes in image width and height.
    FourierTransform(size_t image_width, size_t image_height)
        : image_width(image_width), image_height(image_height)
        {
            // Initialise input output buffers as complex arrays.
            input_buffer = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * image_width * image_height);
            output_buffer = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * image_width * image_height);

            // Initialise forward plan.
            forward_plan = fftw_plan_dft_2d(
                image_height,
                image_width,
                input_buffer,
                output_buffer,
                FFTW_FORWARD,
                FFTW_MEASURE);

            // Initialise backward plan.
            backward_plan = fftw_plan_dft_2d(
                image_height,
                image_width,
                input_buffer,
                output_buffer,
                FFTW_BACKWARD,
                FFTW_MEASURE);
        }

    // Destructor for FourierTransform operator.
    ~FourierTransform()
    {
        // Free the memory.
        fftw_free(input_buffer);
        fftw_free(output_buffer);
        fftw_destroy_plan(forward_plan);
        fftw_destroy_plan(backward_plan);
    }

    // Forward Fourier Transform: x' = Φ(x)
    vector<std::complex<double>> operator()(const vector<double>& x) override
    {
        // Check if the input size is compatible with the image size.
        if (x.size() != image_width * image_height)
        {
            throw std::runtime_error("Input size " + std::to_string(x.size()) + " does not match image size " + std::to_string(image_width * image_height));
        }

        // Initalise the input buffer as x and zeros.
        for (size_t i = 0; i < x.size(); ++i)
        {
            input_buffer[i][0] = x[i];
            input_buffer[i][1] = 0.0;
        }
        
        // Perform the forward Fourier transform.
        // x -> x_f
        fftw_execute(forward_plan);

        // Return x_f as a complex vector.
        vector<std::complex<double>> x_f(x.size());
        for (size_t i = 0; i < x.size(); ++i)
        {
            x_f[i] = std::complex<double>(output_buffer[i][0], output_buffer[i][1]);
        }

        return x_f;
    }

    // Adjoint transform:
    vector<double> adjoint(const vector<std::complex<double>>& x_f) override
    {
        if (x_f.size() != image_width * image_height)
        {
            throw std::runtime_error("Input size " + std::to_string(x_f.size()) + " does not match image size " + std::to_string(image_width * image_height));
        }

        // Initalise the input buffer as x' and zeros.
        for (size_t i = 0; i < x_f.size(); ++i)
        {
            input_buffer[i][0] = x_f[i].real();
            input_buffer[i][1] = x_f[i].imag();
        }
        
        
        // Perform the backward Fourier transform.
        // x' -> x
        fftw_execute(backward_plan);

        // Return x as real vector
        vector<double> x(x_f.size());
        for (size_t i = 0; i < x_f.size(); ++i)
        {
            x[i] = output_buffer[i][0]/x_f.size();
        }
        
        return x;
    }

};