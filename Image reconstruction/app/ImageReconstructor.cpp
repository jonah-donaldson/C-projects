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

    // Default parameters
    string image_path = "";
    string indices_file = "";
    double alpha = 1.0;
    double beta = 1e4;
    double sigma = 1.0;
    double tol = 0.1;
    size_t kernel_size = 1;
    bool useKernel = false;
    bool useIndices = false;

    // 1. ROBUST COMMAND LINE PARSING (Assignment Compliant)
    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "-h")
        {
            std::cout << "Usage: " << name << " -f <image_path> [options]\n\n"
              << "Options:\n"
              << "  -f <path>            Path to the input measurements file.\n"
              << "  -alpha <val>         Gradient step size\n"
              << "  -beta <val>          Proximal step size\n"
              << "  -sigma <val>         Value in log-likelihood function\n"
              << "  -delta <val>         Convergence tolerance\n"
              << "  -k <kernel_size>     Gaussian kernel size (convolution).\n"
              << "  -i <indices_file>    Path to indices file (subsampling).\n"
              << "  -h                   Display this help message and exit.\n";
            return 0; // MUST exit after printing help
        }
        else if (arg == "-f" && i + 1 < argc) image_path = argv[++i];
        else if (arg == "-alpha" && i + 1 < argc) alpha = std::stod(argv[++i]);
        else if (arg == "-beta" && i + 1 < argc) beta = std::stod(argv[++i]);
        else if (arg == "-sigma" && i + 1 < argc) sigma = std::stod(argv[++i]);
        else if (arg == "-delta" && i + 1 < argc) tol = std::stod(argv[++i]);
        else if (arg == "-k" && i + 1 < argc) { useKernel = true; kernel_size = std::stoi(argv[++i]); }
        else if (arg == "-i" && i + 1 < argc) { useIndices = true; indices_file = argv[++i]; }
    }

    if (image_path.empty() || (!useKernel && !useIndices))
    {
        std::cerr << "Error: Must provide a measurements file (-f) and either a kernel (-k) or indices (-i).\n";
        return 1;
    }

    // Extract just the filename from the input path (e.g., gets "UtahTeapot_convolved.dat")
    size_t last_slash = image_path.find_last_of("/");
    std::string base_name = (last_slash == std::string::npos) ? image_path : image_path.substr(last_slash + 1);

    // Strip the .dat extension (e.g., gets "UtahTeapot_convolved")
    size_t last_dot = base_name.find_last_of(".");
    if (last_dot != std::string::npos) {
        base_name = base_name.substr(0, last_dot);
    }

    // Point to your new folder from the screenshot
    std::string output_dir = "data/app_out/";

    if (useKernel)
    {
        // Load image
        DataPack<double> image = OptimisationUtils::ReadData<double>(image_path);

        DataPack<double> input_pack = {image.width, image.height, image.data};
        ImageUtils::WriteImage(input_pack, output_dir + base_name + "_input.pgm");
        
        // Generate the kernel
        vector<double> kernel = ImageUtils::GenSincKernel(image.width, image.height, kernel_size);

        // Get dirty image
        Convolution conv(kernel, image.width, image.height, 1e-2);
        vector<double> dirty_image = conv.adjoint(image.data);
        DataPack<double> dirty_image_pack = {image.width, image.height, dirty_image};
        ImageUtils::WriteImage(dirty_image_pack, output_dir + base_name + "_dirty.pgm");

        // Define functions
        Gaussian<double> gauss(conv, image.data, sigma);
        DCTL1Norm dctl1(image.width, image.height, 0.1);

        // Wrap the C_converged function in a lambda to pass to IOA
        auto my_convergence = [&gauss, &dctl1](vector<double>& x_np1, vector<double>& x0, double tolerance) {
            return OptimisationUtils::C_converged(x_np1, x0, gauss, dctl1, tolerance);
        };

        // Apply optimisation
        vector<double> result = OptimisationUtils::IOA<double, double>(gauss, dctl1, image.data, conv, alpha, beta, 500, tol, my_convergence);
        
        // Clamp result values to [0, 255] range
        for (auto& pixel : result) {
        pixel = std::max(0.0, std::min(255.0, pixel));
        }
        
        // Write result
        DataPack<double> result_pack = {image.width, image.height, result};
        ImageUtils::WriteImage(result_pack, output_dir + base_name + "_reconstructed.pgm");
    }
    else if (useIndices)
    {
        // Load image (measurements) and indices
        DataPack<double> image = OptimisationUtils::ReadData<double>(image_path);
        DataPack<double> indices = OptimisationUtils::ReadData<double>(indices_file);
        
        // Setup SubSampler
        vector<size_t> indices_data(indices.data.size());
        for (size_t i = 0; i < indices.data.size(); i++)
        {
            indices_data[i] = static_cast<size_t>(indices.data[i]);
        }
        SubSampler<double> sub_sampler(indices_data);

        // Band-aid solution to set full size
        std::vector<double> quick_fix(image.width * image.height, 1.0);
        sub_sampler(quick_fix);

        // Get dirty image
        vector<double> dirty_image_data = sub_sampler.adjoint(image.data); 
        DataPack<double> dirty_image_pack = {image.width, image.height, dirty_image_data};
        ImageUtils::WriteImage(dirty_image_pack, output_dir + base_name + "_dirty.pgm");

        // Define functions (FIXED: Pass image.data, NOT dirty_image_pack.data)
        Gaussian<double> gauss(sub_sampler, image.data, sigma);
        DCTL1Norm dctl1(image.width, image.height, 0.1);

        // Wrap the C_converged function in a lambda to pass to IOA
        auto my_convergence = [&gauss, &dctl1](vector<double>& x_np1, vector<double>& x0, double tolerance) {
            return OptimisationUtils::C_converged(x_np1, x0, gauss, dctl1, tolerance);
        };

        // Apply optimisation (FIXED: Pass image.data, NOT dirty_image_pack.data)
        vector<double> result = OptimisationUtils::IOA<double, double>(gauss, dctl1, image.data, sub_sampler, alpha, beta, 500, tol, my_convergence);
        
        // Clamp result values to [0, 255] range
        for (auto& pixel : result) {
        pixel = std::max(0.0, std::min(255.0, pixel));
        }
        
        // Write result
        DataPack<double> result_pack = {image.width, image.height, result};
        ImageUtils::WriteImage(result_pack, output_dir + base_name + "_reconstructed.pgm");
    }

    return 0;
}