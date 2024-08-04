# EASTL String View
project(EASTLStringView LANGUAGES CXX)

# Collect all source files
file(GLOB_RECURSE sources *.cpp *.c *.h)

# Add executable and link source files
add_executable(EASTLStringView ${sources})

# Link the EASTL static library
target_link_libraries(EASTLStringView ${EASTL_LIBRARY})