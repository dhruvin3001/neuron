cmake_minimum_required(VERSION 3.20)
project(neuron VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build configuration options
option(BUILD_STATIC "Build static binary" OFF)
option(BUILD_TESTS "Build tests" OFF)

# Configure static linking
if(BUILD_STATIC)
    message(STATUS "Building static binary")
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    
    # Platform-specific static linking
    if(APPLE)
        # macOS: Can't fully static link, but minimize dependencies
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-search_paths_first")
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version")
    else()
        # Linux: Full static linking
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    endif()
endif()

# Compiler flags
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Werror -pedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Dependencies
if(BUILD_STATIC)
    # For static builds, use pkg-config to find static libraries
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(CURL REQUIRED libcurl)
    set(CURL_LIBRARIES ${CURL_STATIC_LIBRARIES})
    set(CURL_LDFLAGS ${CURL_STATIC_LDFLAGS})
else()
    find_package(CURL REQUIRED)
endif()

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

# Define the executable target
add_executable(neuron ${SOURCES})

# Link libraries based on build type
if(BUILD_STATIC)
    target_link_libraries(neuron PRIVATE 
        nlohmann_json::nlohmann_json
        cxxopts::cxxopts
        ${CURL_LIBRARIES}
    )
    target_link_options(neuron PRIVATE ${CURL_LDFLAGS})
else()
    target_link_libraries(neuron PRIVATE 
        CURL::libcurl
        nlohmann_json::nlohmann_json
        cxxopts::cxxopts
    )
endif()

# Installation
include(GNUInstallDirs)
install(TARGETS neuron
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# CPack configuration
set(CPACK_PACKAGE_NAME "neuron")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AI-powered command-line assistant")
set(CPACK_PACKAGE_VENDOR "Neuron Team")

include(CPack)
