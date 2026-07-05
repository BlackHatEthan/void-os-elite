#!/usr/bin/env bash
# Void OS Elite - macOS installer
# Run from repo root: ./install-macos.sh
# Or: bash install-macos.sh

set -e

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
VOID_DIR="$REPO_ROOT/void"

echo "=========================================="
echo "  Void OS Elite - macOS Install"
echo "=========================================="
echo ""

# Check for Xcode Command Line Tools (provides clang/gcc)
if ! command -v clang >/dev/null 2>&1 && ! command -v gcc >/dev/null 2>&1; then
    echo "No C compiler found. Install Xcode Command Line Tools:"
    echo "  xcode-select --install"
    echo ""
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
if [ "$1" = "--install" ] || [ "$1" = "-i" ]; then
    echo "Installing to /usr/local/bin (requires sudo)..."
    sudo mkdir -p /usr/local/bin
    sudo cp void /usr/local/bin/void
    sudo chmod +x /usr/local/bin/void
    echo "Done. Run 'void' from anywhere."
else
    read -p "Install to /usr/local/bin? (requires sudo) [y/N] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        sudo mkdir -p /usr/local/bin
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
