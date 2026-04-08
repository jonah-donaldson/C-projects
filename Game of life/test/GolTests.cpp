#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

#include "GolGrid.h"
#include "GolIterator.h"
#include <iostream>
#include <sstream>
#include <random>
#include <stdexcept>

using namespace Catch::Matchers;

/////////////////////////////////////////////////////////////////////////

// Constructor tests for the Grid class

/////////////////////////////////////////////////////////////////////////

TEST_CASE("Grid construction", "[Grid]") 
{
    gol::Grid grid(5, 5);
    // Verify that all cells are dead initially
    for (size_t i = 0; i < 5; ++i) 
    {
        for (size_t j = 0; j < 5; ++j) 
        {
            REQUIRE_FALSE(grid.get(i, j));
        }
    }
    REQUIRE(grid.NumberAlive() == 0);

    // Fails for invalid dimensions
    REQUIRE_THROWS_AS(gol::Grid(0, 5), std::invalid_argument);
}

TEST_CASE("Grid construction with alive cells", "[Grid]") 
{
    bool cells[] = {false, true, false, false, false,
                    false, false, true, false, false,
                    false, false, false, true, false,
                    false, false, false, false, true,
                    false, false, false, false, false};

    gol::Grid grid(5, 5, cells);
    
    // Check the number of alive cells
    REQUIRE(grid.NumberAlive() == 4);

    // Check the state of specific cells
    REQUIRE(grid.get(0, 1) == true);
    REQUIRE(grid.get(1, 2) == true);
    REQUIRE(grid.get(2, 3) == true);
    REQUIRE(grid.get(3, 4) == true);

    // Verify throws for invalid args
    REQUIRE_THROWS_AS(gol::Grid(5, 5, nullptr), std::invalid_argument);
    REQUIRE_THROWS_AS(gol::Grid(0, 5, cells), std::invalid_argument);
}

TEST_CASE("Grid random construction", "[Grid]") 
{
    std::random_device rd;
    std::mt19937 rng(rd());
    gol::Grid grid(10, 10, 20, rng);
    
    // Check that the number of alive cells is correct
    REQUIRE(grid.NumberAlive() == 20);

    // Check generation is random
    gol::Grid grid2(10, 10, 20, rng);
    // Check that the two grids are not equal
    // Note: May generate same sequence but this is highly unlikely. 
    REQUIRE_FALSE(grid == grid2);

    // Check that the alive cells are within bounds
    REQUIRE_THROWS_AS(gol::Grid(10, 10, 200, rng), std::invalid_argument);

    // Throws error for invalid dimensions
    REQUIRE_THROWS_AS(gol::Grid(0, 10, 20, rng), std::invalid_argument);
}

TEST_CASE("Grid file construction", "[Grid]") 
{
    // Create a temporary file with a grid representation
    std::string filename = "test/data/glider.txt";

    gol::Grid grid(filename);
    
    // Check the number of alive cells
    REQUIRE(grid.NumberAlive() == 5);

    // Check the state of specific cells
    REQUIRE(grid.get(2, 0) == true);
    REQUIRE(grid.get(3, 1) == true);
    REQUIRE(grid.get(1, 2) == true);
    REQUIRE(grid.get(2, 2) == true);
    REQUIRE(grid.get(3, 2) == true);

    // Check that errors are thrown for invalid file
    std::string InvalidFilename = "invalid_file.txt";
    REQUIRE_THROWS_AS(gol::Grid(InvalidFilename), std::runtime_error);

    // Check that errors are thrown for invalid grid format
    std::string InvalidGrid = "test/data/glider_badgrid.txt";
    REQUIRE_THROWS_AS(gol::Grid(InvalidGrid), std::runtime_error);

    // Check that errors are thrown for invalid cell values
    std::string InvalidCell = "test/data/glider_badcell.txt";
    REQUIRE_THROWS_AS(gol::Grid(InvalidCell), std::runtime_error);
}

TEST_CASE("Grid copy constructor", "[Grid]") 
{
    gol::Grid grid1(5, 5);
    grid1.set(2, 2, true); // Set a cell to alive

    gol::Grid grid2(grid1); // Copy constructor

    // Check that the copied grid has the same state
    REQUIRE(grid1 == grid2);
    
    // Modify the original grid and check that the copied grid remains unchanged
    grid1.set(0, 0, true);
    REQUIRE_FALSE(grid1 == grid2);
}

/////////////////////////////////////////////////////////////////////////

// Grid class function tests

/////////////////////////////////////////////////////////////////////////

TEST_CASE("Set and Get functions", "[Grid]") 
{
    gol::Grid grid(3, 3);
    // Set a few cells to alive
    grid.set(1, 1, true);
    grid.set(0, 2, true);
    // Verify using get()
    REQUIRE(grid.get(1, 1) == true);
    REQUIRE(grid.get(0, 2) == true);
    // Verify that other cells remain unchanged (dead)
    REQUIRE_FALSE(grid.get(0, 0));

    // Confirm the count of alive cells
    REQUIRE(grid.NumberAlive() == 2);
    
    // Check boundary conditions: invalid indices should throw std::out_of_range exception
    REQUIRE_THROWS_AS(grid.get(5, 4), std::out_of_range);
    REQUIRE_THROWS_AS(grid.get(4, 5), std::out_of_range);

    // Negtaive vals also throw exception
    // Note: this takes advantge of size_t wrapping around for negative values
    // Fundementally, this is the same as the above test.
    // It's not a bug it's a feature.
    REQUIRE_THROWS_AS(grid.get(-1, 3), std::out_of_range);
    REQUIRE_THROWS_AS(grid.get(3, -1), std::out_of_range);

    // Also test set throwing exception for out of bounds
    REQUIRE_THROWS_AS(grid.set(5, 4, true), std::out_of_range);
    REQUIRE_THROWS_AS(grid.set(4, 5, true), std::out_of_range);
}

TEST_CASE("Print function output", "[Grid]") 
{
    gol::Grid grid(2, 2);
    grid.set(0, 1, true); // Only one cell alive

    std::stringstream ss;
    auto CoutBuffer = std::cout.rdbuf(); // save original buffer
    std::cout.rdbuf(ss.rdbuf()); // redirect cout to ss

    grid.print();

    std::cout.rdbuf(CoutBuffer); // restore original buffer
 
    std::string output = ss.str();
    // Here, adjust the expected output according to your print format:
    std::string expected = "- o \n- - \n";
    REQUIRE(output == expected);
}

TEST_CASE("Cell alive neighbour counter", "[Grid]") 
{
    gol::Grid grid("test/data/glider.txt");
    // Check the number of alive neighbours for a specific cell
    REQUIRE(grid.NumberAliveNeighbours(2, 1) == 5); 
    REQUIRE(grid.NumberAliveNeighbours(2, 2) == 3); 
    REQUIRE(grid.NumberAliveNeighbours(1, 3) == 2); 
    // Check for a cell with no alive neighbours
    REQUIRE(grid.NumberAliveNeighbours(0, 0) == 0);
    // Check for a cell on the edge with one alive neighbour
    REQUIRE(grid.NumberAliveNeighbours(0, 1) == 1);
}

TEST_CASE("Grid cleave function", "[Grid]") 
{
    gol::Grid grid(4, 4);
    grid.set(0, 0, true); // top left
    grid.set(0, 3, true); // top right
    grid.set(3, 0, true); // bottom left
    grid.set(3, 3, true); // bottom right

    // Cleave the grid into 2x2 sub-grids
    std::vector<gol::Grid> SubGrids = grid.Cleave(2, 2);

    // Check the number of sub-grids
    REQUIRE(SubGrids.size() == 4);

    //SubGrids[0].print(); // Print the top-right sub-grid
    //SubGrids[1].print(); // Print the top-right sub-grid

    // Check the state of each sub-grid 
    // Also confirms row wise ordering of the sub-grids
    REQUIRE(SubGrids[0].get(0, 0) == true); // Top-left
    REQUIRE(SubGrids[1].get(0, 1) == true); // Top-right
    REQUIRE(SubGrids[2].get(1, 0) == true); // Bottom-left
    REQUIRE(SubGrids[3].get(1, 1) == true); // Bottom-right

    // Check for invalid cleave parameters
    REQUIRE_THROWS_AS(grid.Cleave(5, 5), std::invalid_argument);

    // Check the number of sub-grids
    REQUIRE(SubGrids.size() == 4);

    // Check the dimensions of each sub-grid
    for (const auto& subGrid : SubGrids) 
    {
        REQUIRE(subGrid.GetRows() == 2);
        REQUIRE(subGrid.GetCols() == 2);
        REQUIRE(subGrid.GetSize() == 4); // Total number of cells in each sub-grid
    }

    // Check for edge cases
    REQUIRE_THROWS_AS(grid.Cleave(5, 5), std::invalid_argument);
    REQUIRE_THROWS_AS(grid.Cleave(0, 1), std::invalid_argument);
    std::vector<gol::Grid> SubGrid = grid.Cleave(1,1); 
    REQUIRE(SubGrid.size() == 1); // 1x1 = 1 sub-grid
    REQUIRE(SubGrid[0] == grid); // Check that the sub-grid is equal to the original grid
    
    // Non even split for large grids
    gol::Grid grid2(11, 10);

    // Spllit into 3x3 sub-grids
    std::vector<gol::Grid> SubGrids2 = grid2.Cleave(3, 3);
    // 11%3 = 2 and 10%3 = 1 
    // Thus 2 rows and 1 column will be left over
    // Grids in first two rows will contain + 1 rows
    // Grids in first column will contain + 1 columns

    // Check sizes
    REQUIRE(SubGrids2.size() == 9); // 3x3 = 9 sub-grids
    for (size_t i = 0; i < 3; ++i) 
    {
        for (size_t j = 0; j < 3; ++j) 
        {
            if (i < 2)
            {
                REQUIRE(SubGrids2[i * 3 + j].GetRows() == 4); // First two rows have +1
            } 
            else if (j < 1)
            {
                REQUIRE(SubGrids2[i * 3 + j].GetCols() == 4); // First column has +1
            } 
            else if (j<1 && i<2)
            {
                REQUIRE(SubGrids2[i * 3 + j].GetRows() == 4); // First two rows have +1
                REQUIRE(SubGrids2[i * 3 + j].GetCols() == 4); // First column has +1
            }
            else
            {
                REQUIRE(SubGrids2[i * 3 + j].GetRows() == 3); // Remaining rows
                REQUIRE(SubGrids2[i * 3 + j].GetCols() == 3); // Remaining columns
            }
        }
    }
}

TEST_CASE("Still Life extraction", "[Grid]") 
{
    gol::Grid grid("test/data/still_lifes.txt");
    // Check the number of alive cells
    REQUIRE(grid.NumberAlive() == 19);

    // Extract still life patterns
    std::vector<gol::Grid> StillLifePatterns = grid.ExtractSL();

    // Print the still life patterns for manual verification
    // for (const auto& pattern : StillLifePatterns)
    // {
    //     pattern.print();
    //     std::cout << "\n";
    // }

    // Check the number of still life patterns
    REQUIRE(StillLifePatterns.size() == 4); 
}


TEST_CASE("Grid size getters", "[Grid]") 
{
    gol::Grid grid(5, 10);
    // Check the number of rows and columns
    REQUIRE(grid.GetRows() == 5);
    REQUIRE(grid.GetCols() == 10);
    REQUIRE(grid.GetSize() == 50); // Total number of cells
}

TEST_CASE("Grid cell getter", "[Grid]") 
{
    gol::Grid grid1(3, 3);
    grid1.set(1, 1, true); // Set a cell to alive
    
    // Make new grid to compare
    gol::Grid grid2(3, 3, grid1.GetCells().get());
    REQUIRE(grid1 == grid2); 
}

TEST_CASE("Grid equality operator", "[Grid]") 
{
    gol::Grid grid1(3, 3);
    gol::Grid grid2(3, 3);
    // Initially, grids are equal
    REQUIRE(grid1 == grid2);
    
    // Change grid1 and verify inequality
    grid1.set(0, 0, true);
    REQUIRE_FALSE(grid1 == grid2);
    
    // Also test grids of different dimensions
    gol::Grid grid3(4, 4);
    REQUIRE_FALSE(grid1 == grid3);

    // Ensure != operator works as expected
    REQUIRE(grid1 != grid3);
}

TEST_CASE("Grid Assigment operator", "[Grid]") 
{
    gol::Grid grid1(3, 3);
    grid1.set(0, 0, true);
    grid1.set(1, 1, true);

    gol::Grid grid2(3, 3);
    
    grid2 = grid1; // Assignment operator

    // Check that grid2 has the same state as grid1
    REQUIRE(grid1 == grid2);

    // Modify grid1 and check that grid2 remains unchanged
    grid1.set(2, 2, true);
    REQUIRE_FALSE(grid1 == grid2);
}

/////////////////////////////////////////////////////////////////////////

// Iterator class tests

/////////////////////////////////////////////////////////////////////////

TEST_CASE("Iterator construction", "[Iterator]") 
{
    gol::Grid grid(5, 5);
    gol::Iterator iterator(grid);
    // Check that the iterator is initialized correctly
    REQUIRE(iterator.GetIteration() == 0);
}

TEST_CASE("Iterator step function", "[Iterator]") 
{
    gol::Grid grid("test/data/glider.txt");

    // Create an iterator and take a step
    gol::Iterator iterator(grid);
    // iterator.printGrid(); // Manual check of the grid state
    iterator.takeStep();

    // Check Grid state after one step
    // iterator.printGrid();

    // Check the state of the grid after one step
    REQUIRE(grid.get(1, 2) == false); // Should die due to underpopulation
    REQUIRE(grid.get(2, 2) == true);  // Should survive
    REQUIRE(grid.get(3, 3) == false); // Should die due to overpopulation
    REQUIRE(grid.get(2, 3) == true);  // Should come to life

    // Check the number of alive cells after the step
    REQUIRE(grid.NumberAlive() == 5);

    iterator.takeStep(); // Take another step
    REQUIRE(grid.get(2, 2) == false); // Should die from overpopulation

    // Check iteration count
    REQUIRE(iterator.GetIteration() == 2);
}

TEST_CASE("Still Life Check functions", "[Iterator]") 
{
    // Initialise a grid with a known still life pattern
    gol::Grid grid(4, 4);
    grid.set(1, 1, true);
    grid.set(1, 2, true);
    grid.set(2, 1, true);
    grid.set(2, 2, true);
    // grid.print(); // Print the grid for debugging
    // std::cout << "\n";
    
    gol::Iterator iterator(grid);

    // Make a vector to contain the known still life patterns
    std::vector<gol::Grid> SeenPatterns;
    REQUIRE(SeenPatterns.empty() == true); // Check that the vector is empty

    // Check SLStep function
    REQUIRE(iterator.StepSL() == true); // Should return true

    // Check IsUniqueSL function
    std::vector<gol::Grid> SLPatterns = grid.ExtractSL(); // Extract the still life patterns

    REQUIRE(SLPatterns[0].IsUnique(SeenPatterns) == true); // Should add one new pattern   

    // Check again now that the pattern is in the vector
    SeenPatterns.push_back(SLPatterns[0]); // Add the pattern to the vector
    REQUIRE(SLPatterns[0].IsUnique(SeenPatterns) == false); // Should not add any new patterns

    // Check False SL
    gol::Grid grid2(5, 5);
    grid2.set(0, 0, true);
    grid2.set(0, 1, true);
    grid2.set(1, 0, true);
    grid2.set(1, 1, true);
    // grid2.print(); // Print the grid for debugging
    // std::cout << "\n";

    gol::Iterator iterator2(grid2);

    // Try for same patter different position
    REQUIRE(grid2.IsUnique(SeenPatterns) == true); // Should not add any new patterns

    // Test
    grid2.set(2, 2, true);
    REQUIRE(iterator2.StepSL() == false); // Should return false
    REQUIRE(iterator2.StepSL() == true); // Is now a new still life

    // Check that the new pattern is added
    std::vector<gol::Grid> SLPatterns2 = grid2.ExtractSL(); // Extract the still life patterns
    REQUIRE(SLPatterns2[0].IsUnique(SeenPatterns) == true); // Should add one new pattern

    // Test for more than one still life 
    // Note due to proximity of patterns this isn't a real still life but is irrelevant when testing.
    grid2.set(3, 4, true);
    grid2.set(3, 3, true);
    grid2.set(4, 4, true);
    grid2.set(4, 3, true);
    // grid2.print(); // Print the grid for debugging
    // std::cout << "\n";
    
    SLPatterns2 = grid2.ExtractSL(); // Extract the still life patterns
    SeenPatterns.clear(); // Clear the known patterns
    for (const auto& pattern : SLPatterns2)
    {
        REQUIRE(pattern.IsUnique(SeenPatterns) == true); // Should add one new pattern
    }

    // Print the known patterns
    // for (const auto& pattern : SeenPatterns)
    // {
    //     pattern.print();
    //     std::cout << "\n";
    // }
}



