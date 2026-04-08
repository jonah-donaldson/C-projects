#include "ImageUtils.h"
#include "OptimisationUtils.h"
#include "LinearOperator.h"
#include <iostream>
#include <string>
#include <cmath>

using std::string;

int main(int argc, char *argv[])
{
    string name = std::string(argv[0]);

    // help message -h flag
    for (int i = 0; i < argc; i++)
    {
        if (string(argv[i]) == "-h")
        {
            std::cout << "Usage: " << name
              << " <image_path> <alpha> <beta> <sigma> <tol> (-k <kernel_size> | -i <indices_file_path>)\n\n"
              << "Description:\n"
              << "  ImageReconstructor reconstructs an image using an optimisation algorithm.\n\n"
              << "Positional arguments:\n"
              << "  <image_path>         Path to the input image file.\n"
              << "  <alpha>              Gradient step size (Recommended: 1.0)\n"
              << "  <beta>               Proximal step size (Recommended: 1e4)\n"
              << "  <sigma>              Value in log-likelihood function (Recommended: 1.0)\n"
              << "  <tol>                Convergence tolerance (Recommended: 0.1)\n\n"
              << "Optional arguments (choose ONE):\n"
              << "  -k <kernel_size>     Gaussian kernel size in pixels (for convolution problems).\n"
              << "  -i <indices_file>    Path to the indices file (for subsampling problems).\n\n"
              << "Other options:\n"
              << "  -h                   Display this help message and exit.\n\n"
              << "Additional Defaults:\n"
              << "  Maximum optimization steps: 500\n";
        } 
    }

    // Check that the correct number of arguments have been passed
    if (argc != 8)
    {
        std::cerr << "Usage: " << argv[0] 
             << "The number of arguments passed is incorrect. Please see the help message for more information."
             << std::endl;
        return 1;
    }

    // Parse the positional arguments
    string image_path = argv[1];

    // Parse the remaining arguments
    double alpha = std::stod(argv[2]);
    double beta  = std::stod(argv[3]);
    double sigma = std::stod(argv[4]);
    double tol   = std::stod(argv[5]);

    // Parse the optional arguments
    bool useKernel = false;
    bool useIndices = false;
    if (std::string(argv[6])== "-k")
    {
        useKernel = true;
        size_t kernel_size = std::stoi(argv[7]);
    }
    else if (std::string(argv[6]) == "-i")
    {
        useIndices = true;
    }
    else
    {
        std::cerr << "Usage: " << argv[0] 
             << "The optional arguments passed are incorrect. Please see the help -h message for more information."
             << std::endl;
        return 1;
    }

    // Proceed based on the option selected
    if (useKernel)
    {
        // Load image
        DataPack image = OptimisationUtils::ReadData<double>(image_path);
        
        // Generate the kernel
        vector<double> kernel = ImageUtils::GenSincKernel(image.width, image.height, 1);

        // Get dirty image
        Convolution conv(kernel, image.width, image.height, 1e-2);

        vector<double> dirty_image = conv.adjoint(image.data);
        DataPack dirty_image_pack = {image.width, image.height, dirty_image};
        
        // Remove any extension from image_path before appending file name extensions
        size_t last_dot = image_path.find_last_of('.');
        string base_image_path = (last_dot != string::npos) ? image_path.substr(0, last_dot) : image_path;

        ImageUtils::WriteImage(dirty_image_pack, base_image_path + "_dirty.pgm");

        // Define the Gaussian likelihood and DCT l1-norm prior
        Gaussian<double> gauss(conv, image.data, sigma);
        DCTL1Norm dctl1(image.width, image.height, 0.1);

        // Apply the optimisation
        vector<double> result = OptimisationUtils::IOA(gauss, dctl1, image.data, conv, alpha, beta, 100, tol);

        // Write the result to a file for visual inspection
        DataPack result_pack = {image.width, image.height, result};
        ImageUtils::WriteImage(result_pack, base_image_path + "_reconstructed.pgm");
        
    }
    
    else if (useIndices)
    {
        // Parse the positional arguments
        string indices_file = argv[7];

        // Load image
        DataPack image = OptimisationUtils::ReadData<double>(image_path);

        // Load indices
        DataPack indices = OptimisationUtils::ReadData<double>(indices_file);
        
        // Define the sub-sampler operator
        vector<size_t> indices_data(indices.data.size());
        for (size_t i = 0; i < indices.data.size(); i++)
        {
            indices_data[i] = static_cast<size_t>(indices.data[i]);
        }
        SubSampler sub_sampler(indices_data);

        // Band-aid solution to set full size
        // Since object full size is set when forward operator is called.
        std::vector<double> quick_fix(image.width * image.height, 1.0);
        sub_sampler(quick_fix);

        // Get dirty image (subsampled)
        vector<double> dirty_image_data = sub_sampler.adjoint(image.data); // Seg fault occurs here, and I'm fully out of time.
        DataPack dirty_image_pack = {image.width, image.height, dirty_image_data};

        // Remove any extension from image_path before appending file name extensions
        size_t last_dot = image_path.find_last_of('.');
        string base_image_path = (last_dot != string::npos) ? image_path.substr(0, last_dot) : image_path;

        ImageUtils::WriteImage(dirty_image_pack, base_image_path + "_dirty.pgm");

        // Define the Gaussian likelihood and DCT l1-norm prior
        Gaussian<double> gauss(sub_sampler, dirty_image_pack.data, sigma);
        DCTL1Norm dctl1(image.width, image.height, 0.1);

        // Apply the optimisation
        vector<double> result = OptimisationUtils::IOA(gauss, dctl1, dirty_image_pack.data, sub_sampler, alpha, beta, 500, tol);

        // Write the result to a file for visual inspection
        DataPack result_pack = {image.width, image.height, result};
        ImageUtils::WriteImage(result_pack, base_image_path + "_reconstructed.pgm");
    }

} 