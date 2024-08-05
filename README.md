# EASTL Examples
A work-in-progress guide to integrating and using the Electronic Arts Standard Template Library (EASTL).

## Build Instructions
This repository includes EASTL as a git submodule, which can be updated and built using a helper script (Scripts/BuildEASTL.sh).

- Navigate to the root of the project directory in a shell script compatible terminal.
  Eg: Git BASH
- From the root directory, execute Scripts/BuildEASTL.sh
    ```Shell
    cd EASTLExamples
    ./Scripts/BuildEASTL.sh
    ```
- Once completed, the EASTL submodule should be updated and debug/release libraries should be
  present under "external/EASTL/build/debug" and "external/EASTL/build/release"
- Editors that are compatible with CMake projects should be able to configure and build the project
  at this point by simply pointing them to the root 'CMakeLists.txt' file.
- For command line builds, create a build directory and configure the project from within it.
    ```Shell
    mkdir build
    cd build
    cmake ..
    ```
- Once configured, specify a build type and build
    ```Shell
    cmake -DCMAKE_BUILD_TYPE=Release ..
    # Replace 'N' with the desired number of parallel jobs
    cmake --build . -- -jN
    ```