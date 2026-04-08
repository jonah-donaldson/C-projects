#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <tuple>
#include "DataPack.h"
#include "DifferentiableFunction.h"
#include "NonDifferentiableFunction.h"
#include "LinearOperator.h"
#include "VectorUtils.h"
#include <iostream>

using std::string;
using std::vector;

namespace OptimisationUtils
{
    /**
     * @brief Utility function for splitting a string into a list of sub-strings separated by delimiter
     *
     * @param line
     * @param delim
     * @return vector<string>
     */
    vector<string> tokenise(const string &line, char delim=' ');

    /**
     * @brief Utility function for converting list of string tokens to a data type T.
     * Must be explicitly instantiated in OptimisationUtils.cpp due to differences in implementation.
     *
     * @tparam T
     * @param tokens
     * @return T
     */
    template <typename T>
    T getDataFromTokens(const vector<string> &tokens);

    /**
     * @brief Read a data file. The width and height of the related image as stored in the
     * reference parameters.
     * Requires getDataFromTokens to be implements for type T.
     *
     * @tparam T
     * @param path
     * @param width
     * @param height
     * @return vector<T>
     */
    template <typename T>
    DataPack ReadData(const string &path)
    {
        std::fstream file;
        file.open(path, std::ios::in);
        if (!file.is_open()) // Check if file is exists
        {
            throw std::runtime_error("Could not open file: " + path);
        }
        string line;
        std::getline(file, line);
        vector<string> tokens = tokenise(line);
        size_t width = std::stoi(tokens[0]);
        size_t height = std::stoi(tokens[1]);

        std::getline(file, line);
        size_t data_size = std::stoi(line);
        vector<T> data(data_size);
        
        size_t count = 0;
        while (std::getline(file, line))
        {
            vector<string> tokens = tokenise(line);
            data[count++] = getDataFromTokens<T>(tokens);
        }

        file.close();
        if (count != data_size) 
        {
        // Checks if the number of data points read is equal to the number of data points expected
        throw std::runtime_error("Data size mismatch: expected " + std::to_string(data_size) + ", got " + std::to_string(count));
        }
        return {width, height, data};
    }

    /**
     * @brief Write a data file
     *
     * @tparam Datatype T must overload the << operator for streaming to file.
     * @param data
     * @param width
     * @param height
     * @param path
     */
    template <typename T>
    void WriteData(const vector<T> &data, size_t width, size_t height, const string &path)
    {
        std::fstream file;
        file.open(path, std::ios::out);
        file << width << " " << height << "\n";
        file << data.size() << "\n";

        for (size_t i = 0; i < data.size(); i++)
        {
            file << data[i] << "\n";
        }
        file.close();
    }

    /**
     * @brief Simple convergence function that checks the relative change in two images
     *
     * The operators * and - must be implemented for the data type T
     * The result of the squared difference should be able to be added to the double type.
     *
     * @param x1 : vector of data
     * @param x2 : updated vector of data
     * @param tol : tolerance for convergence
     * @return true
     * @return false
     */
    
    template <typename T>
    bool converged(vector<T> &x2, vector<T> &x1, double tol)
    {
        double sum = 0;
        for (size_t i = 0; i < x1.size(); i++)
        {
            sum += (x2[i] - x1[i]) * (x2[i] - x1[i]);
        }
        double ms_diff = sqrt(sum / x1.size());
        return ms_diff <= tol;
    }

    /**
     * @brief Convergence function that checks the relative change in two images, using C(x) = f(x) + g(x).
     *
     * The operators * and - must be implemented for the data type T
     * The result of the squared difference should be able to be added to the double type.
     *
     * @param x1 : vector of data
     * @param x2 : updated vector of data
     * @param f : differentiable function
     * @param g : non-differentiable function
     * @param tol : tolerance for convergence
     * @return true
     * @return false
     */
    // TODO: Response md.
    template <typename T>
    bool C_converged(vector<T> &x2, vector<T> &x1, DifferentiableFunction<T> &f, NonDifferentiableFunction<T> &g, double tol)
    {
        T Cx1 = f(x1) + g(x1);
        T Cx2 = f(x2) + g(x2);
        double ms_diff = std::abs(Cx2 - Cx1)/std::abs(Cx1);
        
        return ms_diff < tol;
    }

    /**
    * @brief Iterative optimisation algorithm.
    *
    * @param f : differentiable function
    * @param g : non-differentiable function
    * @param y : vector of data
    * @param A : linear operator
    * @param alpha : step size
    * @param beta : step size
    * @param n_max : maximum number of iterations
    * @param tol : convergence tolerance
    * @return Vector<T> : Data vector at the final iteration
    */
    template <typename T>
    vector<T> IOA (DifferentiableFunction<T> &f,
            NonDifferentiableFunction<T> &g,
            const vector<T> &y, 
            LinearOperator<T,T> &A,
            double alpha, 
            double beta, 
            double n_max, 
            double tol) 
    {
        vector<T> x0 = A.adjoint(y);
        vector<T> x_np1 = x0; // Initialize x_np1 with x0
        vector<T> x_temp = x0; // Initialize x_temp with x0
        for (size_t i = 0; i < n_max; i++) 
        {
            x_temp = x0 - alpha * f.gradient(x0);
            x_np1 = x_temp - beta * g.proximal(x_temp); 

            if (C_converged<T>(x_np1, x0, f, g, tol)) // TDO: add multi convergence function option.
            {
                std::cout << "Converged at iteration: " << i << std::endl; // Final iteration
                return x_np1; // Return the value of the function at the final iteration
            }
            x0 = x_np1; // Update x0 for the next iteration
        }
        std::cout << "No convergence" << std::endl; // Print if no convergence
        return x_np1; // Return the last value if no convergence
    }
}; // namespace OptimisationUtils