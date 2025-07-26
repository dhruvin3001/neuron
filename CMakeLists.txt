cmake_minimum_required(VERSION 3.20)
project(neuron VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Treat all warnings as errors
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# Output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include public headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Dependency management
# find_package(...)

# Source files
set(SOURCES
    src/main.cpp
    src/cli.cpp
    src/config.cpp
    src/ai_client.cpp
)

# Executable
add_executable(neuron ${SOURCES})
