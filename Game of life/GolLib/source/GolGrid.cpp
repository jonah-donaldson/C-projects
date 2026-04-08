#include "GolGrid.h"
#include <iostream> // For std::cout
#include <fstream> // For std::ifstream
#include <sstream> // For std::istringstream
#include <cstring> // For std::memcmp & std::memcpy
#include <algorithm> // For std::count
#include <stdexcept> // For std::invalid_argument
#include <vector> // For std::vector
#include <random> // For std::mt19937
#include <utility> // For std::pair
#include <memory> // For std::unique_ptr
#include <set> // For std::set (to track visited cells)
#include <queue> // For std::queue (for BFS)

namespace gol 
{

/////////////////////////////////////////////////////////////////////////

// Constructors

/////////////////////////////////////////////////////////////////////////

//%% Constructor with rows and columns
/**
 * @brief Constructs a Grid with the specified number of rows and columns.
 *
 * All cells are initially set to dead.
 *
 * @param rows Number of rows in the grid.
 * @param cols Number of columns in the grid.
 */
Grid::Grid(size_t rows, size_t cols) : rows(rows), cols(cols) 
{
    if (rows == 0 || cols == 0) 
    {
        throw std::invalid_argument("Grid::Grid - rows and cols must be greater than 0");
    }
    size = rows * cols;
    cells = std::make_unique<bool[]>(size);
    std::fill(cells.get(), cells.get() + size, false);
}

//%% Constructor with rows, columns, and cell data
/**
 * @brief Constructs a Grid with the specified number of rows, columns, and initial cell data.
 *
 * The cells are initialised to the specified values.
 * Caution: Make sure cell pointer has same number of elements as rows * cols.
 *
 * @param rows Number of rows in the grid.
 * @param cols Number of columns in the grid.
 * @param cells Pointer to an array of cell values (true for alive, false for dead).
 */
Grid::Grid(size_t rows, size_t cols, const bool* cells) : rows(rows), cols(cols)
{
    if (!cells) 
    {
        throw std::invalid_argument("Grid::Grid - cells pointer cannot be null");
    }
    if(rows == 0 || cols == 0) 
    {
        throw std::invalid_argument("Grid::Grid - rows and cols must be greater than 0");
    }
    size = rows * cols;
    this->cells = std::make_unique<bool[]>(size);
    std::memcpy(this->cells.get(), cells, size * sizeof(bool)); // Copy data into the new grid
}

//%% Constructor with random alive cells
/**
 * @brief Constructs a Grid with a specific number of randomly alive cells.
 *
 * Randomly selects cells until the number of alive cells equals StartAlive.
 *
 * @param rows Number of rows in the grid.
 * @param cols Number of columns in the grid.
 * @param StartAlive The desired number of initially alive cells.
 * @param rng A random number generator.
 */
Grid::Grid(size_t rows, size_t cols, size_t StartAlive, std::mt19937& rng)
    : rows(rows), cols(cols) 
{
    if (rows == 0 || cols == 0) 
    {
        throw std::invalid_argument("Grid::Grid - rows and cols must be greater than 0");
    }
    size = rows * cols;
    if (StartAlive > size) 
    {
        throw std::invalid_argument("Grid::Grid - StartAlive cannot exceed grid size");
    }
    cells = std::make_unique<bool[]>(size);
    std::fill(cells.get(), cells.get() + size, false); // Initialise all cells to dead

    for (size_t i = 0; i < StartAlive; ++i) 
    {
        size_t index;
        do {index = rng() % size;} while (cells[index]);
        cells[index] = true;
    }
}

//%% Constructor from file
/**
 * @brief Constructs a Grid from a file.
 *
 * The file should contain rows of cell values, where "o" represents an alive cell and "-" represents a dead cell.
 * Throws std::runtime_error if the file is not found, is empty, or if the grid format is inconsistent.
 *
 * @param filename Name of the file to load the grid from.
 */
Grid::Grid(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        throw std::runtime_error("Grid::Grid - Unable to open file: " + filename);
    }
    
    std::vector<std::vector<bool>> GridData;
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty()) continue; // Skip empty lines
        std::istringstream iss(line);
        std::vector<bool> row;
        std::string cell;
        while (iss >> cell)
        {
            if (cell == "o")
            {
                row.push_back(true);
            }
            else if (cell == "-")
            {
                row.push_back(false);
            }
            else
            {
                throw std::runtime_error("Grid::Grid - Invalid cell value in file: " + cell);
            }
        }
        GridData.push_back(row);
    }

    if (GridData.empty())
    {
        throw std::runtime_error("Grid::Grid - File is empty: " + filename);
    }

    // Set grid dimensions based on the file's content
    rows = GridData.size();
    cols = GridData[0].size();
    for (const auto& r : GridData)
    {
        if (r.size() != cols)
        {
            throw std::runtime_error("Grid::Grid - Inconsistent row lengths in file: " + filename);
        }
    }
    
    size = rows * cols;
    cells = std::make_unique<bool[]>(size);

    // Populate the grid cells
    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j)
        {
            cells[i * cols + j] = GridData[i][j];
        }
    }
}

//%% Copy constructor
/**
 * @brief Copy constructor for the Grid class.
 *
 * @param other The grid to copy from.
 */
Grid::Grid(const Grid& other) : rows(other.rows), cols(other.cols), size(other.size)
{
    cells = std::make_unique<bool[]>(size);
    std::memcpy(cells.get(), other.cells.get(), size * sizeof(bool)); // Copy data into the new grid
}

/////////////////////////////////////////////////////////////////////////

// Functions

/////////////////////////////////////////////////////////////////////////

//%% Print function
/**
 * @brief Prints the grid to the console.
 *
 * Alive cells are printed as 'o' and dead cells as '-'.
 */
void Grid::print() const 
{
    for (size_t i = 0; i < rows; ++i) 
    {
        for (size_t j = 0; j < cols; ++j) 
        {
            std::cout << (cells[i * cols + j] ? 'o' : '-') << " ";
        }
        std::cout << "\n";
    }
    std::cout.flush();
}

//%% IsUnique  
/**
 * @brief Checks if the current grid state is unique compared to previously seen patterns.
 * This is done by comparing the current grid with a vector of known patterns.
 *
 * @param KnownPatterns A vector of previously seen Grids.
 * @return true if the current grid state is unique, false otherwise.
 */
bool Grid::IsUnique(std::vector<gol::Grid> &KnownPatterns) const
{
    bool IsUnique = true;
    if (KnownPatterns.empty())
    {
        return IsUnique; // No known patterns, so current grid is unique
    } 
    
    for (const auto& KnownSL : KnownPatterns) 
    {
        if (*this == KnownSL) 
        {
            IsUnique = false;
            break;
        }
    }
    return IsUnique;    
}

//%% Count alive cells
/**
 * @brief Counts the number of alive cells in the grid.
 *
 * @return The number of alive cells.
 */
size_t Grid::NumberAlive() const 
{
    return std::count(cells.get(), cells.get() + size, true);
}

//%% Count alive neighbours
/**
 * @brief Counts the number of alive neighbouring cells.
 *
 * Neighbours include all adjacent cells (including diagonals) but do not count the cell itself.
 *
 * @param row The row index of the cell.
 * @param col The column index of the cell.
 * @return The number of alive neighbouring cells.
 * @throws std::out_of_range if row or col are out of bounds.
 */
size_t Grid::NumberAliveNeighbours(size_t row, size_t col) const 
{
    if (row >= rows || col >= cols)
    {
        throw std::out_of_range("Grid::NumberAliveNeighbours - Index out of range");
    }
    size_t AliveCount = 0;

    for (int i = -1; i <= 1; ++i) 
    {
        for (int j = -1; j <= 1; ++j) 
        {
            if (i == 0 && j == 0) continue; // Skip the cell itself
            size_t NeighbourRow = row + i;
            size_t NeighbourCol = col + j;
            if (NeighbourRow >= 0 && NeighbourRow < rows && NeighbourCol >= 0 && NeighbourCol < cols &&
                cells[NeighbourRow * cols + NeighbourCol]) 
            {
                ++AliveCount;
            }
        }
    }
    return AliveCount;
}

//%% Cleave function
/**
 * @brief Splits the grid into smaller grids.
 *
 * The grid is divided into NumGrid sub-grids, each with (roughly) equal dimesnsions. 
 *
 * @param Number of sub-grids in the row direction.
 * @param Number of sub-grids in the column direction.
 * @return A vector of smaller Grid objects.
 */
std::vector<Grid> Grid::Cleave(size_t RowGrids, size_t ColGrids) const // Take NumRows and NumCols as input
{
    size_t NumGrids = RowGrids * ColGrids;

    // Check if the number of grids is valid
    if (RowGrids == 0 || ColGrids == 0) // Check individual dimensions
    {
        throw std::invalid_argument("Grid::Cleave - Number of rows and columns for sub-grids must be greater than 0");
    }
    if (NumGrids == 1)
    {
        return {*this }; // Return the original grid if only one sub-grid is requested
    }
    // Check if requested subgrid dimensions exceed original grid dimensions
    if (RowGrids > rows || ColGrids > cols)
    {
         throw std::invalid_argument("Grid::Cleave - Number of sub-grids in a dimension cannot exceed the grid's dimension size");
    }

    std::vector<Grid> SubGrids;
    SubGrids.reserve(NumGrids); // Reserve space for the sub-grids

    // Calculate the base dimensions of each sub-grid
    size_t BaseRows = rows / RowGrids;
    size_t BaseCols = cols / ColGrids;
    size_t ExtraRows = rows % RowGrids;
    size_t ExtraCols = cols % ColGrids;

    // Loop through the grid and create sub-grids
    for (size_t i = 0; i < RowGrids; ++i) 
    {
        for (size_t j = 0; j < ColGrids; ++j) 
        {
            size_t StartRow = i * BaseRows + std::min(i, ExtraRows);
            size_t StartCol = j * BaseCols + std::min(j, ExtraCols);
            size_t SubGridRows = BaseRows + (i < ExtraRows ? 1 : 0);
            size_t SubGridCols = BaseCols + (j < ExtraCols ? 1 : 0);

            // Create a new sub-grid
            Grid SubGrid(SubGridRows, SubGridCols);
            for (size_t r = 0; r < SubGridRows; ++r) 
            {
                for (size_t c = 0; c < SubGridCols; ++c) 
                {
                    SubGrid.set(r, c, cells[(StartRow + r) * cols + (StartCol + c)]);
                }
            }
            SubGrids.emplace_back(std::move(SubGrid)); // Add the sub-grid to the vector
        }
    }

    return SubGrids; // Return the vector of sub-grids
}

//%% Extract Still life patterns helper function
/**
 * @brief Helper function to extract still life patterns from the grid.
 *
 * Uses property that still life patterns are formed of continous chains of alive cells.
 * Looks for chains of alive cells and retruns them as a vector of grids.
 *
 * @return A vector of Grid objects representing the still life patterns.
 */
std::vector<Grid> Grid::ExtractSL() const
{
    std::vector<Grid> StillLifeGrids;
    std::set<std::pair<size_t, size_t>> VisitedCells; // Keep track of visited alive cells

    for (size_t r = 0; r < rows; ++r)
    {
        for (size_t c = 0; c < cols; ++c)
        {
            // Check if the cell is alive and not yet visited
            if (cells[r * cols + c] && VisitedCells.find({r, c}) == VisitedCells.end())
            {
                // Found the start of a new chain
                std::vector<std::pair<size_t, size_t>> CurrentChain;
                std::queue<std::pair<size_t, size_t>> q;

                // Start BFS from this cell
                q.push({r, c});
                VisitedCells.insert({r, c});

                size_t min_r = r, max_r = r, min_c = c, max_c = c;

                while (!q.empty())
                {
                    std::pair<size_t, size_t> CurrentCell = q.front();
                    q.pop();
                    CurrentChain.push_back(CurrentCell);

                    // Update bounding box
                    min_r = std::min(min_r, CurrentCell.first);
                    max_r = std::max(max_r, CurrentCell.first);
                    min_c = std::min(min_c, CurrentCell.second);
                    max_c = std::max(max_c, CurrentCell.second);


                    // Check all 8 neighbours
                    for (int dr = -1; dr <= 1; ++dr)
                    {
                        for (int dc = -1; dc <= 1; ++dc)
                        {
                            if (dr == 0 && dc == 0) continue; // Skip the cell itself

                            size_t NewRow = CurrentCell.first + dr;
                            size_t NewCol = CurrentCell.second + dc;

                            // Check bounds
                            if (NewRow < rows && NewCol < cols)
                            {
                                std::pair<size_t, size_t> Neighbour = {NewRow, NewCol};
                                // Check if neighbour is alive and not visited
                                if (cells[NewRow * cols + NewCol] && VisitedCells.find(Neighbour) == VisitedCells.end())
                                {
                                    VisitedCells.insert(Neighbour);
                                    q.push(Neighbour);
                                }
                            }
                        }
                    }
                }

                // Create a minimal grid for the found chain
                if (!CurrentChain.empty())
                {
                    size_t SLRows = max_r - min_r + 1; 
                    size_t SLCols = max_c - min_c + 1;
                    Grid SLGrid(SLRows + 2, SLCols + 2); // Intialise subgrid with padded layer
                    
                    // Populate inner cells of the subgrid
                    for (const auto& cell : CurrentChain)
                    {
                        SLGrid.set(cell.first - min_r + 1, cell.second - min_c + 1, true);
                    }
                    StillLifeGrids.emplace_back(std::move(SLGrid));
                }
            }
        }
    }

    return StillLifeGrids;
}

//%% GetCells 
/**
 * @brief Returns a pointer to the cell data of the grid.
 *
 * @return A unique pointer to the cell data.
 */
std::unique_ptr<bool[]> Grid::GetCells() const
{
    // Return a copy of the cells
    auto cellsCopy = std::make_unique<bool[]>(size);
    std::memcpy(cellsCopy.get(), cells.get(), size * sizeof(bool));
    return cellsCopy;
}

//%% Equality operator
/**
 * @brief Overloads == operator for Grid class objects.
 *
 * Two grids are considered equal if they have the same dimensions and the same cell states.
 *
 * @param lhs The first grid.
 * @param rhs The second grid.
 * @return true if the grids are equal, false otherwise.
 */
bool operator==(const Grid& lhs, const Grid& rhs) 
{
    if (lhs.rows != rhs.rows || lhs.cols != rhs.cols) 
    {
        return false;
    }
    return std::memcmp(lhs.cells.get(), rhs.cells.get(), lhs.size * sizeof(bool)) == 0;
}

//%% Assignment operator
/**
 * @brief Overloads = operator for Grid class objects.
 *
 * Assigns the state of one grid to another.
 *
 * @param other The grid to assign from.
 * @return A reference to the current grid.
 */
Grid& Grid::operator=(const Grid& other)
{
    if (this != &other) // Check for self-assignment
    {
        // Check if dimensions are different
        if (rows != other.rows || cols != other.cols) 
        {
            // Dimensions differ, need to reallocate
            rows = other.rows;
            cols = other.cols;
            size = other.size;
            cells = std::make_unique<bool[]>(size); 
        }
        // Copy the cell data
        std::memcpy(cells.get(), other.cells.get(), size * sizeof(bool));
    }
    return *this; 
}

}// namespace gol
