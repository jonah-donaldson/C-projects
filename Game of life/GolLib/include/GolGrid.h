#ifndef golGrid_h
#define golGrid_h
#include <cstddef>
#include <stdexcept>
#include <memory>
#include <random>

namespace gol 
{

// Grid class for Conway's Game of Life
// This class represents a 2D grid of cells, where each cell can be either alive or dead.
class Grid 
{
private:
    /// @cond Private member functions for Grid class
    // Number of rows and columns
    // Unique pointer to the grid cells 
    // Size of the grid (total number of cells)
    size_t rows;
    size_t cols;
    size_t size;
    std::unique_ptr<bool[]> cells; 

public: 
    /// @cond Constructors
    // Default constructor
    Grid()
    {   
        rows = 0;
        cols = 0;
        size = 0;
        cells = nullptr;
    }
    Grid(size_t rows, size_t cols);
    Grid(size_t rows, size_t cols, const bool* cells);
    Grid(size_t rows, size_t cols, size_t StartAlive, std::mt19937& rng);
    Grid(const std::string& filename);
    Grid(const Grid& other); 

    /// @cond Function to print the grid
    void print() const;

    // Functions to get cell state
    bool get(size_t row, size_t col) const
    {
        if (row >= rows || col >= cols)
        {
            throw std::out_of_range("Grid::set - Index out of range");
        }
        return cells[row * cols + col];
    }

    // Function to set cell state
    void set(size_t row, size_t col, bool value)
    {
        if (row >= rows || col >= cols)
        {
            throw std::out_of_range("Grid::set - Index out of range");
        }
        cells[row * cols + col] = value;
    }

    /// @cond  Function to that checks current grid against known patterns
    bool IsUnique(std::vector<gol::Grid> &KnownPatterns) const;

    /// @cond Functions count number of alive cells and alive neighbours
    size_t NumberAlive() const; 
    size_t NumberAliveNeighbours(size_t row, size_t col) const;

    /// @cond Function to evenly split the grid into smaller grids with a box wise scheme
    std::vector<Grid> Cleave (size_t NumRows, size_t NumCols) const;

    /// @cond Function to extract still life patterns from grid
    std::vector<Grid> ExtractSL() const;
    
    /// @cond Getters for rows, columns, and size 
    size_t GetRows() const { return rows; }
    size_t GetCols() const { return cols; }
    size_t GetSize() const { return size; }
    std::unique_ptr<bool[]> GetCells() const;
    
    /// @cond Operator overloads
    friend bool operator==(const Grid& lhs, const Grid& rhs);
    friend bool operator!=(const Grid& lhs, const Grid& rhs) {return !(lhs == rhs);}
    Grid& operator=(const Grid& other);
};    

} // namespace gol

#endif