#pragma once

#include <vector>

using std::vector;

/**
 * @brief This is a utility struct for bundling image data together.
 * This is mostly useful for packing multiple data into the
 * return of the ReadImage function.
 */
template <typename T>
struct DataPack
{
    size_t width;
    size_t height;
    vector<T> data;
};