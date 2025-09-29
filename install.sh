#!/bin/bash

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

# Configuration
REPO="dhruvin3001/neuron"
INSTALL_DIR="/usr/local/bin"
VERSION="latest"

print_header() {
    echo -e "${PURPLE}🧬 Neuron AI Installer${NC}"
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
}

detect_platform() {
    OS=$(uname -s)
    ARCH=$(uname -m)
    
    case "$OS" in
        "Darwin")
            PLATFORM="macos"
            ;;
        "Linux")
            PLATFORM="linux"
            ;;
        *)
            echo -e "${RED}❌ Unsupported OS: $OS${NC}"
            exit 1
            ;;
    esac
    
    case "$ARCH" in
        "x86_64"|"amd64")
            ARCH="x86_64"
            ;;
        "arm64"|"aarch64")
            ARCH="arm64"
            ;;
        *)
            echo -e "${RED}❌ Unsupported architecture: $ARCH${NC}"
            exit 1
            ;;
    esac
    
    BINARY_NAME="neuron-${PLATFORM}-${ARCH}"
    echo -e "${GREEN}✅ Detected platform: ${PLATFORM}-${ARCH}${NC}"
}

check_dependencies() {
    echo -e "${BLUE}🔍 Checking dependencies...${NC}"
    
    # Check if curl is available
    if ! command -v curl &> /dev/null; then
        echo -e "${RED}❌ curl is required but not installed${NC}"
        exit 1
    fi
    
    # Check write permissions
    if [ ! -w "$(dirname "$INSTALL_DIR")" ]; then
        echo -e "${YELLOW}⚠️  Need sudo for installation to $INSTALL_DIR${NC}"
        NEED_SUDO=true
    fi
}

download_binary() {
    echo -e "${BLUE}📥 Downloading Neuron...${NC}"
    
    if [ "$VERSION" = "latest" ]; then
        DOWNLOAD_URL="https://github.com/${REPO}/releases/latest/download/${BINARY_NAME}"
    else
        DOWNLOAD_URL="https://github.com/${REPO}/releases/download/v${VERSION}/${BINARY_NAME}"
    fi
    
    echo "Downloading from: $DOWNLOAD_URL"
    
    # Create temp file
    TEMP_FILE=$(mktemp)
    
    # Download with progress bar
    if curl -L --fail --progress-bar --output "$TEMP_FILE" "$DOWNLOAD_URL"; then
        echo -e "${GREEN}✅ Download completed${NC}"
    else
        echo -e "${RED}❌ Download failed${NC}"
        echo -e "${YELLOW}💡 Make sure the release exists at: https://github.com/${REPO}/releases${NC}"
        rm -f "$TEMP_FILE"
        exit 1
    fi
    
    # Make executable
    chmod +x "$TEMP_FILE"
    
    # Install binary
    if [ "$NEED_SUDO" = true ]; then
        echo -e "${YELLOW}🔐 Installing with sudo...${NC}"
        sudo mv "$TEMP_FILE" "$INSTALL_DIR/neuron"
    else
        mv "$TEMP_FILE" "$INSTALL_DIR/neuron"
    fi
}

verify_installation() {
    echo -e "${BLUE}🧪 Verifying installation...${NC}"
    
    if command -v neuron &> /dev/null; then
        echo -e "${GREEN}✅ Neuron installed successfully!${NC}"
        echo -e "${GREEN}📍 Location: $(which neuron)${NC}"
        
        # Test version
        echo -e "${BLUE}🔖 Version check:${NC}"
        neuron --help | head -5
    else
        echo -e "${RED}❌ Installation verification failed${NC}"
        echo -e "${YELLOW}💡 You may need to add $INSTALL_DIR to your PATH${NC}"
        exit 1
    fi
}

show_next_steps() {
    echo -e "${BLUE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${GREEN}🎉 Installation Complete!${NC}"
    echo ""
    echo -e "${BLUE}📖 Next steps:${NC}"
    echo -e "1. Set your API key:"
    echo -e "   ${YELLOW}export NEURON_API_KEY=your_key_here${NC}"
    echo ""
    echo -e "2. Try it out:"
    echo -e "   ${YELLOW}neuron run \"show disk usage\"${NC}"
    echo -e "   ${YELLOW}neuron tell \"what is docker\"${NC}"
    echo ""
    echo -e "3. Get your API key from:"
    echo -e "   ${BLUE}• GitHub Models: https://github.com/marketplace/models${NC}"
    echo -e "   ${BLUE}• OpenAI: https://platform.openai.com/api-keys${NC}"
    echo ""
    echo -e "${PURPLE}🧬 Happy coding with Neuron AI!${NC}"
}

# Main execution
main() {
    print_header
    detect_platform
    check_dependencies
    download_binary
    verify_installation
    show_next_steps
}

# Handle command line arguments
case "${1:-}" in
    --version=*)
        VERSION="${1#--version=}"
        ;;
    --install-dir=*)
        INSTALL_DIR="${1#--install-dir=}"
        ;;
    --help|-h)
        echo "Neuron AI Installer"
        echo ""
        echo "Usage: $0 [options]"
        echo ""
        echo "Options:"
        echo "  --version=VERSION      Install specific version (default: latest)"
        echo "  --install-dir=DIR      Install to specific directory (default: /usr/local/bin)"
        echo "  --help, -h            Show this help message"
        echo ""
        echo "Examples:"
        echo "  $0                              # Install latest version"
        echo "  $0 --version=1.0.0             # Install version 1.0.0"
        echo "  $0 --install-dir=\$HOME/.local/bin  # Install to user directory"
        exit 0
        ;;
esac

main