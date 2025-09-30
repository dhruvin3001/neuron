#!/bin/bash

set -e

echo "🔨 Building Neuron Static Binary..."

# Allow override from environment variables (for CI/CD)
OS=${NEURON_OS:-$(uname -s)}
ARCH=${NEURON_ARCH:-$(uname -m)}

echo "Building for: $OS-$ARCH"

# Normalize architecture names
case "$ARCH" in
    "x86_64"|"amd64")
        ARCH="x86_64"
        ;;
    "arm64"|"aarch64")
        ARCH="arm64"
        ;;
    *)
        echo "❌ Unsupported architecture: $ARCH"
        exit 1
        ;;
esac

# Create build directory
BUILD_DIR="build-static-$OS-$ARCH"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure based on OS and Architecture
case "$OS" in
    "Darwin")
        echo "🍎 Configuring for macOS..."
        if [ "$ARCH" = "arm64" ]; then
            cmake .. \
                -DCMAKE_BUILD_TYPE=Release \
                -DBUILD_STATIC=ON \
                -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
                -DCMAKE_OSX_ARCHITECTURES=arm64
        else
            cmake .. \
                -DCMAKE_BUILD_TYPE=Release \
                -DBUILD_STATIC=ON \
                -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15
        fi
        ;;
    "Linux")
        echo "🐧 Configuring for Linux..."
        if [ "$ARCH" = "arm64" ] && [ -n "$CI" ]; then
            # Cross-compilation for ARM64 in CI
            cmake .. \
                -DCMAKE_BUILD_TYPE=Release \
                -DBUILD_STATIC=ON \
                -DCMAKE_SYSTEM_NAME=Linux \
                -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
                -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
                -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++
        else
            # Native compilation
            cmake .. \
                -DCMAKE_BUILD_TYPE=Release \
                -DBUILD_STATIC=ON
        fi
        ;;
    *)
        echo "❌ Unsupported OS: $OS"
        exit 1
        ;;
esac

# Build
echo "🚀 Building..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Test the binary
echo "🧪 Testing binary..."
if ./bin/neuron --help >/dev/null 2>&1; then
    echo "✅ Binary test passed"
else
    echo "⚠️  Binary test failed, but continuing..."
fi

# Create distribution directory
DIST_DIR="../dist"
mkdir -p "$DIST_DIR"

# Copy binary with custom name if provided, otherwise use OS/ARCH suffix
if [ -n "$NEURON_NAME" ]; then
    BINARY_NAME="neuron-$NEURON_NAME"
else
    case "$OS" in
        "Darwin")
            BINARY_NAME="neuron-macos-$ARCH"
            ;;
        "Linux")
            BINARY_NAME="neuron-linux-$ARCH"
            ;;
    esac
fi

cp bin/neuron "$DIST_DIR/$BINARY_NAME"

echo "✅ Static binary created: dist/$BINARY_NAME"

# Show binary info
echo "📊 Binary information:"
file "$DIST_DIR/$BINARY_NAME"
ls -lh "$DIST_DIR/$BINARY_NAME"

# Check dependencies (macOS/Linux specific)
if command -v otool >/dev/null 2>&1; then
    echo "🔍 macOS Dependencies:"
    otool -L "$DIST_DIR/$BINARY_NAME" | grep -v "$BINARY_NAME:"
elif command -v ldd >/dev/null 2>&1; then
    echo "🔍 Linux Dependencies:"
    ldd "$DIST_DIR/$BINARY_NAME" 2>/dev/null || echo "Static binary (no dynamic dependencies)"
fi

echo "🎉 Build complete!"