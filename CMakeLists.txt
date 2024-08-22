cmake_minimum_required(VERSION 3.10) 
project(MyProject) # Replace MyProject with your project name

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

# Group sources into different logical targets if needed (optional)
# For example, if you have a core library, you might want to create a separate library target
# add_library(core STATIC ${CORE_SOURCES})

# Add the executable
add_executable(out ${SOURCES})

# Link libraries
target_link_libraries(out
    ${CMAKE_SOURCE_DIR}/lib/libraylib.a
    opengl32
    gdi32
    winmm
)

# Optional: Define compile options if needed
# target_compile_options(out PRIVATE -Wall)
