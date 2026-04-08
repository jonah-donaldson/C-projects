#include <iostream>
#include <omp.h>
#include "GolIterator.h"
#include "GolGrid.h"
#include <utility> // For std::pair
#include <memory> // For std::unique_ptr

namespace gol 
{
//%% Constructor with rows and columns
/**
 * @brief Constructor for the Iterator class.
 *
 * @param Takes in reference to a Grid object to be iterated over.
 */
Iterator::Iterator(Grid& grid) 
    : grid(grid), TempGrid(grid), generation(0) {}
 
///%% TakeStep
/**
 * @brief Takes a step in the simulation, updating the grid based on the rules of Conway's Game of Life.
 * Rules can be found here: https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
 *
 * @param grid Reference to the Grid object to be updated.
 * @note camelCase was used for function name as per assigment documentation.
 */
void Iterator::takeStep() 
{
    #pragma omp parallel for collapse(2) // Parallelise the outer loops
    for (size_t i = 0; i < grid.GetRows(); ++i) 
    {
        for (size_t j = 0; j < grid.GetCols(); ++j) 
        {
            size_t AliveNeighbours = grid.NumberAliveNeighbours(i, j);
            bool CurrentState = grid.get(i, j);

            if (CurrentState && (AliveNeighbours < 2 || AliveNeighbours > 3)) 
            {
                TempGrid.set(i, j, false); // Cell dies
            } 
            else if (!CurrentState && AliveNeighbours == 3) 
            {
                TempGrid.set(i, j, true); // Cell becomes alive
            } 
        }
    }
    grid = TempGrid; // Turn the left grid into the right grid
    generation++;
}

///%% StillLifeStep
/**
 * @brief Takes a step in the simulation and checks for still life patterns.
 * Still life patterns are defined as N = N+1.
 */
bool Iterator::StepSL()
{
    bool StillLife = false;
    #pragma omp parallel for collapse(2) // Parallelise the outer loops
    for (size_t i = 0; i < grid.GetRows(); ++i) 
    {
        for (size_t j = 0; j < grid.GetCols(); ++j) 
        {
            size_t AliveNeighbours = grid.NumberAliveNeighbours(i, j);
            bool CurrentState = grid.get(i, j);

            if (CurrentState && (AliveNeighbours < 2 || AliveNeighbours > 3)) 
            {
                TempGrid.set(i, j, false); // Cell dies
            } 
            else if (!CurrentState && AliveNeighbours == 3) 
            {
                TempGrid.set(i, j, true); // Cell becomes alive
            } 
        }
    }
    if (grid == TempGrid)
    {
        // If the current grid state is the same as the last seen pattern, we have a still life
        StillLife = true;
        generation++;
        return StillLife;
    }
    grid = TempGrid; // Turn the left grid into the right grid
    generation++;
    return StillLife;
}

/**
 * @brief Prints the current state of the grid to the console.
 * @note camelCase was used for function name as per assigment documentation.  
 */
void Iterator::printGrid() const
{ 
    grid.print();
    std::cout << "\n"; // Add newline as separator between grid prints
}

} // namespace gol