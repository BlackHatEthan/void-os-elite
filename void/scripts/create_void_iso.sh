#!/bin/bash
# create_void_iso.sh - Automated ISO creation script for Void OS Elite
# Creates a bootable ISO with Alpine Linux base and Void OS as default shell

set -e

ISO_NAME="void-os-elite"
WORK_DIR="${HOME}/void_iso"
VOID_DIR="$(cd "$(dirname "$0")/.." && pwd)"
ALPINE_VERSION="3.19"

# Detect architecture or allow override
if [ -z "$ARCH" ]; then
    # Detect system architecture
    SYS_ARCH=$(uname -m)
    case "$SYS_ARCH" in
        x86_64|amd64)
            ARCH="x86_64"
            ;;
        aarch64|arm64)
            ARCH="aarch64"
            ;;
        *)
            echo "Warning: Unknown architecture $SYS_ARCH, defaulting to x86_64"
            ARCH="x86_64"
            ;;
    esac
else
    # Normalize user-provided architecture
    case "$ARCH" in
        arm64)
            ARCH="aarch64"
            ;;
        amd64)
            ARCH="x86_64"
            ;;
    esac
fi

# Allow user to override architecture for VM compatibility
echo "Architecture: $ARCH"
if [ "$ARCH" = "aarch64" ]; then
    echo "✓ Building for ARM64 (Apple Silicon / ARM VMs)"
else
    echo "✓ Building for x86_64 (Intel/AMD VMs)"
fi
echo ""
echo "Note: If your VM uses a different architecture, set ARCH environment variable:"
echo "  ARCH=aarch64 bash create_void_iso.sh  # For ARM64 VMs (UTM on Apple Silicon)"
echo "  ARCH=x86_64 bash create_void_iso.sh   # For x86_64 VMs"
echo ""

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
}

# Create workspace
echo "Creating workspace..."
mkdir -p "$WORK_DIR/iso"
cd "$WORK_DIR"

# Download Alpine Linux mini rootfs
# Alpine uses 'aarch64' for ARM64, but we need to map it correctly
ALPINE_ARCH="$ARCH"
if [ "$ARCH" = "arm64" ]; then
    ALPINE_ARCH="aarch64"
fi

ALPINE_ROOTFS="alpine-minirootfs-${ALPINE_VERSION}.0-${ALPINE_ARCH}.tar.gz"
if [ ! -f "$ALPINE_ROOTFS" ]; then
    echo "Downloading Alpine Linux mini rootfs for ${ALPINE_ARCH}..."
    wget "https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION}/releases/${ALPINE_ARCH}/${ALPINE_ROOTFS}" || {
        echo "Failed to download Alpine Linux for ${ALPINE_ARCH}"
        echo "Trying alternative version..."
        # Try latest version
        LATEST_ROOTFS="alpine-minirootfs-${ALPINE_VERSION}.9-${ALPINE_ARCH}.tar.gz"
        wget "https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION}/releases/${ALPINE_ARCH}/${LATEST_ROOTFS}" && {
            ALPINE_ROOTFS="$LATEST_ROOTFS"
        } || {
            echo "Failed to download Alpine Linux"
            exit 1
        }
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
# Alpine boot files are in netboot directories
# Try latest netboot version first (3.19.9), then fall back to 3.19.0
KERNEL="vmlinuz-virt"
INITRAMFS="initramfs-virt"

# Try latest netboot version first
NETBOOT_VERSION="${ALPINE_VERSION}.9"
NETBOOT_BASE="https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION}/releases/${ALPINE_ARCH}/netboot-${NETBOOT_VERSION}"
KERNEL_URL="${NETBOOT_BASE}/${KERNEL}"
INITRAMFS_URL="${NETBOOT_BASE}/${INITRAMFS}"

if [ ! -f "$KERNEL" ]; then
    echo "Downloading kernel from: ${KERNEL_URL}"
    if ! wget "$KERNEL_URL" -O "$KERNEL" 2>/dev/null; then
        # Fallback: try older netboot version (3.19.0)
        echo "Trying netboot-${ALPINE_VERSION}.0..."
        NETBOOT_BASE="https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION}/releases/${ALPINE_ARCH}/netboot-${ALPINE_VERSION}.0"
        KERNEL_URL="${NETBOOT_BASE}/${KERNEL}"
        if ! wget "$KERNEL_URL" -O "$KERNEL" 2>/dev/null; then
            echo "Failed to download kernel. Trying alternative: download Alpine ISO and extract boot files..."
            # Alternative: download minimal ISO and extract
            ISO_FILE="alpine-extended-${ALPINE_VERSION}.0-${ARCH}.iso"
            if [ ! -f "$ISO_FILE" ]; then
                echo "Downloading Alpine extended ISO to extract boot files..."
                wget "https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION}/releases/${ARCH}/${ISO_FILE}" || {
                    echo "Failed to download Alpine ISO"
                    exit 1
                }
            fi
            # Extract boot files from ISO (requires 7z or similar)
            echo "Extracting boot files from ISO..."
            if command -v 7z >/dev/null 2>&1; then
                7z x "$ISO_FILE" boot/vmlinuz-virt boot/initramfs-virt 2>/dev/null || true
                if [ -f boot/vmlinuz-virt ]; then
                    mv boot/vmlinuz-virt "$KERNEL"
                    mv boot/initramfs-virt "$INITRAMFS"
                else
                    echo "Failed to extract boot files from ISO"
                    exit 1
                fi
            else
                echo "Install 7z or p7zip to extract boot files: brew install p7zip (macOS) or apt install p7zip-full (Linux)"
                exit 1
            fi
        fi
    fi
fi

if [ ! -f "$INITRAMFS" ]; then
    INITRAMFS_URL="${NETBOOT_BASE}/${INITRAMFS}"
    echo "Downloading initramfs from: ${INITRAMFS_URL}"
    if ! wget "$INITRAMFS_URL" -O "$INITRAMFS" 2>/dev/null; then
        # Fallback: try older netboot version
        NETBOOT_BASE="https://dl-cdn.alpinelinux.org/alpine/v${ALPINE_VERSION}/releases/${ALPINE_ARCH}/netboot-${ALPINE_VERSION}.0"
        INITRAMFS_URL="${NETBOOT_BASE}/${INITRAMFS}"
        echo "Trying netboot-${ALPINE_VERSION}.0 for initramfs..."
        if ! wget "$INITRAMFS_URL" -O "$INITRAMFS" 2>/dev/null; then
            echo "Failed to download initramfs"
            exit 1
        fi
    fi
fi

sudo mkdir -p iso/boot
sudo cp "$KERNEL" iso/boot/vmlinuz-virt
sudo cp "$INITRAMFS" iso/boot/initramfs-virt

# Create GRUB config
echo "Creating GRUB configuration..."
sudo mkdir -p iso/boot/grub
sudo tee iso/boot/grub/grub.cfg > /dev/null << EOF
set timeout=5
set default=0

menuentry "Void OS Elite" {
    linux /boot/vmlinuz-virt root=/dev/loop0 init=/init
    initrd /boot/initramfs-virt
}

menuentry "Void OS Elite (Safe Mode)" {
    linux /boot/vmlinuz-virt root=/dev/loop0 init=/init single
    initrd /boot/initramfs-virt
}
EOF

# Set up EFI boot structure
echo "Setting up EFI boot structure..."
sudo mkdir -p iso/EFI/BOOT

# Create EFI bootloader if grub-mkstandalone is available
if command -v grub-mkstandalone >/dev/null 2>&1; then
    echo "Creating EFI bootloader with grub-mkstandalone..."
    
    # Determine EFI architecture
    if [ "$ARCH" = "aarch64" ]; then
        EFI_FORMAT="arm64-efi"
        EFI_NAME="BOOTAA64.EFI"
    else
        EFI_FORMAT="x86_64-efi"
        EFI_NAME="BOOTX64.EFI"
    fi
    
    # Create EFI bootloader with correct architecture
    echo "Creating EFI bootloader: $EFI_NAME (format: $EFI_FORMAT)"
    sudo grub-mkstandalone \
        --format=$EFI_FORMAT \
        --output=iso/EFI/BOOT/$EFI_NAME \
        --locales="" \
        --fonts="" \
        "boot/grub/grub.cfg=iso/boot/grub/grub.cfg" 2>&1 | grep -v "Installing for" || {
        echo "Warning: Failed to create EFI bootloader for $EFI_FORMAT"
        echo "This may be because the GRUB package for this architecture is not installed."
        if [ "$ARCH" = "aarch64" ]; then
            echo "Install: sudo apt install grub-efi-arm64-bin"
        else
            echo "Install: sudo apt install grub-efi-amd64-bin"
        fi
    }
    
    # Also create BIOS bootloader (for x86_64 only)
    if [ "$ARCH" = "x86_64" ] && command -v grub-mkstandalone >/dev/null 2>&1; then
        echo "Creating BIOS bootloader..."
        sudo mkdir -p iso/boot/grub/i386-pc
        sudo grub-mkstandalone \
            --format=i386-pc \
            --output=iso/boot/grub/core.img \
            --locales="" \
            --fonts="" \
            "boot/grub/grub.cfg=iso/boot/grub/grub.cfg" 2>&1 | grep -v "Installing for" || true
    fi
else
    echo "Warning: grub-mkstandalone not found. EFI boot may not work."
    echo "Install:"
    if [ "$ARCH" = "aarch64" ]; then
        echo "  sudo apt install grub-efi-arm64-bin"
    else
        echo "  sudo apt install grub-efi-amd64-bin"
    fi
fi

# Create ISO
echo "Creating ISO file..."
ISO_FILE="${ISO_NAME}-${ARCH}.iso"
ISO_PATH="${WORK_DIR}/${ISO_FILE}"

# Try GRUB-based ISO creation first (if grub-mkrescue is available)
if command -v grub-mkrescue >/dev/null 2>&1; then
    echo "Using GRUB to create bootable ISO..."
    # Create a temporary grub config for mkrescue
    TEMP_GRUB_DIR=$(mktemp -d)
    mkdir -p "$TEMP_GRUB_DIR/boot/grub"
    cp iso/boot/grub/grub.cfg "$TEMP_GRUB_DIR/boot/grub/"
    cp iso/boot/vmlinuz-virt "$TEMP_GRUB_DIR/boot/" 2>/dev/null || true
    cp iso/boot/initramfs-virt "$TEMP_GRUB_DIR/boot/" 2>/dev/null || true
    
    # Copy EFI boot files if they exist
    if [ -d "iso/EFI" ]; then
        cp -r iso/EFI "$TEMP_GRUB_DIR/" 2>/dev/null || true
    fi
    
    # Copy rest of ISO content
    cp -r iso/* "$TEMP_GRUB_DIR/" 2>/dev/null || true
    
    # Create ISO with GRUB
    echo "Running grub-mkrescue (this may take a minute)..."
    if grub-mkrescue -o "$ISO_FILE" "$TEMP_GRUB_DIR" 2>&1 | grep -v "xorriso: UPDATE:"; then
        if [ -f "$ISO_FILE" ]; then
            echo "✓ ISO created successfully: $ISO_FILE"
        else
            echo "✗ Error: grub-mkrescue completed but ISO file not found!"
            ISO_CREATED=false
        fi
    else
        echo "✗ Error: grub-mkrescue failed!"
        ISO_CREATED=false
    fi
    rm -rf "$TEMP_GRUB_DIR"
else
    # Fallback: Create ISO with EFI support using xorriso
    echo "Creating ISO with EFI boot support using xorriso..."
    ISO_CREATED=false
    
    EFI_BOOT_ARGS=""
    if [ -f "iso/EFI/BOOT/BOOTX64.EFI" ] || [ -f "iso/EFI/BOOT/BOOTAA64.EFI" ]; then
        if [ -f "iso/EFI/BOOT/BOOTAA64.EFI" ]; then
            EFI_BOOT_ARGS="-e EFI/BOOT/BOOTAA64.EFI -no-emul-boot"
        else
            EFI_BOOT_ARGS="-e EFI/BOOT/BOOTX64.EFI -no-emul-boot"
        fi
        echo "Found EFI bootloader, creating bootable ISO..."
    else
        echo "Warning: No EFI bootloader found. Creating basic ISO (may not boot in UEFI mode)."
    fi
    
    # Try creating ISO with EFI boot support
    if sudo xorriso -as mkisofs \
        -R -J \
        -V "VOIDOSELITE" \
        -o "$ISO_FILE" \
        $EFI_BOOT_ARGS \
        iso/ 2>&1 | grep -v "xorriso: UPDATE:"; then
        if [ -f "$ISO_FILE" ]; then
            echo "✓ ISO created successfully: $ISO_FILE"
            ISO_CREATED=true
        fi
    fi
    
    # If that failed, try basic ISO without boot options
    if [ "$ISO_CREATED" != "true" ]; then
        echo "Trying basic ISO creation..."
        if sudo xorriso -as mkisofs \
            -R -J \
            -V "VOIDOSELITE" \
            -o "$ISO_FILE" \
            iso/ 2>&1 | grep -v "xorriso: UPDATE:"; then
            if [ -f "$ISO_FILE" ]; then
                echo "✓ Basic ISO created: $ISO_FILE"
                echo "Warning: This ISO may not be bootable. Install GRUB tools for bootable ISO."
                ISO_CREATED=true
            fi
        fi
    fi
    
    if [ "$ISO_CREATED" != "true" ]; then
        echo ""
        echo "✗ Error: Failed to create ISO file!"
        echo "The 'iso/' folder contains the prepared files, but ISO creation failed."
        echo ""
        echo "Troubleshooting:"
        echo "  1. Check if xorriso is installed: which xorriso"
        echo "  2. Try installing GRUB: sudo apt install grub-efi-amd64-bin (or grub-efi-arm64-bin)"
        echo "  3. Or use grub-mkrescue: sudo apt install grub-pc-bin"
        echo ""
        echo "You can manually create the ISO later using:"
        echo "  sudo xorriso -as mkisofs -R -J -V VOIDOSELITE -o void-os-elite.iso iso/"
        exit 1
    fi
    
    if [ ! -f "iso/EFI/BOOT/BOOTX64.EFI" ] && [ ! -f "iso/EFI/BOOT/BOOTAA64.EFI" ]; then
        echo ""
        echo "Warning: EFI bootloader not found. ISO may not boot in UEFI mode."
        echo "Install GRUB:"
        if [ "$ARCH" = "aarch64" ]; then
            echo "  sudo apt install grub-efi-arm64-bin"
        else
            echo "  sudo apt install grub-efi-amd64-bin"
        fi
    fi
fi

# Set ownership (macOS-compatible)
if [ "$(uname)" = "Darwin" ]; then
    sudo chown "$USER" "$ISO_FILE" 2>/dev/null || true
else
    sudo chown "$USER:$USER" "$ISO_FILE" 2>/dev/null || true
fi

# Verify ISO was created
if [ ! -f "$ISO_FILE" ]; then
    echo ""
    echo "✗ Error: ISO file was not created!"
    echo "Location expected: ${WORK_DIR}/${ISO_FILE}"
    echo "Current directory: $(pwd)"
    echo "Files in current directory:"
    ls -lah *.iso 2>/dev/null || echo "  (no .iso files found)"
    exit 1
fi

echo ""
echo "=========================================="
echo "  ISO Creation Complete!"
echo "=========================================="
echo ""
echo "✓ ISO file created: ${WORK_DIR}/${ISO_FILE}"
echo "  Full path: $(pwd)/${ISO_FILE}"
echo "  Size: $(du -h "$ISO_FILE" | cut -f1)"
echo ""
echo "To write to USB:"
echo "  sudo dd if=${ISO_FILE} of=/dev/sdX bs=4M status=progress oflag=sync"
echo ""
echo "To use in VM (QEMU):"
if [ "$ARCH" = "aarch64" ]; then
    echo "  qemu-system-aarch64 -M virt -cpu cortex-a72 -cdrom ${ISO_FILE} -m 512M"
else
    echo "  qemu-system-x86_64 -cdrom ${ISO_FILE} -m 512M"
fi
echo ""
echo "To use in VirtualBox:"
echo "  VBoxManage storageattach \"VM Name\" --storagectl \"IDE\" --port 0 --device 0 --type dvddrive --medium ${ISO_FILE}"
echo ""
echo "Important: Make sure your VM architecture matches the ISO architecture!"
echo "  ISO Architecture: $ARCH"
echo "  If using UTM on macOS with Apple Silicon, use: ARCH=aarch64 bash create_void_iso.sh"
echo ""

