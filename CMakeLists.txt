cmake_minimum_required (VERSION 2.8)
project ("Final Project")
# Set the version number
set (FinalProject_VERSION_MAJOR 1)
set (FinalProject_VERSION_MINOR 0)
set(CMAKE_BUILD_TYPE Debug)

# set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories("${PROJECT_BINARY_DIR}")

set(EXECUTABLE_NAME "shader")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory( src )

# Copy the resources directory to build
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_BINARY_DIR})
