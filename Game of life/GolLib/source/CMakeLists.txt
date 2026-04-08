add_library(GameOfLifeLib GolGrid.cpp GolIterator.cpp)
target_include_directories(GameOfLifeLib PUBLIC ${CMAKE_SOURCE_DIR}/GolLib/include)
#Add OpenMP
target_link_libraries(GameOfLifeLib PUBLIC OpenMP::OpenMP_CXX)