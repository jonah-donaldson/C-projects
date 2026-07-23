#pragma once

#include <vector>
#include <complex>
#include "LinearOperator.h"
#include "VectorUtils.h"

using std::vector;

/**
 * @brief Base class for differentiable functions.
 */ 
template <typename T>
class DifferentiableFunction 
{
public:
    // Evaluate the function for input x.
    virtual double operator()(const vector<T> &x) = 0;

    // Compute the gradient of the function at input x.
    virtual vector<T> gradient(const vector<T> &x) = 0;

    // Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~DifferentiableFunction() {}
};

/**
 * @brief An example of a differentiable function. 
 * You should modify this to inherit from your own differentiable function class.
 */
class Quadratic : public DifferentiableFunction<double>
{
    public:
    double operator()(const vector<double> &x) override 
    { 
        return x[0]*x[0];
    }

    vector<double> gradient(const vector<double> &x) override
    {
        return {2 * x[0]};
    }
};

template <typename T>
class Gaussian : public DifferentiableFunction<double> // Lock image to double
{
    private:
        double std_dev;
        vector<T> y;
        LinearOperator<double, T> &Lin; // Takes double (image) in, outputs T (measurements)
    
    public:
        // Constructor 
        Gaussian (LinearOperator<double, T> &Lin, vector<T> &y, double std_dev) 
            : Lin(Lin), y(y), std_dev(std_dev) {}

        // Evaluate the Gaussian likelihood function (x is always double)
        double operator()(const vector<double> &x) override
        {
            vector<T> model = Lin(x);
            double norm = 0;

            // if constexpr is safe here since assignment enforces C++17
            if constexpr (std::is_same<T, std::complex<double>>::value) 
            {
                for (size_t i = 0; i < model.size(); ++i)
                {
                    auto diff = model[i] - y[i];
                    norm += std::norm(diff);
                }
            }
            else
            {
                for (size_t i = 0; i < model.size(); ++i)
                {
                    auto diff = model[i] - y[i];
                    norm += diff * diff;
                }
            }
            
            return (norm / (2 * std_dev * std_dev));
        }

        // Compute gradient (Input x is double, returned gradient is double)
        vector<double> gradient(const vector<double> &x) override
        {
            vector<T> model = Lin(x);

            if (model.size() != y.size())
            {
                throw std::invalid_argument("Size mismatch between model and measurements.");
            }

            vector<T> grad(model.size());
            for (size_t i = 0; i < grad.size(); ++i)
            {
                grad[i] = (model[i] - y[i]) / (std_dev * std_dev);
            }

            // The adjoint maps the complex gradient perfectly back to a real double gradient!
            return Lin.adjoint(grad);
        }
};