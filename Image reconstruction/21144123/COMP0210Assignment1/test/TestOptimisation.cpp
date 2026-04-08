#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>  
#include <catch2/matchers/catch_matchers_all.hpp>
#include "OptimisationUtils.h"
#include "ImageUtils.h"
#include "DifferentiableFunction.h"
#include "LinearOperator.h"
#include "NonDifferentiableFunction.h"
#include <string>
#include "DataPack.h"
#include "fftw3.h"

using namespace Catch::Matchers;
using std::vector;
using std::complex;



/****************************************************************************/
/* Section 1.2 - IO Testing
/****************************************************************************/

/** 
* @brief Test that the image reading and writing functions are working correctly 
*/

TEST_CASE("Test Data reading", "IO Tests")
{    
    //Load data from a file
    std::string path = "test/data/measurements/TestSphere_proximal.dat";
    DataPack data;
    REQUIRE_NOTHROW(data = OptimisationUtils::ReadData<double>(path));

    // Check that the data has been extracted correctly
    CHECK(data.width == 320);
    CHECK(data.height == 240);
    CHECK(data.data.size() == 76800);
    CHECK(data.data[0] == -0.000235895);
    CHECK(data.data[1] == 8.07497e-05);
    CHECK(data.data[2] == -0.000146522);
}
/****************************************************************************/
/* Section 2.3 - Optimisation Test
/****************************************************************************/

/**
 * @brief Fill out this test function by filling in the 
 */

TEST_CASE("Test optimisation algorithm for simple quadratic function", "Algorithm Tests")
{
    // TODO: Make sure Quadratic is a differentiable function type
    Quadratic f_quadratic;

    // TODO: Make sure Empty is a non-differentiable function type
    Empty g_empty;

    // TODO: Make sure Identity is a linear operator type
    Identity Id;

    // Quadratic works with vectors of size 1 to represent a single value
    vector<double> y = {1.0}; // because Phi = Id the value of y is the start value for x
    
    // Call your optimisation function with the default convergence function
    vector<double> result = OptimisationUtils::IOA(f_quadratic, g_empty, y, Id, 0.1, 0.1, 100, 0.1);

    // Check that your result is close to 0 
    REQUIRE(result[0] == Catch::Approx({0}).margin(1e-6));
}

/****************************************************************************/
/* Section 3.1 - Linear Operator Testing
/****************************************************************************/

// Write a test to check your sub-sampler operator
TEST_CASE("Test sub-sampler", "Linear Operator Tests")
{
    // Define a simple vector of values
    vector<double> x = {1.0, 2.0, 3.0, 4.0, 5.0};

    // Define the indices to sample
    vector<size_t> indices = {0, 2, 4};

    // Define the sub-sampler operator
    SubSampler sub_sampler(indices);

    // Apply the sub-sampler operator to the vector
    vector<double> y = sub_sampler(x);

    // Check that the output is correct
    vector<double> y_expected = {1.0, 3.0, 5.0};
    REQUIRE(y == y_expected);

    // Apply the adjoint operator to the output
    vector<double> x_prime = sub_sampler.adjoint(y);

    // Check that the reconstructed vector is correct
    vector<double> x_prime_expected = {1.0, 0.0, 3.0, 0.0, 5.0};
    REQUIRE(x_prime_expected == x_prime);
}

//Complete this convolution test function using your class
TEST_CASE("Test Convolution", "Linear Operator Tests")
{
    //Test Convolution operator forward operation.
    // Load test image and kernel data
    string image_path = "test/data/images/TestSphere.pgm";
    DataPack image = ImageUtils::ReadImage(image_path);
    vector<double> kernel_data = ImageUtils::GenSincKernel(image.width, image.height, 1.0);

    // Load expectation images
    string blurred_image_path = "test/data/images/TestSphere_convolved.pgm";
    DataPack blurred_image_expected = ImageUtils::ReadImage(blurred_image_path);

    string deblurred_image_path = "test/data/images/TestSphere_deconvolved.pgm";
    DataPack deblurred_image_expected = ImageUtils::ReadImage(deblurred_image_path);

    // Write test image to file for visual inspection
    ImageUtils::WriteImage(image,"test/data/testing_images/TestSphere.pgm");

    // Applyforward Convolution operator and adjoint
    Convolution conv(kernel_data, image.width, image.height, 1e-2);
    
    vector <double> blurred_image = conv(image.data);
    vector <double> deblurred_image = conv.adjoint(blurred_image);

    // Check that the output is close to the expected blurred image
    for (size_t i = 0; i < image.data.size(); i++)
    {
        CAPTURE(i);
        //CHECK(std::round(blurred_image[i]) == Catch::Approx(blurred_image_expected.data[i]).margin(10));
    }

    // Write image to file for visual inspection
    DataPack blurred_image_pack = {image.width, image.height, blurred_image};
    ImageUtils::WriteImage(blurred_image_pack,"test/data/testing_images/TestSphere_blurred.pgm");

    // Check that the output is close to the expected deblurred image
    for (size_t i = 0; i < image.data.size(); i++)
    {
        CAPTURE(i);
        //CHECK(std::round(deblurred_image[i]) == Catch::Approx(deblurred_image_expected.data[i]).margin(10));
    }

    // Write image to file for visual inspection
    DataPack deblurred_image_pack = {image.width, image.height, deblurred_image};
    ImageUtils::WriteImage(deblurred_image_pack,"test/data/testing_images/TestSphere_deblurred.pgm");
}

/****************************************************************************/
/* Section 3.2 - Gaussian Function Testing
/****************************************************************************/

// TODO: Write a test to check that your Gaussian function is working correctly
TEST_CASE("Test Gaussian Log-Likelihood", "Differentiable Function Tests")
{
    // TODO: Use small vectors of simple values for x and y so that you can calculate the expected result analytically
    vector<double> x = {2.0, 4.0, 6.0, 8.0, 10.0};
    vector<double> y = {1.0, 2.0, 3.0, 4.0, 5.0};
    Identity Id;
    double std_dev = 2.0;

    // Check the result of a Gaussian function and its gradient with the Identity operator
    Gaussian<double> gauss(Id, y, std_dev);
    double result = gauss(x);
    vector<double> gradient = gauss.gradient(x);

    // Expected = 1/8 * (1 + 2 + 9 + 16 + 25) = 55/8
    // Gradient = 1/4 * (1, 2, 3, 4, 5)
    REQUIRE(result ==  55.0/8.0);
    REQUIRE(gradient == vector<double>({0.25, 0.5, 0.75, 1.0, 1.25}));

    // Check the result of the Gaussian function and its gradient with a sub-sampling operator
    SubSampler sub_sampler({0, 2, 4});
    y = {1.0, 3.0, 5.0}; // New measurement set
    
    Gaussian<double> sub_gauss(sub_sampler, y, std_dev);
    result = sub_gauss(x);
    gradient = sub_gauss.gradient(x);

    // Expected = 1/8 * (1 + 9 + 25) = 35/8
    // Gradient = 1/4 * (1, 0, 3, 0, 5)
    REQUIRE(result == Catch::Approx(35.0/8.0).margin(1e-6));
    REQUIRE(gradient == vector<double>({0.25, 0.0, 0.75, 0.0, 1.25}));
}

/****************************************************************************/
/* Section 3.3 - DCT L1-Norm Testing
/****************************************************************************/

// TODO: Write a test to check that your DCT L1-Norm function is working correctly
TEST_CASE("Test DCT L1-Norm", "Non-Differentiable Function Tests")
{
    string image_path = "test/data/images/TestSphere.pgm";
    DataPack image = ImageUtils::ReadImage(image_path);

    // Define DCT l1-norm prior and apply it to the image to get l1_norm
    // Use default threshold tau = 0.1
    DCTL1Norm dctl1(image.width, image.height, 0.1);
    double l1_norm = dctl1(image.data);
    REQUIRE_THAT(l1_norm, WithinRel(1.93e8, 0.05));

    // Calculate the proximal and check that it is close to the expected proximal
    DataPack expected_prox = OptimisationUtils::ReadData<double>("test/data/measurements/TestSphere_proximal.dat");
    
    vector<double> proximal = dctl1.proximal(image.data);
    for (size_t i = 0; i < image.data.size(); i++)
    {
        CAPTURE(i);
        CHECK(proximal[i] == Catch::Approx(expected_prox.data[i]).margin(1e-6));
    }
}

/****************************************************************************/
/* Section 3.5 - Integration Test
/****************************************************************************/

TEST_CASE("Recover From Synthetic Data", "Integration Test")
{
// Write a test that checks a small optimisation problem using the operators and functions
// that you have developed. 
    double sigma = 0.1;
    double alpha = 0.01;
    double beta = 1e4;
    double tol = 1e-2;
    double tau = 0.1;

    string image_path = "test/data/images/TestSphere.pgm";
    DataPack image = ImageUtils::ReadImage(image_path);

    // Define your linear operator to drop 20% of the pixels in the image
    vector<size_t> indices;
    for (size_t i = 0; i < image.width * image.height; i++)
    {
        if (rand() % 5 != 0)
        {
            indices.push_back(i);
        }
    }
    SubSampler sub_sampler(indices);   

    vector<double> y = sub_sampler(image.data);

    // Apply your optimisation (you may use the parameters above if you wish)
    // with the Gaussian likelihood and DCT l1-norm prior
    Gaussian<double> gauss(sub_sampler, y, sigma);
    DCTL1Norm dctl1(image.width, image.height, tau);

    vector<double> result = OptimisationUtils::IOA(gauss, dctl1, y, sub_sampler, alpha, beta, 100, tol);

    // Write the result to a file for visual inspection
    DataPack result_pack = {image.width, image.height, result};
    ImageUtils::WriteImage(result_pack, "test/data/testing_images/TestSphere_Reconstructed.pgm");
   
    // Write the dirty image to a file for visual inspection
    DataPack subsampled_image_pack = {image.width, image.height, sub_sampler.adjoint(y)};
    ImageUtils::WriteImage(subsampled_image_pack, "test/data/testing_images/TestSphere_sub_sampled.pgm");

}

/****************************************************************************/
/* Section 5.1 - Code testing
/****************************************************************************/
TEST_CASE("App code", "Functionality Check")
{
    // Load and write the image
    string image_path = "data/measurements/UtahTeapot_convolved.dat";
    DataPack image = OptimisationUtils::ReadData<double>(image_path);

    DataPack image_pack = {image.width, image.height, image.data};
    ImageUtils::WriteImage(image_pack, "images/UtahTeapot_blurred.pgm");

    // Generate the kernel
    vector<double> kernel = ImageUtils::GenSincKernel(image.width, image.height, 1);

    // Get dirty image
    Convolution conv(kernel, image.width, image.height, 1e-2);

    vector<double> dirty_image = conv.adjoint(image.data);
    DataPack dirty_image_pack = {image.width, image.height, dirty_image};
    ImageUtils::WriteImage(dirty_image_pack, "images/UtahTeapot_blurred_dirty.pgm");

    // Define the Gaussian likelihood and DCT l1-norm prior
    double sigma = 1.0;
    double alpha = 1.0;
    double beta = 1e4;
    double tol = 1e-2;
    double tau = 0.1;

    Gaussian<double> gauss(conv, image.data, sigma);
    DCTL1Norm dctl1(image.width, image.height, tau);

    // Apply the optimisation
    vector<double> result = OptimisationUtils::IOA(gauss, dctl1, image.data, conv, alpha, beta, 500, tol);

    // Write the result to a file for visual inspection
    DataPack result_pack = {image.width, image.height, result};
    ImageUtils::WriteImage(result_pack, "images/UtahTeapot_Convolution_Optimised.pgm");
}

/****************************************************************************/
/* Section 6.1 - FT Operator Testing
/****************************************************************************/
TEST_CASE("Fourier Transform Operator", "Linear Operator Tests")
{
    // Load a test set
    DataPack x = ImageUtils::ReadImage("test/data/images/TestSphere.pgm");

    // Define the Fourier Transform operator
    FourierTransform ft(x.width, x.height);

    // Apply the Fourier Transform operator to the vector
    vector<complex<double>> y = ft(x.data);

    // Apply the adjoint operator to the output
    vector<double> x_prime = ft.adjoint(y);

    // Check that the reconstructed vector is correct
    for (size_t i = 0; i < x.data.size(); i++)
    {
        CAPTURE(i);
        CHECK(x_prime[i] == Catch::Approx(x.data[i]).margin(1e-3));
    }
}

/****************************************************************************/
/* Section 6.2 - Operator Composition Testing
/****************************************************************************/
// Test for the composition of two identity operators

