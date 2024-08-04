# EASTL Examples
A work-in-progress guide to integrating and using the Electronic Arts Standard Template Library.

## Build Instructions
This project includes EASTL as a git submodule and a helper script (BuildEASTL.sh) to update and
build the library.

- Navigate to the root of the project directory in your shell script compatible terminal of choice.
  Eg: Git BASH
- From the root directory, execute Scripts/BuildEASTL.sh
- Once completed, the EASTL submodule should be updated and debug/release libraries should be
  present under "external/EASTL/build/debug" and "external/EASTL/build/release"
- Editors that are compatible with CMake projects should be able to configure and build the project
  at this point by simply pointing them to the root 'CMakeLists.txt' file.
- For command line builds, create a build directory and configure the project from within it.
    ```cmake
    mkdir build
    cd build
    cmake ..
    ```
- Once configured, specify a build type and build
    ```cmake
    cmake -DCMAKE_BUILD_TYPE=Release ..
    # Replace 'N' with the desired number of parallel jobs
    cmake --build . -- -jN
    ```
