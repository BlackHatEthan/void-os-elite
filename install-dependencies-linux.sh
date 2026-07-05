#!/bin/bash
# install-dependencies-linux.sh
# Installs all dependencies needed to build and run Void OS Elite on Linux

set -e

GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
RESET='\033[0m'

echo "=========================================="
echo "  Void OS Elite - Linux Dependencies"
echo "=========================================="
echo ""

# Detect package manager
if command -v apt-get >/dev/null 2>&1; then
    PKG_MGR="apt"
    UPDATE_CMD="sudo apt-get update"
    INSTALL_CMD="sudo apt-get install -y"
elif command -v dnf >/dev/null 2>&1; then
    PKG_MGR="dnf"
    UPDATE_CMD="sudo dnf check-update || true"
    INSTALL_CMD="sudo dnf install -y"
elif command -v yum >/dev/null 2>&1; then
    PKG_MGR="yum"
    UPDATE_CMD="sudo yum check-update || true"
    INSTALL_CMD="sudo yum install -y"
elif command -v pacman >/dev/null 2>&1; then
    PKG_MGR="pacman"
    UPDATE_CMD="sudo pacman -Sy"
    INSTALL_CMD="sudo pacman -S --noconfirm"
elif command -v zypper >/dev/null 2>&1; then
    PKG_MGR="zypper"
    UPDATE_CMD="sudo zypper refresh"
    INSTALL_CMD="sudo zypper install -y"
else
    echo -e "${RED}Error: Could not detect package manager.${RESET}"
    echo "Supported: apt, dnf, yum, pacman, zypper"
    exit 1
fi

echo -e "${BLUE}Detected package manager: ${PKG_MGR}${RESET}"
echo ""

# Update package lists
echo -e "${BLUE}Updating package lists...${RESET}"
$UPDATE_CMD || true
echo ""

# Core build dependencies
echo -e "${BLUE}Installing core build dependencies...${RESET}"
if [ "$PKG_MGR" = "apt" ]; then
    $INSTALL_CMD \
        build-essential \
        gcc \
        g++ \
        make \
        libc6-dev \
        git \
        wget \
        curl \
        tar \
        gzip
elif [ "$PKG_MGR" = "dnf" ] || [ "$PKG_MGR" = "yum" ]; then
    $INSTALL_CMD \
        gcc \
        gcc-c++ \
        make \
        glibc-devel \
        git \
        wget \
        curl \
        tar \
        gzip
elif [ "$PKG_MGR" = "pacman" ]; then
    $INSTALL_CMD \
        base-devel \
        gcc \
        make \
        git \
        wget \
        curl \
        tar \
        gzip
elif [ "$PKG_MGR" = "zypper" ]; then
    $INSTALL_CMD \
        gcc \
        gcc-c++ \
        make \
        glibc-devel \
        git \
        wget \
        curl \
        tar \
        gzip
fi

echo ""

# ISO creation dependencies
echo -e "${BLUE}Installing ISO creation dependencies...${RESET}"
if [ "$PKG_MGR" = "apt" ]; then
    $INSTALL_CMD \
        xorriso \
        genisoimage \
        grub-pc-bin \
        grub-efi-amd64-bin \
        grub-efi-ia32-bin \
        syslinux \
        isohybrid \
        p7zip-full \
        p7zip-rar
elif [ "$PKG_MGR" = "dnf" ]; then
    $INSTALL_CMD \
        xorriso \
        genisoimage \
        grub2-efi-x64 \
        grub2-pc \
        syslinux \
        isohybrid \
        p7zip \
        p7zip-plugins
elif [ "$PKG_MGR" = "yum" ]; then
    $INSTALL_CMD \
        xorriso \
        genisoimage \
        grub2-efi-x64 \
        grub2-pc \
        syslinux \
        p7zip \
        p7zip-plugins
elif [ "$PKG_MGR" = "pacman" ]; then
    $INSTALL_CMD \
        xorriso \
        cdrkit \
        grub \
        syslinux \
        p7zip
elif [ "$PKG_MGR" = "zypper" ]; then
    $INSTALL_CMD \
        xorriso \
        genisoimage \
        grub2 \
        syslinux \
        p7zip
fi

echo ""

# Optional but recommended dependencies
echo -e "${BLUE}Installing optional dependencies...${RESET}"
if [ "$PKG_MGR" = "apt" ]; then
    $INSTALL_CMD \
        pkg-config \
        libssl-dev \
        zlib1g-dev \
        libreadline-dev \
        libncurses-dev
elif [ "$PKG_MGR" = "dnf" ] || [ "$PKG_MGR" = "yum" ]; then
    $INSTALL_CMD \
        pkgconfig \
        openssl-devel \
        zlib-devel \
        readline-devel \
        ncurses-devel
elif [ "$PKG_MGR" = "pacman" ]; then
    $INSTALL_CMD \
        pkgconf \
        openssl \
        zlib \
        readline \
        ncurses
elif [ "$PKG_MGR" = "zypper" ]; then
    $INSTALL_CMD \
        pkg-config \
        libopenssl-devel \
        zlib-devel \
        readline-devel \
        ncurses-devel
fi

echo ""
echo -e "${GREEN}=========================================="
echo "  Installation Complete!"
echo "==========================================${RESET}"
echo ""
echo "Installed packages:"
echo "  ✓ Build tools (gcc, make, etc.)"
echo "  ✓ ISO creation tools (xorriso, grub, etc.)"
echo "  ✓ Archive tools (tar, gzip, p7zip)"
echo "  ✓ Network tools (wget, curl)"
echo "  ✓ Version control (git)"
echo ""
echo "You can now:"
echo "  1. Build Void OS Elite: cd void && make"
echo "  2. Create ISO: cd void/scripts && sudo bash create_void_iso.sh"
echo ""
