#!/bin/bash
# create_void_iso.sh - Automated ISO creation script for Void OS Elite
# Creates a bootable ISO with Alpine Linux base and Void OS as default shell

set -e

ISO_NAME="void-os-elite"
WORK_DIR="${HOME}/void_iso"
VOID_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ALPINE_VERSION="3.19"
ARCH="x86_64"

echo "=========================================="
echo "  Void OS Elite - ISO Creator"
echo "=========================================="
echo ""

# Check dependencies
command -v xorriso >/dev/null 2>&1 || {
    echo "Error: xorriso not found."
    echo "Install it: sudo apt install xorriso"
    exit 1
}

command -v grub-mkstandalone >/dev/null 2>&1 || {
    echo "Warning: grub-mkstandalone not found. UEFI support may be limited."
    echo "Install it: sudo apt install grub-pc-bin grub-efi-amd64-bin"
fi

# Create workspace
echo "Creating workspace..."
mkdir -p "$WORK_DIR/iso"
cd "$WORK_DIR"

# Download Alpine Linux mini rootfs
ALPINE_ROOTFS="alpine-minirootfs-${ALPINE_VERSION}.0-${ARCH}.tar.gz"
if [ ! -f "$ALPINE_ROOTFS" ]; then
    echo "Downloading Alpine Linux mini rootfs..."
    wget "https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION}/releases/${ARCH}/${ALPINE_ROOTFS}" || {
        echo "Failed to download Alpine Linux"
        exit 1
    }
fi

# Extract Alpine
echo "Extracting Alpine Linux..."
sudo rm -rf iso/*
sudo tar -xzf "$ALPINE_ROOTFS" -C iso/

# Build Void OS
echo "Building Void OS..."
cd "$VOID_DIR"
if [ ! -f void ]; then
    make clean
    make static
fi

# Copy Void OS
echo "Copying Void OS to ISO..."
sudo cp void "$WORK_DIR/iso/bin/void"
sudo chmod +x "$WORK_DIR/iso/bin/void"

# Configure system
cd "$WORK_DIR"
echo "Configuring system..."

# Create passwd with void as shell
sudo tee iso/etc/passwd > /dev/null << 'EOF'
root:x:0:0:root:/root:/bin/void
EOF

# Create shadow (empty password for root)
sudo tee iso/etc/shadow > /dev/null << 'EOF'
root::0:0:99999:7:::
EOF

# Create group
sudo tee iso/etc/group > /dev/null << 'EOF'
root:x:0:
EOF

# Create init script
echo "Creating init script..."
sudo tee iso/init > /dev/null << 'EOF'
#!/bin/sh
# Void OS Elite Init Script

# Mount essential filesystems
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t devtmpfs devtmpfs /dev

# Create essential device nodes
mknod /dev/console c 5 1
mknod /dev/null c 1 3
mknod /dev/zero c 1 5
mknod /dev/tty c 5 0

# Set up networking (basic)
ifconfig lo 127.0.0.1 up 2>/dev/null || true

# Start Void OS
exec /bin/void
EOF
sudo chmod +x iso/init

# Download kernel and initramfs
echo "Downloading kernel and initramfs..."
KERNEL="vmlinuz-lts"
INITRAMFS="initramfs-lts"

if [ ! -f "$KERNEL" ]; then
    wget "https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION}/releases/${ARCH}/boot/${KERNEL}" || {
        echo "Failed to download kernel"
        exit 1
    }
fi

if [ ! -f "$INITRAMFS" ]; then
    wget "https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION}/releases/${ARCH}/boot/${INITRAMFS}" || {
        echo "Failed to download initramfs"
        exit 1
    }
fi

sudo mkdir -p iso/boot
sudo cp "$KERNEL" iso/boot/
sudo cp "$INITRAMFS" iso/boot/

# Create GRUB config
echo "Creating GRUB configuration..."
sudo mkdir -p iso/boot/grub
sudo tee iso/boot/grub/grub.cfg > /dev/null << 'EOF'
set timeout=5
set default=0

menuentry "Void OS Elite" {
    linux /boot/vmlinuz-lts root=/dev/loop0 init=/init
    initrd /boot/initramfs-lts
}

menuentry "Void OS Elite (Safe Mode)" {
    linux /boot/vmlinuz-lts root=/dev/loop0 init=/init single
    initrd /boot/initramfs-lts
}
EOF

# Create ISO
echo "Creating ISO file..."
ISO_FILE="${ISO_NAME}.iso"
sudo xorriso -as mkisofs \
    -R -J \
    -V "Void OS Elite" \
    -o "$ISO_FILE" \
    -b isolinux/isolinux.bin \
    -c isolinux/boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    iso/ 2>&1 | grep -v "xorriso: UPDATE:" || true

# Make ISO bootable on USB (hybrid)
if command -v isohybrid >/dev/null 2>&1; then
    echo "Making ISO bootable on USB..."
    sudo isohybrid "$ISO_FILE" 2>/dev/null || true
fi

# Set ownership
sudo chown "$USER:$USER" "$ISO_FILE"

echo ""
echo "=========================================="
echo "  ISO Creation Complete!"
echo "=========================================="
echo ""
echo "ISO file: ${WORK_DIR}/${ISO_FILE}"
echo "Size: $(du -h "$ISO_FILE" | cut -f1)"
echo ""
echo "To write to USB:"
echo "  sudo dd if=${ISO_FILE} of=/dev/sdX bs=4M status=progress oflag=sync"
echo ""
echo "To use in VM (QEMU):"
echo "  qemu-system-x86_64 -cdrom ${ISO_FILE} -m 512M"
echo ""
echo "To use in VirtualBox:"
echo "  VBoxManage storageattach \"VM Name\" --storagectl \"IDE\" --port 0 --device 0 --type dvddrive --medium ${ISO_FILE}"
echo ""

