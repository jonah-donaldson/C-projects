add_executable(TestOptimisation TestOptimisation.cpp)
target_link_libraries(TestOptimisation PUBLIC  Catch2 Catch2::Catch2WithMain OptimisationLib)

add_test(NAME ImgReconTests COMMAND TestOptimisation)

set_tests_properties(ImgReconTests PROPERTIES 
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.."
)