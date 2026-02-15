#!/bin/bash
# Create Bootable USB with Void OS Elite
# This script creates a minimal Linux system with Void OS as the default shell

set -e

USB_DEVICE="${1:-/dev/disk2}"  # Default to /dev/disk2, user can override
MOUNT_POINT="/tmp/void_usb"
VOID_DIR="$(cd "$(dirname "$0")/.." && pwd)"

echo "=========================================="
echo "  Void OS Elite - Bootable USB Creator"
echo "=========================================="
echo ""
echo "WARNING: This will format the USB device at $USB_DEVICE"
echo "All data on the USB will be lost!"
echo ""
read -p "Continue? (yes/no): " confirm

if [ "$confirm" != "yes" ]; then
    echo "Aborted."
    exit 1
fi

echo ""
echo "Step 1: Creating partitions..."
# Create GPT partition table and single partition
sudo diskutil eraseDisk GPT "VoidOS" "$USB_DEVICE" || {
    echo "Failed to format USB. Make sure device is correct."
    exit 1
}

echo "Step 2: Mounting USB..."
sudo mkdir -p "$MOUNT_POINT"
USB_PARTITION="${USB_DEVICE}s1"
sudo mount -t msdos "$USB_PARTITION" "$MOUNT_POINT" 2>/dev/null || {
    # Try different filesystem
    sudo mount -t exfat "$USB_PARTITION" "$MOUNT_POINT" 2>/dev/null || {
        echo "Failed to mount USB. Trying alternative method..."
        diskutil mount "$USB_PARTITION"
        MOUNT_POINT="/Volumes/VoidOS"
    }
}

echo "Step 3: Creating minimal Linux structure..."
sudo mkdir -p "$MOUNT_POINT"/{bin,boot,dev,etc,home,lib,lib64,mnt,opt,proc,root,run,sbin,sys,tmp,usr,var}

echo "Step 4: Copying Void OS..."
sudo cp "$VOID_DIR/void" "$MOUNT_POINT/bin/void"
sudo chmod +x "$MOUNT_POINT/bin/void"

# Create symlink so void can be used as /bin/sh
sudo ln -sf /bin/void "$MOUNT_POINT/bin/sh"

echo "Step 5: Creating init script..."
sudo tee "$MOUNT_POINT/init" > /dev/null << 'EOF'
#!/bin/sh
# Minimal init script for Void OS

mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t devtmpfs devtmpfs /dev

# Set up basic devices
mknod /dev/console c 5 1
mknod /dev/null c 1 3

# Start Void OS
exec /bin/void
EOF

sudo chmod +x "$MOUNT_POINT/init"

echo "Step 6: Creating GRUB configuration..."
sudo mkdir -p "$MOUNT_POINT/boot/grub"
sudo tee "$MOUNT_POINT/boot/grub/grub.cfg" > /dev/null << 'EOF'
set timeout=5
set default=0

menuentry "Void OS Elite" {
    linux /boot/vmlinuz root=/dev/sda1 init=/init
    initrd /boot/initrd.img
}
EOF

echo ""
echo "=========================================="
echo "  Bootable USB Creation Complete!"
echo "=========================================="
echo ""
echo "Note: This creates a basic structure."
echo "For a fully bootable USB, you need:"
echo "  1. Linux kernel (vmlinuz)"
echo "  2. Initramfs (initrd.img)"
echo "  3. GRUB bootloader"
echo ""
echo "Consider using a minimal Linux distribution like:"
echo "  - Alpine Linux (recommended - very small)"
echo "  - Tiny Core Linux"
echo "  - Buildroot"
echo ""
echo "Then replace the default shell with Void OS."

