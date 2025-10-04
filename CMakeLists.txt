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
        # Linux: Partial static linking (static libstdc++/libgcc, dynamic system libs)
        # This avoids issues with glibc and other system libraries that should be dynamic
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
        # Don't use full -static flag as it causes issues with system libraries
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
if(BUILD_STATIC AND UNIX AND NOT APPLE)
    # For Linux static builds, build curl from source to avoid dependency issues
    message(STATUS "Building CURL from source for static linking")
    
    FetchContent_Declare(
        curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG curl-8_4_0
    )
    
    # Configure curl to build as static library with minimal dependencies
    set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(CURL_STATICLIB ON CACHE BOOL "" FORCE)
    set(HTTP_ONLY ON CACHE BOOL "" FORCE)
    set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_LDAPS ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_RTSP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_PROXY ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_DICT ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_TELNET ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_TFTP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_POP3 ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_IMAP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_SMTP ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_GOPHER ON CACHE BOOL "" FORCE)
    set(CURL_DISABLE_SMB ON CACHE BOOL "" FORCE)
    set(USE_LIBIDN2 OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
    set(CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(curl)
    
    set(CURL_LIBRARIES libcurl)
else()
    # For macOS and non-static builds, use system curl
    find_package(CURL REQUIRED)
    set(CURL_LIBRARIES CURL::libcurl)
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
target_link_libraries(neuron PRIVATE 
    nlohmann_json::nlohmann_json
    cxxopts::cxxopts
    ${CURL_LIBRARIES}
)

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
