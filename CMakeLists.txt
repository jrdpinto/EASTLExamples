# ROOT CMake
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 17)

project("EASTLExamples" LANGUAGES CXX)

message("Current directory: ${CMAKE_CURRENT_SOURCE_DIR}")
set(EASTL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/EASTL)
message("EASTL root directory: ${EASTL_ROOT_DIR}")
set(EASTL_BUILD_DIR ${EASTL_ROOT_DIR}/build)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(EASTL_LIB_NAME libEASTL.lib)
else()
    set(EASTL_LIB_NAME libEASTL.a)
endif()

include_directories (
    ${EASTL_ROOT_DIR}/include
    ${EASTL_ROOT_DIR}/test/packages/EAAssert/include
    ${EASTL_ROOT_DIR}/test/packages/EABase/include/Common
    ${EASTL_ROOT_DIR}/test/packages/EAMain/include
    ${EASTL_ROOT_DIR}/test/packages/EAStdC/include
    ${EASTL_ROOT_DIR}/test/packages/EATest/include
    ${EASTL_ROOT_DIR}/test/packages/EAThread/include)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(EASTL_LIBRARY ${EASTL_BUILD_DIR}/debug/${EASTL_LIB_NAME})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
  set(EASTL_LIBRARY ${EASTL_BUILD_DIR}/release/${EASTL_LIB_NAME})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

set(BUILD_EASTL_ERROR_MSG "Try running the Scripts/BuildEASTL.sh script to update and build EASTL.")

if(NOT EXISTS ${EASTL_ROOT_DIR})
    message(FATAL_ERROR "EASTL root directory does not exist: ${EASTL_ROOT_DIR}.
        /n${BUILD_EASTL_ERROR_MSG}")
endif()

if(NOT EXISTS ${EASTL_LIBRARY})
    message(FATAL_ERROR "EASTL library does not exist: ${EASTL_LIBRARY}
        /n${BUILD_EASTL_ERROR_MSG}")
endif()

# Collect all subdirectories
file(GLOB sources "*/CMakeLists.txt")

# Iterate over each subdirectory and add it to the project
foreach(source ${sources})
    message("Directory: ${source}")
    get_filename_component(subdir ${source} DIRECTORY)
    add_subdirectory(${subdir})
endforeach()
