# ROOT CMake
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("EASTLExamples" LANGUAGES CXX)

message("Current directory: ${CMAKE_CURRENT_SOURCE_DIR}")
set(EASTL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/EASTL)
message("EASTL root directory: ${EASTL_ROOT_DIR}")

if(NOT EXISTS ${EASTL_ROOT_DIR})
    message(FATAL_ERROR "EASTL root directory does not exist: ${EASTL_ROOT_DIR}.")
    message("Try running the Scripts/BuildEASTL.sh script to update and build EASTL.")
endif()

include_directories (
    ${EASTL_ROOT_DIR}/include
    ${EASTL_ROOT_DIR}/test/packages/EAAssert/include
    ${EASTL_ROOT_DIR}/test/packages/EABase/include/Common
    ${EASTL_ROOT_DIR}/test/packages/EAMain/include
    ${EASTL_ROOT_DIR}/test/packages/EAStdC/include
    ${EASTL_ROOT_DIR}/test/packages/EATest/include
    ${EASTL_ROOT_DIR}/test/packages/EAThread/include)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE_INIT}")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EASTL_BUILD_DIR ${EASTL_ROOT_DIR}/build/debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(EASTL_BUILD_DIR ${EASTL_ROOT_DIR}/build/release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(EASTL_LIB_NAME EASTL.lib)
else()
    set(EASTL_LIB_NAME libEASTL.a)
endif()

message("Looking for ${EASTL_LIB_NAME} in ${EASTL_BUILD_DIR}")
file(GLOB_RECURSE LIBS ${EASTL_BUILD_DIR}/*${EASTL_LIB_NAME})
foreach(lib ${LIBS})
    message("Found ${lib}")
    set(EASTL_LIBRARY ${lib})
    break()
endforeach()

if(NOT EXISTS ${EASTL_LIBRARY})
    message(FATAL_ERROR "Could not find ${EASTL_LIB_NAME} in ${EASTL_BUILD_DIR}")
    message("Ensure that the Scripts/BuildEASTL.sh script builds EASTL without errors.")
endif()

# Collect all subdirectories
file(GLOB sources "*/CMakeLists.txt")

# Iterate over each subdirectory and add it to the project
foreach(source ${sources})
    message("Directory: ${source}")
    get_filename_component(subdir ${source} DIRECTORY)
    add_subdirectory(${subdir})
endforeach()
