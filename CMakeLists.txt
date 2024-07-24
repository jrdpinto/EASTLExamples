# ROOT CMake

cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("EASTLString" LANGUAGES CXX)

# Collect all subdirectories
file(GLOB sources "*/CMakeLists.txt")

# Iterate over each subdirectory and add it as a subdirectory in the project
foreach(source ${sources})
    message("Directory: ${source}")
    get_filename_component(subdir ${source} DIRECTORY)
    add_subdirectory(${subdir})
endforeach()
