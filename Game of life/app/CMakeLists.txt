add_executable(GolSimulator GolSimulator.cpp)
target_link_libraries(GolSimulator GameOfLifeLib)

add_executable(GolSimulatorMPI GolSimulatorMPI.cpp)
target_link_libraries(GolSimulatorMPI GameOfLifeLib MPI::MPI_CXX)

#For MPI App
add_executable(GolStillLifeFinder GolStillLifeFinder.cpp)
target_link_libraries(GolStillLifeFinder GameOfLifeLib MPI::MPI_CXX)
