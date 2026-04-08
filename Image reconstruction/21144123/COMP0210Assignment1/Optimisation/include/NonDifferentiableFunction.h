#pragma once

#include <vector>
#include <fftw3.h>

using std::vector;

/**
 * @brief Base class for non differentiable functions.
 */
template <typename T>
class NonDifferentiableFunction 
{
public:
    // Evaluate the function for input x.
    virtual double operator()(const vector<T>& x) = 0;

    // Compute proximal operator of the function at input x.
    virtual vector<T> proximal(const vector<T>& x) = 0;

    // Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~NonDifferentiableFunction() {}
};

/**
 * @brief Trivial non-differentiable function class.
 * Provides no meaningful functionality.
 * Should be made to inherit from your own non differentiable function base class.
 */
class Empty : public NonDifferentiableFunction<double>
{
public:
    double operator()(const vector<double> &x) override
    {
        return 0;
    }

    vector<double> proximal(const vector<double> &x) override
    {
         return vector<double>(x.size(), 0);
    }
};

class DCTL1Norm : public NonDifferentiableFunction<double>
{
    private:
    // Image dimensions
    size_t w;
    size_t h;

    // Threshold for the proximal operator
    double tau;

    // Buffers for the DCT
    double *input_buffer;
    double *output_buffer;

    // Plans for the DCT
    fftw_plan forward_plan;
    fftw_plan backwards_plan;

    public:
    DCTL1Norm(size_t w, size_t h, double tau = 0.1) : w(w), h(h), tau(tau) 
    {
        // Initialise buffers for the DCT
        input_buffer = (double *) fftw_malloc(sizeof(double) * h * w);
        output_buffer = (double *) fftw_malloc(sizeof(double) * h * w);

        // Create a plan for the forward DCT
        forward_plan = fftw_plan_r2r_2d(
            h,
            w,
            input_buffer,
            output_buffer,
            FFTW_REDFT10,
            FFTW_REDFT10,
            FFTW_MEASURE);

        // Create a plan for the backwards DCT
        backwards_plan = fftw_plan_r2r_2d(
            h,
            w,
            input_buffer,
            output_buffer,
            FFTW_REDFT01,
            FFTW_REDFT01,
            FFTW_MEASURE);
    }

    ~DCTL1Norm()
    {
        // Free the input and output buffers
        fftw_free(input_buffer);
        fftw_free(output_buffer);

        // Destroy the plan
        fftw_destroy_plan(forward_plan);
        fftw_destroy_plan(backwards_plan);
    }

    double operator()(const vector<double> &x) override
    {
        // Check that the input vector has dimensions h x w
        if (x.size() != h * w)
        {
            throw std::invalid_argument("Input vector has incorrect size. Expected " + std::to_string(h * w) + ", got " + std::to_string(x.size()) + ".");
        }

        // Copy the input vector into the input buffer
        for (size_t i = 0; i < h * w; i++)
        {
            input_buffer[i] = x[i];
        }

        // Apply forward DCT
        // x -> xd
        fftw_execute(forward_plan);

        // Calculate the L1 norm
        double l1_norm = 0;
        for (size_t i = 0; i < h * w; i++)
        {
            l1_norm += std::abs(output_buffer[i]);
        }

        return l1_norm;
    }

    vector<double> proximal(const vector<double> &x) override
    {
        // Check that the input vector has dimensions h x w
        if (x.size() != h * w)
        {
            throw std::invalid_argument("Input vector has incorrect size. Expected " + std::to_string(h * w) + ", got " + std::to_string(x.size()) + ".");
        }
        
        // Copy the input vector into the input buffer
        for (size_t i = 0; i < h * w; i++)
        {
            input_buffer[i] = x[i];
        }

        // Apply forward DCT
        // xd -> DCT(x)
        fftw_execute(forward_plan);

        // Apply the soft thresholding
        // xd -> xd_st
        for (int i = 0; i < w * h; i++) 
        {
            double val = output_buffer[i];
            double abs_val = std::abs(val);
            if (std::abs(val) < tau) 
            {
                input_buffer[i] = 0.0;
            } 
            else 
            {
                input_buffer[i] = val * (1.0 - tau/std::abs(val));
            }
        }
            
        // Apply the inverse DCT
        // DCT(xd_st) -> x_st
        fftw_execute(backwards_plan);

        // Copy difference between x_st and x the output buffer into the output vector
        vector<double> diff(x.size());
        for (size_t i = 0; i < h * w; i++)
        {
            diff[i] = x[i] - (output_buffer[i]/(4*h*w));
        }

        return diff;
    }
};