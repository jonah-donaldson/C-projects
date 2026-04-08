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
class Gaussian : public DifferentiableFunction<T>
{
    private:
        // Standard deviation of the Gaussian.
        double std_dev;
        vector<T> y;
        LinearOperator<T,T> &Lin;
    
    public:
        // Constructor for Gaussian likelihood function.
        Gaussian (LinearOperator<T,T> &Lin, vector<T> &y, double std_dev) : Lin(Lin), y(y), std_dev(std_dev) {}

                // Evaluate the Gaussian likelihood function.
                double operator()(const vector<T> &x) override
                {
                    // Compute the difference between the observed data and the model.
                    vector<T> diff = Lin(x) - y;
        
                    // Compute the squared L2 norm of the difference.
                    double norm = 0;
        
                    if (std::is_same<T, std::complex<double>>::value) 
                    {
                        for (size_t i = 0; i < diff.size(); ++i)
                        {
                            norm += std::norm(diff[i]);
                        }
                    }
                    
                    else
                    {
                        for (size_t i = 0; i < diff.size(); ++i)
                        {
                            norm += diff[i] * diff[i];
                        }
                    }
                
                 // Return the negative log-likelihood.
                 return (norm / (2 * std_dev * std_dev));
                }

        // Compute the gradient of the Gaussian likelihood function.
        vector<T> gradient(const vector<T> &x) override
        {
            // Compute the difference between the observed data and the model.
            vector<T> diff = Lin(x) - y;

            // Compute the gradient of the negative log-likelihood.
            if (Lin(x).size() != y.size())
            {
                throw std::invalid_argument("The size of Lin(x) "+ std::to_string(Lin(x).size()) +" must match the size of y "+ std::to_string(y.size()) +".");
            }

            vector<T> grad(Lin(x).size());
            for (size_t i = 0; i < grad.size(); ++i)
            {
                grad[i] = diff[i] / (std_dev * std_dev);
            }

            return Lin.adjoint(grad);
        }
};
