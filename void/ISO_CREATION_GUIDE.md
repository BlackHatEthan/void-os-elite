# Void OS Elite - ISO Creation Guide

## Overview

This guide explains how to create a bootable ISO file for Void OS Elite. An ISO file can be:
- Burned to CD/DVD
- Written to USB drives
- Used in virtual machines
- Distributed for easy installation

---

## Prerequisites

- Linux system (for ISO creation)
- Root/sudo access
- 2GB+ free disk space
- Tools: `genisoimage` or `xorriso`, `grub`, `syslinux` (optional)

---

## Method 1: Create ISO with Alpine Linux Base (Recommended)

This method creates a bootable ISO that includes Alpine Linux as the base system with Void OS as the default shell.

### Step 1: Install Required Tools

```bash
# Debian/Ubuntu
sudo apt install genisoimage xorriso grub-pc-bin grub-efi-amd64-bin

# Alpine Linux
sudo apk add xorriso grub grub-efi

# Fedora/RHEL
sudo dnf install xorriso grub2-efi-x64 grub2-pc
```

### Step 2: Prepare Workspace

```bash
# Create working directory
mkdir -p ~/void_iso
cd ~/void_iso

# Create ISO structure
mkdir -p iso/{boot/grub,bin,etc,home,lib,lib64,mnt,opt,proc,root,run,sbin,sys,tmp,usr,var}
```

### Step 3: Download Alpine Linux Base

```bash
# Download Alpine Linux mini rootfs
cd ~/void_iso
wget https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/alpine-minirootfs-3.19.0-x86_64.tar.gz

# Extract to iso directory
sudo tar -xzf alpine-minirootfs-3.19.0-x86_64.tar.gz -C iso/
```

### Step 4: Build Void OS

```bash
# Build Void OS static binary
cd /path/to/void
make static

# Copy to ISO
cp void ~/void_iso/iso/bin/void
chmod +x ~/void_iso/iso/bin/void
```

### Step 5: Configure Void OS as Default Shell

```bash
cd ~/void_iso

# Create /etc/passwd with void as shell
sudo tee iso/etc/passwd > /dev/null << 'EOF'
root:x:0:0:root:/root:/bin/void
EOF

# Create /etc/shadow (empty password for root)
sudo tee iso/etc/shadow > /dev/null << 'EOF'
root::0:0:99999:7:::
EOF

# Create /etc/group
sudo tee iso/etc/group > /dev/null << 'EOF'
root:x:0:
EOF
```

### Step 6: Create Init Script

```bash
# Create init script that starts Void OS
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
ifconfig lo 127.0.0.1 up

# Start Void OS
exec /bin/void
EOF

sudo chmod +x iso/init
```

### Step 7: Create GRUB Configuration

```bash
# Create GRUB directory structure
sudo mkdir -p iso/boot/grub

# Create GRUB configuration
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
```

### Step 8: Copy Kernel and Initramfs

```bash
# Download Alpine Linux kernel and initramfs
cd ~/void_iso
wget https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/boot/vmlinuz-lts
wget https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/boot/initramfs-lts

# Copy to ISO
sudo cp vmlinuz-lts iso/boot/
sudo cp initramfs-lts iso/boot/
```

### Step 9: Create ISO File

```bash
cd ~/void_iso

# Create ISO using xorriso (recommended)
sudo xorriso -as mkisofs \
    -R -J \
    -V "Void OS Elite" \
    -o void-os-elite.iso \
    -b isolinux/isolinux.bin \
    -c isolinux/boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -isohybrid-mbr /usr/lib/ISOLINUX/isohdpfx.bin \
    iso/

# Or using genisoimage
sudo genisoimage -R -J \
    -V "Void OS Elite" \
    -o void-os-elite.iso \
    -b isolinux/isolinux.bin \
    -c isolinux/boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    iso/
```

### Step 10: Make ISO Bootable (UEFI Support)

```bash
# Install GRUB to ISO for UEFI support
sudo grub-mkstandalone \
    --format=x86_64-efi \
    --output=iso/boot/grub/grubx64.efi \
    --locales="" \
    --fonts="" \
    "boot/grub/grub.cfg=iso/boot/grub/grub.cfg"

# Create EFI boot image
sudo mkdir -p iso/EFI/boot
sudo cp iso/boot/grub/grubx64.efi iso/EFI/boot/bootx64.efi

# Recreate ISO with UEFI support
sudo xorriso -as mkisofs \
    -R -J \
    -V "Void OS Elite" \
    -o void-os-elite.iso \
    -b isolinux/isolinux.bin \
    -c isolinux/boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -eltorito-alt-boot \
    -e EFI/boot/bootx64.efi \
    -no-emul-boot \
    -isohybrid-gpt-basdat \
    iso/
```

---

## Method 2: Automated ISO Creation Script

Create a script to automate the process:

```bash
#!/bin/bash
# create_void_iso.sh - Automated ISO creation script

set -e

ISO_NAME="void-os-elite"
WORK_DIR="$HOME/void_iso"
VOID_DIR="$(cd "$(dirname "$0")/.." && pwd)"

echo "=========================================="
echo "  Void OS Elite - ISO Creator"
echo "=========================================="
echo ""

# Check dependencies
command -v xorriso >/dev/null 2>&1 || { echo "Error: xorriso not found. Install it first."; exit 1; }
command -v grub-mkstandalone >/dev/null 2>&1 || { echo "Error: grub not found. Install it first."; exit 1; }

# Create workspace
mkdir -p "$WORK_DIR/iso"
cd "$WORK_DIR"

# Download Alpine Linux mini rootfs
if [ ! -f alpine-minirootfs-3.19.0-x86_64.tar.gz ]; then
    echo "Downloading Alpine Linux mini rootfs..."
    wget https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/alpine-minirootfs-3.19.0-x86_64.tar.gz
fi

# Extract Alpine
echo "Extracting Alpine Linux..."
sudo rm -rf iso/*
sudo tar -xzf alpine-minirootfs-3.19.0-x86_64.tar.gz -C iso/

# Build Void OS
echo "Building Void OS..."
cd "$VOID_DIR"
make clean
make static

# Copy Void OS
echo "Copying Void OS to ISO..."
sudo cp void "$WORK_DIR/iso/bin/void"
sudo chmod +x "$WORK_DIR/iso/bin/void"

# Configure system
cd "$WORK_DIR"
echo "Configuring system..."

# Create passwd
sudo tee iso/etc/passwd > /dev/null << 'EOF'
root:x:0:0:root:/root:/bin/void
EOF

# Create shadow
sudo tee iso/etc/shadow > /dev/null << 'EOF'
root::0:0:99999:7:::
EOF

# Create group
sudo tee iso/etc/group > /dev/null << 'EOF'
root:x:0:
EOF

# Create init script
sudo tee iso/init > /dev/null << 'EOF'
#!/bin/sh
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t devtmpfs devtmpfs /dev
mknod /dev/console c 5 1
mknod /dev/null c 1 3
ifconfig lo 127.0.0.1 up
exec /bin/void
EOF
sudo chmod +x iso/init

# Download kernel and initramfs
if [ ! -f vmlinuz-lts ]; then
    echo "Downloading kernel..."
    wget https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/boot/vmlinuz-lts
    wget https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/boot/initramfs-lts
fi

sudo cp vmlinuz-lts iso/boot/
sudo cp initramfs-lts iso/boot/

# Create GRUB config
sudo mkdir -p iso/boot/grub
sudo tee iso/boot/grub/grub.cfg > /dev/null << 'EOF'
set timeout=5
set default=0
menuentry "Void OS Elite" {
    linux /boot/vmlinuz-lts root=/dev/loop0 init=/init
    initrd /boot/initramfs-lts
}
EOF

# Create ISO
echo "Creating ISO file..."
sudo xorriso -as mkisofs \
    -R -J \
    -V "Void OS Elite" \
    -o "${ISO_NAME}.iso" \
    -b isolinux/isolinux.bin \
    -c isolinux/boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    iso/

echo ""
echo "=========================================="
echo "  ISO Creation Complete!"
echo "=========================================="
echo ""
echo "ISO file: ${WORK_DIR}/${ISO_NAME}.iso"
echo ""
echo "To write to USB:"
echo "  sudo dd if=${ISO_NAME}.iso of=/dev/sdX bs=4M status=progress"
echo ""
echo "To use in VM:"
echo "  qemu-system-x86_64 -cdrom ${ISO_NAME}.iso"
echo ""
```

Save this as `scripts/create_void_iso.sh` and make it executable:

```bash
chmod +x scripts/create_void_iso.sh
sudo ./scripts/create_void_iso.sh
```

---

## Method 3: Using mkisofs (Alternative)

If you prefer `mkisofs`:

```bash
cd ~/void_iso

sudo mkisofs \
    -o void-os-elite.iso \
    -b isolinux/isolinux.bin \
    -c isolinux/boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -J -r -V "Void OS Elite" \
    iso/
```

---

## Writing ISO to USB

Once you have the ISO file, write it to USB:

```bash
# Find USB device
lsblk

# Write ISO to USB (replace /dev/sdb with your device)
sudo dd if=void-os-elite.iso of=/dev/sdb bs=4M status=progress oflag=sync

# Or use ddrescue for better error handling
sudo ddrescue -d -D void-os-elite.iso /dev/sdb
```

**Warning**: This will erase all data on the USB device!

---

## Using the ISO

### In Virtual Machine

```bash
# QEMU/KVM
qemu-system-x86_64 -cdrom void-os-elite.iso -m 512M

# VirtualBox
VBoxManage createvm --name "Void OS" --register
VBoxManage modifyvm "Void OS" --memory 512 --acpi on
VBoxManage storagectl "Void OS" --name "IDE" --add ide
VBoxManage storageattach "Void OS" --storagectl "IDE" --port 0 --device 0 --type dvddrive --medium void-os-elite.iso
VBoxManage startvm "Void OS"

# VMware
vmware -cdrom void-os-elite.iso
```

### Burn to CD/DVD

```bash
# Using cdrecord
sudo cdrecord -v -eject void-os-elite.iso

# Using growisofs
sudo growisofs -dvd-compat -Z /dev/dvd=void-os-elite.iso

# Using Brasero (GUI)
brasero void-os-elite.iso
```

---

## ISO Customization

### Add Custom Files

```bash
# Add files before creating ISO
sudo cp custom_script.sh ~/void_iso/iso/usr/local/bin/
sudo chmod +x ~/void_iso/iso/usr/local/bin/custom_script.sh

# Recreate ISO
cd ~/void_iso
sudo xorriso -as mkisofs ... iso/
```

### Customize Boot Menu

Edit `iso/boot/grub/grub.cfg` before creating ISO:

```bash
sudo nano ~/void_iso/iso/boot/grub/grub.cfg
```

### Add Network Configuration

```bash
# Create network config
sudo tee ~/void_iso/iso/etc/network/interfaces > /dev/null << 'EOF'
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet dhcp
EOF
```

---

## Troubleshooting

### ISO Won't Boot

1. **Check boot mode**: Ensure BIOS/UEFI settings match ISO type
2. **Verify ISO integrity**: `file void-os-elite.iso` should show "ISO 9660"
3. **Test in VM first**: Use QEMU/VirtualBox to test before burning
4. **Check GRUB config**: Verify `grub.cfg` syntax

### Kernel Panic

1. **Check init script**: Ensure `/init` is executable
2. **Verify kernel**: Ensure kernel matches architecture
3. **Check root filesystem**: Verify all files are present

### Missing Dependencies

```bash
# Install all required tools
sudo apt install xorriso genisoimage grub-pc-bin grub-efi-amd64-bin \
    syslinux isolinux mtools
```

---

## ISO Size Optimization

### Remove Unnecessary Files

```bash
# Remove documentation
sudo rm -rf iso/usr/share/doc

# Remove man pages
sudo rm -rf iso/usr/share/man

# Remove locale files (keep only en_US)
sudo find iso/usr/share/locale -type f ! -name "en_US*" -delete
```

### Compress ISO

```bash
# Create compressed ISO
sudo xorriso -as mkisofs ... | gzip > void-os-elite.iso.gz

# Or use xz
sudo xorriso -as mkisofs ... | xz > void-os-elite.iso.xz
```

---

## Advanced: Hybrid ISO (USB + CD)

Create an ISO that works on both USB and CD:

```bash
sudo xorriso -as mkisofs \
    -R -J \
    -V "Void OS Elite" \
    -o void-os-elite.iso \
    -b isolinux/isolinux.bin \
    -c isolinux/boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -isohybrid-mbr /usr/lib/ISOLINUX/isohdpfx.bin \
    iso/

# Make it bootable on USB
sudo isohybrid void-os-elite.iso
```

---

## Quick Reference

### Create ISO
```bash
cd ~/void_iso
sudo xorriso -as mkisofs -R -J -V "Void OS Elite" \
    -o void-os-elite.iso \
    -b isolinux/isolinux.bin \
    -c isolinux/boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    iso/
```

### Write to USB
```bash
sudo dd if=void-os-elite.iso of=/dev/sdb bs=4M status=progress
```

### Test in VM
```bash
qemu-system-x86_64 -cdrom void-os-elite.iso -m 512M
```

---

## Summary

Creating a Void OS Elite ISO involves:

1. ✅ Setting up Alpine Linux base system
2. ✅ Building Void OS static binary
3. ✅ Configuring Void OS as default shell
4. ✅ Creating init script
5. ✅ Setting up GRUB bootloader
6. ✅ Creating ISO file
7. ✅ Testing in VM
8. ✅ Writing to USB or burning to CD

**Your ISO is ready!** Use it to boot Void OS Elite on any compatible system.

---

**End of ISO Creation Guide**

