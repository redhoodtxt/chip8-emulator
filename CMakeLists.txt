cmake_minimum_required(VERSION 3.10.0)
project(chip8)

# glfw settings
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# adding where to find the glfw and glad
add_subdirectory(external/glfw-3.4)
add_subdirectory(external/glad)

find_package(OpenGL REQUIRED)

# create executable 
add_executable(chip8
    ./src/chip8.cpp
    ./src/main.>cpp
    ./src/include/chip8.h
)

# include directories
target_include_directories(chip8 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw-3.4/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

# source files
target_sources(chip8 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/glad.c
)

# link the libraries
target_link_libraries(chip8 PRIVATE
    glfw
    glad
    OpenGL::GL
)

# set the C++ standards
set_target_properties(chip8 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)
