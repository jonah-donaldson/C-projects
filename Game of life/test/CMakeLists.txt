add_executable(GolTests GolTests.cpp)
target_link_libraries(GolTests PUBLIC GameOfLifeLib Catch2 Catch2::Catch2WithMain)

add_executable(GolTests GolTests.cpp)

target_link_libraries(GolTests 
    PRIVATE 
        Catch2::Catch2WithMain 
        GolLib 
        OpenMP::OpenMP_CXX 
        MPI::MPI_CXX
)

add_test(NAME GameOfLifeTests COMMAND GolTests)