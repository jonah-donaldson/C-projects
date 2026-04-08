add_executable(GolTests GolTests.cpp)
target_link_libraries(GolTests PUBLIC GameOfLifeLib Catch2 Catch2::Catch2WithMain)

