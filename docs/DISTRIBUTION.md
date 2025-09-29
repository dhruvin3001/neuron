# Neuron AI - Distribution Guide

## Overview

Neuron AI is distributed as static binaries for multiple platforms, providing zero-dependency installation and broad compatibility. This document outlines the distribution architecture, build processes, and deployment procedures.

## Supported Platforms

| Platform | Architecture | Binary Name | Minimum Version |
|----------|-------------|-------------|-----------------|
| macOS | x86_64 (Intel) | `neuron-macos-x86_64` | macOS 10.15 (Catalina) |
| macOS | arm64 (Apple Silicon) | `neuron-macos-arm64` | macOS 11.0 (Big Sur) |
| Linux | x86_64 | `neuron-linux-x86_64` | glibc 2.17+ |
| Linux | arm64 (aarch64) | `neuron-linux-arm64` | glibc 2.17+ |

## Installation Methods

### Quick Install (Recommended)

```bash
curl -sSL https://raw.githubusercontent.com/dhruvin3001/neuron/main/install.sh | bash
```

The installer automatically:
- Detects your platform and architecture
- Downloads the appropriate binary
- Installs to `/usr/local/bin/neuron`
- Verifies the installation
- Provides setup instructions

### Manual Installation

1. **Download the binary for your platform** from the [releases page](https://github.com/dhruvin3001/neuron/releases)

2. **Make it executable:**
   ```bash
   chmod +x neuron-*
   ```

3. **Move to PATH:**
   ```bash
   sudo mv neuron-* /usr/local/bin/neuron
   ```

4. **Verify installation:**
   ```bash
   neuron --version
   ```

### Version-Specific Installation

```bash
# Install specific version
curl -sSL https://raw.githubusercontent.com/dhruvin3001/neuron/main/install.sh | bash -s -- --version=1.0.0

# Install from specific GitHub release
wget https://github.com/dhruvin3001/neuron/releases/download/v1.0.0/neuron-linux-x86_64
chmod +x neuron-linux-x86_64
sudo mv neuron-linux-x86_64 /usr/local/bin/neuron
```

## Build System Architecture

### Static Linking Strategy

Neuron AI uses static linking to minimize runtime dependencies:

- **macOS**: Semi-static linking (links against system frameworks)
- **Linux**: Full static linking (including libgcc, libstdc++)

This approach ensures:
- No dependency conflicts
- Broader compatibility
- Simplified deployment
- Consistent behavior across systems

### Build Process

#### Local Development

```bash
# Standard development build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Static binary build
./scripts/build-static.sh
```

#### Production Builds

Production builds are automated via GitHub Actions on:
- Version tags (`v*`)
- Manual workflow dispatch
- Pull requests (for testing)

The CI/CD pipeline:
1. Builds binaries for all supported platforms
2. Runs automated tests
3. Generates checksums
4. Creates GitHub releases
5. Uploads all artifacts

### Cross-Compilation

ARM64 Linux builds use cross-compilation:
```bash
# Automatic in CI environment
NEURON_OS=Linux NEURON_ARCH=arm64 CI=true ./scripts/build-static.sh
```

## Release Management

### Version Scheme

Neuron AI follows [Semantic Versioning](https://semver.org/):
- `MAJOR.MINOR.PATCH` (e.g., `1.2.3`)
- Pre-release: `1.2.3-beta.1`
- Release candidates: `1.2.3-rc.1`

### Release Process

1. **Tag the release:**
   ```bash
   git tag v1.2.3
   git push origin v1.2.3
   ```

2. **GitHub Actions automatically:**
   - Builds all platform binaries
   - Runs test suite
   - Creates release with notes
   - Uploads signed artifacts

3. **Release includes:**
   - Binary files for all platforms
   - SHA256 checksums (`checksums.txt`)
   - Auto-generated release notes
   - Installation instructions

### Security

#### Checksums
All releases include SHA256 checksums for verification:

```bash
# Download and verify
wget https://github.com/dhruvin3001/neuron/releases/download/v1.0.0/neuron-linux-x86_64
wget https://github.com/dhruvin3001/neuron/releases/download/v1.0.0/checksums.txt
sha256sum -c checksums.txt --ignore-missing
```

#### Code Signing
*Note: Code signing is planned for future releases*

## Dependencies

### Runtime Dependencies
- **macOS**: System frameworks only (libcurl, libz, libc++)
- **Linux**: Statically linked (no external dependencies)

### Build Dependencies
- CMake 3.20+
- C++20 compatible compiler
- pkg-config
- libcurl development headers
- Platform-specific build tools

## Package Managers

### Homebrew (macOS)
*Coming Soon*
```bash
brew install dhruvin3001/tap/neuron
```

### APT/DEB (Ubuntu/Debian)
*Planned*
```bash
sudo apt install neuron-ai
```

### Snap (Universal Linux)
*Planned*
```bash
sudo snap install neuron-ai
```

## Docker

### Official Images
*Coming Soon*
```bash
docker run -it dhruvin3001/neuron:latest
```

### Multi-Architecture Support
Docker images will support:
- `linux/amd64`
- `linux/arm64`

## Enterprise Deployment

### Air-Gapped Environments
*Coming Soon*

1. Download binaries from releases page
2. Transfer to target systems
3. Install manually following standard procedure
4. No internet connection required post-installation

### Configuration Management

Neuron AI supports configuration via:
- Environment variables
- Configuration files
- Command-line arguments

## Troubleshooting

### Common Issues

**Binary won't execute on Linux:**
```bash
# Check glibc version
ldd --version

# For older systems, try the static binary
./neuron-linux-x86_64 --version
```

**Permission denied on macOS:**
```bash
# Remove quarantine attribute
xattr -d com.apple.quarantine neuron-macos-arm64
```

**Installation verification fails:**
```bash
# Check if binary is in PATH
which neuron

# Check if directory is in PATH
echo $PATH | grep -o '/usr/local/bin'
```

### Support

- **Issues**: [GitHub Issues](https://github.com/dhruvin3001/neuron/issues)
- **Discussions**: [GitHub Discussions](https://github.com/dhruvin3001/neuron/discussions)

---

*For technical details about the build system, see [BUILD.md](./BUILD.md)*
