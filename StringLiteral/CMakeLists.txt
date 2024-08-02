# StringLiteral root CMake

cmake_minimum_required (VERSION 3.8)

project("EASTLAnalysis")

# Collect all subdirectories
file(GLOB sources "*/CMakeLists.txt")

# Iterate over each subdirectory and add it as a subdirectory in the project
foreach(source ${sources})
    get_filename_component(subdir ${source} DIRECTORY)
    add_subdirectory(${subdir})
endforeach()
