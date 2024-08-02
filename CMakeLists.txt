# ROOT CMake

cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("EASTLAnalysis" LANGUAGES CXX)

set(EASTL_ROOT_DIR external/EASTL)
set(EASTL_BUILD_DIR ${EASTL_ROOT_DIR}/build)

# Create an interface library to hold the include directories
add_library(eastl_includes INTERFACE)

# Include EASTL directories
include_directories (eastl_includes INTERFACE ${EASTL_ROOT_DIR}/include)
include_directories (eastl_includes INTERFACE ${EASTL_ROOT_DIR}/test/packages/EAAssert/include)
include_directories (eastl_includes INTERFACE ${EASTL_ROOT_DIR}/test/packages/EABase/include/Common)
include_directories (eastl_includes INTERFACE ${EASTL_ROOT_DIR}/test/packages/EAMain/include)
include_directories (eastl_includes INTERFACE ${EASTL_ROOT_DIR}/test/packages/EAStdC/include)
include_directories (eastl_includes INTERFACE ${EASTL_ROOT_DIR}/test/packages/EATest/include)
include_directories (eastl_includes INTERFACE ${EASTL_ROOT_DIR}/test/packages/EAThread/include)
set(EASTL_LIBRARY debug ${EASTL_BUILD_DIR}/debug/libEASTL.a release ${EASTL_BUILD_DIR}/release/libEASTL.a)

# Collect all subdirectories
file(GLOB sources "*/CMakeLists.txt")

# Iterate over each subdirectory and add it to the project
foreach(source ${sources})
    message("Directory: ${source}")
    get_filename_component(subdir ${source} DIRECTORY)
    add_subdirectory(${subdir})
endforeach()
