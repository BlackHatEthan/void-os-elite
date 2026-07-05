#!/usr/bin/env bash
# Void OS Elite - Linux installer
# Run from repo root: ./install-linux.sh
# Or: bash install-linux.sh

set -e

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
VOID_DIR="$REPO_ROOT/void"

echo "=========================================="
echo "  Void OS Elite - Linux Install"
echo "=========================================="
echo ""

# Check for build tools
if ! command -v gcc >/dev/null 2>&1 && ! command -v cc >/dev/null 2>&1; then
    echo "GCC/CC not found. Install build-essential:"
    echo "  Debian/Ubuntu: sudo apt update && sudo apt install build-essential"
    echo "  Fedora:        sudo dnf install gcc make"
    echo "  Arch:          sudo pacman -S base-devel"
    exit 1
fi

if [ ! -f "$VOID_DIR/Makefile" ]; then
    echo "Error: Void OS source not found at $VOID_DIR"
    echo "Run this script from the voidos-elite repo root."
    exit 1
fi

cd "$VOID_DIR"
echo "Building Void OS Elite..."
make clean 2>/dev/null || true
make

echo ""
echo "Build complete: $VOID_DIR/void"
echo ""

# Optional system install
INSTALL_SYSTEM="${INSTALL_SYSTEM:-}"
if [ "$1" = "--install" ] || [ "$1" = "-i" ] || [ -n "$INSTALL_SYSTEM" ]; then
    echo "Installing to /usr/local/bin (requires sudo)..."
    sudo cp void /usr/local/bin/void
    sudo chmod +x /usr/local/bin/void
    echo "Done. Run 'void' from anywhere."
else
    read -p "Install to /usr/local/bin? (requires sudo) [y/N] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        sudo cp void /usr/local/bin/void
        sudo chmod +x /usr/local/bin/void
        echo "Done. Run 'void' from anywhere."
    else
        echo "Skipped. Run ./void from $VOID_DIR or add $VOID_DIR to your PATH."
    fi
fi

echo ""
echo "Start Void OS:  void"
echo "Get help:       void-help"
