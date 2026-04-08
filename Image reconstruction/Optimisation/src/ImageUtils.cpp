#include "ImageUtils.h"
#include <vector>
#include <string>
#include <fstream>
#include "DataPack.h"
#include <cmath>
#include <numeric>

DataPack ImageUtils::ReadImage(const string &path)
{
    std::fstream file;
    file.open(path, std::ios::in);
    std::string buffer;
    file >> buffer;
    file >> buffer;
    size_t width = std::stoi(buffer);
    file >> buffer;
    size_t height = std::stoi(buffer);
    int colour_range;
    file >> buffer;
    colour_range = std::stoi(buffer);
    vector<double> data(width * height);
    for(size_t i = 0; i < width*height; i++)
    {
        file >> buffer;
        int x = std::stoi(buffer);
        data[i] =  static_cast<double>(x);
    }
    file.close();
        return {width, height, data};
}

void ImageUtils::WriteImage(const DataPack& image, const string &path)
{
    std::fstream file;
    file.open(path, std::ios::out);
    file << "P2\n";
    file << image.width << " " << image.height << "\n";
    file << "255\n";

    for(size_t i = 0; i < image.height; i++)
    {

        for(size_t j = 0; j < image.width; j++)
    {
        
        int x = std::round(image.data[i*image.width + j]);  // Add 0.5 to get rounding from truncation
        file << x << " ";
    }
        file << "\n";
    }
    file.close();
}

vector<double> ImageUtils::GenSincKernel(size_t w, size_t h, double scale)
{
    vector<double> kernel(w * h, 0.0);
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            double dy = fmod((i + 0.5 + 0.5 * h), h) - (0.5 * h);
            double dx = fmod((j + 0.5 + 0.5 * w), w) - (0.5 * w);
            double r = sqrt(dx*dx + dy*dy) / scale;
            double sinc_r = r == 0 ? 1 : sin(r)/r;
            kernel[i * w + j] = sinc_r*sinc_r;
        }
    }
    double Norm = std::accumulate(kernel.begin(), kernel.end(), 0.0);
    for (int i = 0; i < kernel.size(); i++)
    {
        kernel[i] /= Norm;
    }
    return kernel;
}