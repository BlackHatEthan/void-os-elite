#!/bin/bash
# install-dependencies-macos.sh
# Installs all dependencies needed to build and run Void OS Elite on macOS

set -e

GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
RESET='\033[0m'

echo "=========================================="
echo "  Void OS Elite - macOS Dependencies"
echo "=========================================="
echo ""

# Check for Homebrew
if ! command -v brew >/dev/null 2>&1; then
    echo -e "${YELLOW}Homebrew not found. Installing Homebrew...${RESET}"
    echo ""
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    
    # Add Homebrew to PATH for Apple Silicon Macs
    if [ -f "/opt/homebrew/bin/brew" ]; then
        echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
        eval "$(/opt/homebrew/bin/brew shellenv)"
    fi
    
    echo ""
    echo -e "${GREEN}Homebrew installed successfully!${RESET}"
    echo ""
fi

# Check for Xcode Command Line Tools
echo -e "${BLUE}Checking for Xcode Command Line Tools...${RESET}"
if ! xcode-select -p >/dev/null 2>&1; then
    echo -e "${YELLOW}Xcode Command Line Tools not found. Installing...${RESET}"
    xcode-select --install
    echo ""
    echo -e "${YELLOW}Please complete the Xcode Command Line Tools installation, then run this script again.${RESET}"
    exit 1
else
    echo -e "${GREEN}✓ Xcode Command Line Tools found${RESET}"
fi

echo ""

# Update Homebrew
echo -e "${BLUE}Updating Homebrew...${RESET}"
brew update
echo ""

# Core build dependencies
echo -e "${BLUE}Installing core build dependencies...${RESET}"
brew install \
    gcc \
    make \
    git \
    wget \
    curl \
    gzip \
    tar

echo ""

# ISO creation dependencies
echo -e "${BLUE}Installing ISO creation dependencies...${RESET}"
# Note: GRUB on macOS is complex, so we install what's available
brew install \
    xorriso \
    cdrtools \
    p7zip

# GRUB installation (optional, complex on macOS)
echo ""
echo -e "${YELLOW}Note: GRUB installation on macOS is complex and may require manual setup.${RESET}"
echo -e "${YELLOW}For creating bootable ISOs, consider using a Linux VM or WSL.${RESET}"
echo ""
read -p "Do you want to attempt GRUB installation? (y/N): " -n 1 -r
echo ""
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo -e "${BLUE}Installing GRUB (this may take a while)...${RESET}"
    brew install grub || {
        echo -e "${YELLOW}GRUB installation failed. This is expected on macOS.${RESET}"
        echo -e "${YELLOW}You can still create ISO files, but they may not be bootable without GRUB.${RESET}"
    }
fi

echo ""

# Optional but recommended dependencies
echo -e "${BLUE}Installing optional dependencies...${RESET}"
brew install \
    pkg-config \
    openssl \
    zlib \
    readline \
    ncurses

echo ""
echo -e "${GREEN}=========================================="
echo "  Installation Complete!"
echo "==========================================${RESET}"
echo ""
echo "Installed packages:"
echo "  ✓ Build tools (gcc, make via Xcode CLT)"
echo "  ✓ ISO creation tools (xorriso, cdrtools, p7zip)"
echo "  ✓ Archive tools (tar, gzip)"
echo "  ✓ Network tools (wget, curl)"
echo "  ✓ Version control (git)"
echo ""
echo -e "${YELLOW}Note:${RESET} Creating bootable ISOs on macOS is limited."
echo "For full ISO creation support, use Linux or WSL."
echo ""
echo "You can now:"
echo "  1. Build Void OS Elite: cd void && make"
echo "  2. Create ISO (basic): cd void/scripts && sudo bash create_void_iso.sh"
echo ""
