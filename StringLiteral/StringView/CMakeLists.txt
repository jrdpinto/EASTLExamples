# EASTL String View

project(EASTLStringView LANGUAGES CXX)

# Collect all source files
file(GLOB_RECURSE sources *.cpp *.c *.h)

# Add executable and link source files
add_executable(EASTLStringView ${sources})

# Link the application against the external library and the common includes
#target_link_libraries(EASTLStringView PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/${EASTL_LIBRARY} eastl_includes)