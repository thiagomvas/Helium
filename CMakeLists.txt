cmake_minimum_required(VERSION 3.10)
project(Helium)  # Replace MyProject with your project name

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib
)

# Add source files
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/lib/**/*.cpp
)

# Add the executable
add_executable(out ${SOURCES})

# Link libraries
target_link_libraries(out
    ${CMAKE_SOURCE_DIR}/lib/libraylib.a
    opengl32
    gdi32
    winmm
)

# Copy resources folder to build directory
add_custom_command(TARGET out POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
    COMMENT "Copying resources directory to build directory"
)

# Optional: Define compile options if needed
# target_compile_options(out PRIVATE -Wall)

# Optional: Define any additional compile definitions or flags if needed
# add_definitions(-DDEBUG)
