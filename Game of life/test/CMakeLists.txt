add_executable(GolTests GolTests.cpp)
target_link_libraries(GolTests 
    PRIVATE 
        GameOfLifeLib 
        Catch2::Catch2WithMain
)

add_test(NAME GameOfLifeTests COMMAND GolTests)