# EASTL String
project(EASTLString LANGUAGES CXX)

# Collect all source files
file(GLOB_RECURSE sources *.cpp *.c *.h)

# Add executable and link source files
add_executable(EASTLString ${sources})

# Link the EASTL static library
target_link_libraries(EASTLString ${EASTL_LIBRARY})