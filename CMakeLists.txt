cmake_minimum_required(VERSION 3.10) # Adjust the version as needed
project(MyProject) # Replace MyProject with your project name

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Add source files
file(GLOB SOURCES "*.cpp")

# Add the executable
add_executable(out ${SOURCES})

# Link libraries
target_link_libraries(out
    ${CMAKE_SOURCE_DIR}/libraylib.a
    opengl32
    gdi32
    winmm
)

# Optional: Define compile options if needed
# target_compile_options(out PRIVATE -Wall)
