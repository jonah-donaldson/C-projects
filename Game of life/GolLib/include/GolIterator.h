#ifndef gol_h
#define gol_h

#include "GolGrid.h"

namespace gol 
{

// Iterator class for Game of Life simulation
// This class is responsible for iterating through the grid and updating the state of cells
// based on the rules of Conway's Game of Life
class Iterator
{
private:
    /// @cond Private member variables
    // Reference to the grid being iterated over
    // Temporary grid to store the next state
    // Current generation count
    Grid& grid;
    Grid TempGrid;
    size_t generation;

public:
    /// @cond Constructor
    Iterator(Grid& grid);

    /// @cond  Function to iterate object by a single step
    void takeStep();

    /// @cond  Function that takes step and checks for still life pattern.
    bool StepSL();

    /// @cond  Function to print current state of the grid
    void printGrid() const;

    /// @cond  Function to get the current iteration count
    size_t GetIteration() const { return generation; }

    /// @cond  Function to get the current grid
    Grid GetGrid() const { return grid; }
};

} // namespace gol



#endif 