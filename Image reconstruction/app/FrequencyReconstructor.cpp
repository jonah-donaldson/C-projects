#include "ImageUtils.h"
#include "OptimisationUtils.h"
#include "LinearOperator.h"
#include <iostream>
#include <string>
#include <cmath>
#include <complex>
#include <vector>

using std::string;
using std::vector;

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
    bool useIndices = false;

    // COMMAND LINE PARSING 
    for (int i = 1; i < argc; i++)
    {
        string arg = argv[i];
        if (arg == "-h")
        {
            std::cout << "Usage: " << name << " -f <image_path> -i <indices_file> [options]\n\n"
              << "Options:\n"
              << "  -f <path>            Path to the input complex measurements file.\n"
              << "  -alpha <val>         Gradient step size\n"
              << "  -beta <val>          Proximal step size\n"
              << "  -sigma <val>         Value in log-likelihood function\n"
              << "  -delta <val>         Convergence tolerance\n"
              << "  -i <indices_file>    Path to indices file (subsampling).\n"
              << "  -h                   Display this help message and exit.\n";
            return 0; // MUST exit after printing help
        }
        else if (arg == "-f" && i + 1 < argc) image_path = argv[++i];
        else if (arg == "-alpha" && i + 1 < argc) alpha = std::stod(argv[++i]);
        else if (arg == "-beta" && i + 1 < argc) beta = std::stod(argv[++i]);
        else if (arg == "-sigma" && i + 1 < argc) sigma = std::stod(argv[++i]);
        else if (arg == "-delta" && i + 1 < argc) tol = std::stod(argv[++i]);
        else if (arg == "-i" && i + 1 < argc) { useIndices = true; indices_file = argv[++i]; }
    }

    if (image_path.empty() || !useIndices)
    {
        std::cerr << "Error: Must provide a measurements file (-f) and indices (-i).\n";
        return 1;
    }

    // Extract just the filename from the input path (e.g., gets "UtahTeapot_frequencyLossMeasurements")
    size_t last_slash = image_path.find_last_of("/");
    std::string base_name = (last_slash == std::string::npos) ? image_path : image_path.substr(last_slash + 1);

    // Strip the .dat extension 
    size_t last_dot = base_name.find_last_of(".");
    if (last_dot != std::string::npos) {
        base_name = base_name.substr(0, last_dot);
    }

    // Point to your new folder from the screenshot
    std::string output_dir = "data/app_out/";

    // Load complex image (measurements) and indices
    // Using 'auto' safely handles the complex DataPack type returned by the template
    auto image = OptimisationUtils::ReadData<std::complex<double>>(image_path);
    auto indices = OptimisationUtils::ReadData<double>(indices_file);
    
    // Setup SubSampler
    vector<size_t> indices_data(indices.data.size());
    for (size_t i = 0; i < indices.data.size(); i++)
    {
        indices_data[i] = static_cast<size_t>(indices.data[i]);
    }
    
    // Instantiate Operators
    FourierTransform ft(image.width, image.height);
    SubSampler<std::complex<double>>sub_sampler(indices_data); //

    // Band-aid solution to set full size (Updated for complex values)
    std::vector<std::complex<double>> quick_fix(image.width * image.height, std::complex<double>(1.0, 0.0));
    sub_sampler(quick_fix);

    // Composite Operator: Image (double) -> FT (complex) -> SubSample (complex)
    CompositeOperator<double, std::complex<double>, std::complex<double>> comp(ft, sub_sampler);

    // Get dirty image
    // The adjoint of the composite operator automatically handles Inverse FT and mapping frequencies back to real space
    vector<double> dirty_image_data = comp.adjoint(image.data); 

    // Clamp dirty image values to [0, 255] range
    for (auto& pixel : dirty_image_data) {
        pixel = std::max(0.0, std::min(255.0, pixel));
    }

    DataPack<double> dirty_image_pack = {image.width, image.height, dirty_image_data};
    ImageUtils::WriteImage(dirty_image_pack, output_dir + base_name + "_dirty.pgm");

    // Define functions
    // The Gaussian likelihood takes complex data now because the measurements are complex!
    Gaussian<std::complex<double>> gauss(comp, image.data, sigma);
    DCTL1Norm dctl1(image.width, image.height, 0.1);

    // Wrap the C_converged function in a lambda to pass to IOA
    auto my_convergence = [&gauss, &dctl1](vector<double>& x_np1, vector<double>& x0, double tolerance) {
        return OptimisationUtils::C_converged(x_np1, x0, gauss, dctl1, tolerance);
    };

    // Apply optimisation
    // Notice IOA is explicitly told the input is double and the output space is complex<double>
    vector<double> result = OptimisationUtils::IOA<double, std::complex<double>>(gauss, dctl1, image.data, comp, alpha, beta, 500, tol, my_convergence);
    
    // Clamp result values to [0, 255] range
    for (auto& pixel : result) {
        pixel = std::max(0.0, std::min(255.0, pixel));
    }

    // Write result
    DataPack<double> result_pack = {image.width, image.height, result};
    ImageUtils::WriteImage(result_pack, output_dir + base_name + "_reconstructed.pgm");

    return 0;
}