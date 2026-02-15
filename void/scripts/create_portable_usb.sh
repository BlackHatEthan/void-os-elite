#!/bin/bash
# Create Portable Void OS USB
# This creates a USB that can run Void OS on any Linux system

set -e

USB_MOUNT="${1:-/Volumes/USB}"  # Default mount point, user can override
VOID_DIR="$(cd "$(dirname "$0")/.." && pwd)"

echo "=========================================="
echo "  Void OS Elite - Portable USB Creator"
echo "=========================================="
echo ""

# Build static binary
echo "Step 1: Building static binary..."
cd "$VOID_DIR"
make clean
make static

if [ ! -f "$VOID_DIR/void" ]; then
    echo "Error: Failed to build Void OS"
    exit 1
fi

# Create portable directory structure
echo "Step 2: Creating portable structure..."
PORTABLE_DIR="$USB_MOUNT/void_os"
mkdir -p "$PORTABLE_DIR"

# Copy binary
cp "$VOID_DIR/void" "$PORTABLE_DIR/void"
chmod +x "$PORTABLE_DIR/void"

# Create run script
cat > "$PORTABLE_DIR/run.sh" << 'EOF'
#!/bin/bash
# Portable Void OS Elite Launcher

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# Check if we're on Linux
if [ "$(uname)" != "Linux" ]; then
    echo "Void OS Elite requires Linux to run."
    echo "Current OS: $(uname)"
    exit 1
fi

# Run Void OS
exec ./void "$@"
EOF

chmod +x "$PORTABLE_DIR/run.sh"

# Create README
cat > "$PORTABLE_DIR/README.txt" << 'EOF'
Void OS Elite - Portable Installation
=====================================

To run Void OS Elite on any Linux system:

1. Mount this USB drive
2. Navigate to the void_os directory
3. Run: ./run.sh

Or add to your PATH:
  export PATH=$PATH:/path/to/usb/void_os
  void

Requirements:
- Linux operating system
- POSIX-compliant environment
- No additional dependencies needed (static binary)

For more information, see the main README.md
EOF

echo ""
echo "=========================================="
echo "  Portable USB Created!"
echo "=========================================="
echo ""
echo "Location: $PORTABLE_DIR"
echo ""
echo "To use:"
echo "  1. Mount USB on any Linux system"
echo "  2. cd $PORTABLE_DIR"
echo "  3. ./run.sh"
echo ""
echo "Or copy the void_os directory to any Linux system."

