#!/usr/bin/env bash
# Void OS Elite - Unified installer (auto-detects OS)
# Run from repo root: ./install.sh

set -e

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
VOID_DIR="$REPO_ROOT/void"

# Detect OS
detect_os() {
    local os
    case "$(uname -s 2>/dev/null)" in
        Linux*)   os=linux ;;
        Darwin*)  os=macos ;;
        *)        os=unknown ;;
    esac
    echo "$os"
}

OS="$(detect_os)"

echo "=========================================="
echo "  Void OS Elite - Installer"
echo "=========================================="
echo ""
echo "Detected OS: $OS"
echo ""

case "$OS" in
    linux)
        if [ -f "$REPO_ROOT/install-linux.sh" ]; then
            exec bash "$REPO_ROOT/install-linux.sh" "$@"
        else
            echo "Running built-in Linux install..."
            cd "$VOID_DIR"
            make clean 2>/dev/null || true
            make
            echo ""
            read -p "Install to /usr/local/bin? (requires sudo) [y/N] " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                sudo make install
            fi
            echo ""
            echo "Done. Run ./void from $VOID_DIR or add it to your PATH."
        fi
        ;;
    macos)
        if [ -f "$REPO_ROOT/install-macos.sh" ]; then
            exec bash "$REPO_ROOT/install-macos.sh" "$@"
        else
            echo "Running built-in macOS install..."
            cd "$VOID_DIR"
            make clean 2>/dev/null || true
            make
            echo ""
            read -p "Install to /usr/local/bin? (requires sudo) [y/N] " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                sudo make install
            fi
            echo ""
            echo "Done. Run ./void from $VOID_DIR or add it to your PATH."
        fi
        ;;
    *)
        echo "Unsupported OS for this installer."
        echo ""
        echo "  Linux:   Run ./install-linux.sh or use WSL on Windows"
        echo "  macOS:   Run ./install-macos.sh"
        echo "  Windows: Use WSL and run ./install-linux.sh, or run install-windows.ps1"
        echo ""
        echo "See INSTALL.md for details."
        exit 1
        ;;
esac
