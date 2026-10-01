cmake_minimum_required(VERSION 3.10)
project(generator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(generator
    src/main.cpp
    src/template_parser.cpp
    src/json_parser.cpp
    src/module_loader.cpp
)
target_link_libraries(generator dl)

add_library(math MODULE modules/math.cpp)
set_target_properties(math PROPERTIES
    PREFIX "lib"
    OUTPUT_NAME "math"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/modules"
)

add_library(csv MODULE modules/csv.cpp)
set_target_properties(csv PROPERTIES
    PREFIX "lib"
    OUTPUT_NAME "csv"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/modules"
)