# Neuron AI - Build System Documentation

## Overview

This document describes the technical details of the Neuron AI build system, including CMake configuration, static linking strategies, and cross-compilation setup.

## Build System Components

### CMakeLists.txt Configuration

The build system uses CMake with custom configurations for static linking:

```cmake
# Static build option
option(BUILD_STATIC "Build static binary" OFF)

# Platform-specific static linking
if(BUILD_STATIC)
    if(APPLE)
        # macOS: Semi-static approach
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-search_paths_first")
        set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
    else()
        # Linux: Full static linking
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    endif()
endif()
```

### Build Script (`scripts/build-static.sh`)

The automated build script handles:
- Platform detection (`uname -s`, `uname -m`)
- Architecture normalization
- Environment variable overrides
- Cross-compilation configuration
- Binary testing and validation

#### Environment Variables

| Variable | Description | Example |
|----------|-------------|---------|
| `NEURON_OS` | Override OS detection | `Darwin`, `Linux` |
| `NEURON_ARCH` | Override architecture | `x86_64`, `arm64` |
| `CI` | Enable CI-specific features | `true` |

## Platform-Specific Build Details

### macOS Builds

**Characteristics:**
- Semi-static linking (system frameworks remain dynamic)
- Universal binary support via `CMAKE_OSX_ARCHITECTURES`
- Deployment target: macOS 10.15+

**Dependencies:**
```bash
# Required for building
brew install cmake curl pkg-config

# System frameworks (linked dynamically)
- /usr/lib/libcurl.4.dylib
- /System/Library/Frameworks/LDAP.framework
- /usr/lib/libz.1.dylib
- /usr/lib/libc++.1.dylib
- /usr/lib/libSystem.B.dylib
```

**Build Command:**
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_STATIC=ON \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
    -DCMAKE_OSX_ARCHITECTURES=arm64  # For ARM64 specific
```

### Linux Builds

**Characteristics:**
- Full static linking
- Cross-compilation support for ARM64
- glibc 2.17+ compatibility

**Dependencies:**
```bash
# Ubuntu/Debian build dependencies
sudo apt-get install \
    build-essential \
    cmake \
    pkg-config \
    libcurl4-openssl-dev \
    curl

# ARM64 cross-compilation
sudo apt-get install \
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    libcurl4-openssl-dev:arm64
```

**Native Build:**
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_STATIC=ON
```

**Cross-Compilation (ARM64):**
```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_STATIC=ON \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
    -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
    -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
```

## CI/CD Pipeline (GitHub Actions)

### Matrix Strategy

The GitHub Actions workflow builds for multiple platform combinations:

```yaml
strategy:
  matrix:
    include:
      - os: ubuntu-20.04
        platform: linux
        arch: x86_64
      - os: ubuntu-20.04
        platform: linux
        arch: arm64
      - os: macos-12
        platform: macos
        arch: x86_64
      - os: macos-14
        platform: macos
        arch: arm64
```

### Build Steps

1. **Environment Setup**
   - Install platform-specific dependencies
   - Configure cross-compilation toolchains (ARM64 Linux)
   - Set up build environment variables

2. **Build Execution**
   ```bash
   ./scripts/build-static.sh
   env:
     NEURON_OS: ${{ startsWith(matrix.os, 'ubuntu') && 'Linux' || 'Darwin' }}
     NEURON_ARCH: ${{ matrix.arch }}
     CI: true
   ```

3. **Artifact Management**
   - Upload binaries as GitHub Actions artifacts
   - Generate SHA256 checksums
   - Create GitHub releases with proper metadata

## Dependency Management

### External Libraries

| Library | Version | Purpose | Linking |
|---------|---------|---------|---------|
| nlohmann/json | 3.11+ | JSON parsing | Static (header-only) |
| cxxopts | 3.0+ | CLI argument parsing | Static |
| libcurl | System | HTTP client | Platform-dependent |

### Dependency Resolution

**FetchContent (CMake):**
```cmake
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)

FetchContent_Declare(
    cxxopts
    GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
    GIT_TAG v3.1.1
)
```

**pkg-config (libcurl):**
```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(CURL REQUIRED libcurl)
```

## Static Linking Strategies

### Why Static Linking?

1. **Deployment Simplicity**: No dependency management on target systems
2. **Version Consistency**: Eliminates library version conflicts
3. **Portability**: Runs on systems without dev libraries
4. **Distribution**: Single file deployment

### Platform Differences

**macOS Limitations:**
- System frameworks cannot be statically linked
- Approach: Link against stable system libraries
- Result: Minimal dependencies, broad compatibility

**Linux Advantages:**
- Full static linking possible
- glibc static linking supported
- Result: Zero external dependencies

### Binary Size Optimization

Current optimizations:
- Release builds with `-O3`
- Debug symbol stripping
- Dead code elimination
- Link-time optimization (LTO) *planned*

## Testing and Validation

### Automated Tests

**Build Validation:**
```bash
# Binary execution test
./bin/neuron --help >/dev/null 2>&1
```

## Troubleshooting Build Issues

### Common Problems

**macOS: Unsupported linker flags**
```
error: unsupported option '-static-libgcc'
```
*Solution: Use macOS-specific linking strategy*

**Linux: Missing cross-compilation tools**
```
cmake: Could not find aarch64-linux-gnu-gcc
```
*Solution: Install cross-compilation toolchain*

**All Platforms: Missing dependencies**
```
Could not find libcurl
```
*Solution: Install pkg-config and development headers*

### Debug Build Issues

```bash
# Enable verbose CMake output
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON

# Debug linking issues
make VERBOSE=1

# Check library paths
pkg-config --cflags --libs libcurl
```

## Performance Considerations

### Build Times
- **Local builds**: ~2-3 minutes
- **CI builds**: ~5-8 minutes per platform
- **Optimization**: Parallel compilation (`make -j`)

### Binary Sizes
- **Current size**: ~417KB
- **Size breakdown**: ~60% code, ~40% dependencies
- **Optimization potential**: LTO, binary packing

---

*For user-facing distribution information, see [DISTRIBUTION.md](./DISTRIBUTION.md)*
