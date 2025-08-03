cmake_minimum_required(VERSION 3.20)
project(neuron VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Treat warnings as errors
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Dependencies
find_package(CURL REQUIRED)

include(FetchContent)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG v3.1.1
)
FetchContent_MakeAvailable(cxxopts)

# Sources
set(SOURCES
    src/main.cpp
    src/cli.cpp
    src/config.cpp
    src/ai_client.cpp
)

# Define the executable target BEFORE linking
add_executable(neuron ${SOURCES})

# Then link the dependencies
target_link_libraries(neuron PRIVATE CURL::libcurl)
target_link_libraries(neuron PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(neuron PRIVATE cxxopts::cxxopts)
