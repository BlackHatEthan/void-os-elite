#!/bin/bash
# Create Minimal Linux + Void OS Bootable USB
# Uses Alpine Linux as base (recommended for small size)

set -e

USB_DEVICE="${1}"
ALPINE_VERSION="3.19"
ARCH="x86_64"

if [ -z "$USB_DEVICE" ]; then
    echo "Usage: $0 <usb_device>"
    echo "Example: $0 /dev/sdb"
    echo ""
    echo "Available USB devices:"
    lsblk 2>/dev/null | grep -E "disk|part" || diskutil list | grep -E "disk|external"
    exit 1
fi

VOID_DIR="$(cd "$(dirname "$0")/.." && pwd)"

echo "=========================================="
echo "  Void OS Elite - Minimal Linux USB"
echo "=========================================="
echo ""
echo "This will:"
echo "  1. Download Alpine Linux"
echo "  2. Install to USB device: $USB_DEVICE"
echo "  3. Configure Void OS as default shell"
echo ""
echo "WARNING: This will format $USB_DEVICE!"
read -p "Continue? (yes/no): " confirm

if [ "$confirm" != "yes" ]; then
    echo "Aborted."
    exit 1
fi

echo ""
echo "Step 1: Downloading Alpine Linux..."
ALPINE_ISO="alpine-standard-${ALPINE_VERSION}-${ARCH}.iso"
if [ ! -f "/tmp/$ALPINE_ISO" ]; then
    echo "Downloading Alpine Linux (this may take a while)..."
    curl -L "https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION}/releases/${ARCH}/${ALPINE_ISO}" \
         -o "/tmp/$ALPINE_ISO" || {
        echo "Failed to download Alpine Linux"
        exit 1
    }
fi

echo "Step 2: Writing Alpine Linux to USB..."
sudo dd if="/tmp/$ALPINE_ISO" of="$USB_DEVICE" bs=4M status=progress oflag=sync

echo "Step 3: Waiting for USB to be ready..."
sleep 5

# Find the USB partition
USB_PARTITION=$(lsblk -ln -o NAME,TYPE "$USB_DEVICE" | grep part | head -1 | awk '{print $1}')
if [ -z "$USB_PARTITION" ]; then
    USB_PARTITION="${USB_DEVICE}1"
fi

MOUNT_POINT="/mnt/void_usb"
sudo mkdir -p "$MOUNT_POINT"

echo "Step 4: Mounting USB..."
sudo mount "/dev/$USB_PARTITION" "$MOUNT_POINT" 2>/dev/null || {
    # Try different mount point
    MOUNT_POINT="/media/void_usb"
    sudo mkdir -p "$MOUNT_POINT"
    sudo mount "/dev/$USB_PARTITION" "$MOUNT_POINT"
}

echo "Step 5: Building Void OS..."
cd "$VOID_DIR"
make clean
make

echo "Step 6: Installing Void OS..."
sudo cp "$VOID_DIR/void" "$MOUNT_POINT/usr/local/bin/void"
sudo chmod +x "$MOUNT_POINT/usr/local/bin/void"

# Create symlink for default shell
sudo ln -sf /usr/local/bin/void "$MOUNT_POINT/bin/void-sh"

# Modify /etc/passwd to use void as shell for root
sudo sed -i 's|root:x:0:0:root:/root:/bin/ash|root:x:0:0:root:/root:/usr/local/bin/void|' \
    "$MOUNT_POINT/etc/passwd" 2>/dev/null || {
    echo "Note: Could not modify /etc/passwd. You may need to do this manually after boot."
}

# Create .voidrc
sudo mkdir -p "$MOUNT_POINT/root"
sudo tee "$MOUNT_POINT/root/.voidrc" > /dev/null << 'EOF'
# Void OS Elite Configuration
export VOID_THEME=noir
EOF

echo "Step 7: Unmounting USB..."
sudo umount "$MOUNT_POINT"
sudo rmdir "$MOUNT_POINT"

echo ""
echo "=========================================="
echo "  Bootable USB Created!"
echo "=========================================="
echo ""
echo "USB device: $USB_DEVICE"
echo ""
echo "To boot:"
echo "  1. Insert USB into target machine"
echo "  2. Boot from USB (may need to change BIOS/UEFI settings)"
echo "  3. Void OS Elite will start automatically"
echo ""
echo "Default login: root (no password initially)"
echo "Void OS will be the default shell."

