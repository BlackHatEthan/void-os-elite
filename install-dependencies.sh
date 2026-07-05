#!/bin/bash
# install-dependencies.sh
# Universal dependency installer - detects OS and runs appropriate script

set -e

GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RESET='\033[0m'

echo "=========================================="
echo "  Void OS Elite - Dependency Installer"
echo "=========================================="
echo ""

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo -e "${BLUE}Detected: Linux${RESET}"
    echo ""
    bash install-dependencies-linux.sh
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo -e "${BLUE}Detected: macOS${RESET}"
    echo ""
    bash install-dependencies-macos.sh
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    echo -e "${BLUE}Detected: Windows (Cygwin/MSYS)${RESET}"
    echo ""
    echo -e "${YELLOW}Note: For Windows, please use WSL or run install-dependencies-windows.ps1${RESET}"
    echo ""
    bash install-dependencies-linux.sh
else
    echo -e "${YELLOW}Unknown OS: $OSTYPE${RESET}"
    echo ""
    echo "Please run the appropriate installer manually:"
    echo "  Linux:   bash install-dependencies-linux.sh"
    echo "  macOS:   bash install-dependencies-macos.sh"
    echo "  Windows: powershell install-dependencies-windows.ps1"
    exit 1
fi
