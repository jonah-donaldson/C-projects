#pragma once

#include <vector>
#include <string>
#include <DataPack.h>

using std::string;
using std::vector;


namespace ImageUtils
{
    /**
     * @brief Read an image file from .pgm format.
     * Reference parameters width and height are overwritten with information from the file.
     * This should be replace with a single output of the Image class once written.
     * 
     * @param path
     * @param width
     * @param height
     * @return vector<double>
     */
    DataPack ReadImage(const string &path);

    /**
     * @brief Write an image to file in .pgm format
     * You can pass separate width, height, and image data variables as an DataPack
     * struct using the syntax {width, height, data}.  
     * @param image
     * @param path
     * @return * void
     */
    void WriteImage(const DataPack& image, const string &path);

    /**
     * @brief Generate a Gaussian kernel of a given size
     *
     */
    vector<double> GenSincKernel(size_t w, size_t h, double scale);
}; // namespace ImageUtils