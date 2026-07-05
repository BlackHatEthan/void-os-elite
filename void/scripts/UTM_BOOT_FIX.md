# Fixing UTM Boot Issues

## Problem
When booting Void OS Elite ISO in UTM, you see "UEFI Interactive Shell" instead of the OS booting.

## Root Causes

1. **Architecture Mismatch**: Your UTM VM is configured for ARM64, but the ISO was built for x86_64
2. **Missing EFI Bootloader**: The ISO doesn't have proper EFI boot files

## Solution

### Step 1: Rebuild ISO for ARM64

Since your UTM VM is ARM64 (Apple Silicon Mac), rebuild the ISO with the correct architecture:

```bash
cd void/scripts
ARCH=aarch64 sudo bash create_void_iso.sh
```

This will:
- Download ARM64 Alpine Linux files
- Create ARM64 EFI bootloader
- Generate `void-os-elite-aarch64.iso`

### Step 2: Install Required Dependencies (if needed)

If you see errors about missing GRUB tools:

**On macOS (using Homebrew):**
```bash
brew install grub
```

**On Linux (in WSL or Linux VM):**
```bash
# For ARM64
sudo apt install grub-efi-arm64-bin xorriso

# For x86_64
sudo apt install grub-efi-amd64-bin xorriso
```

### Step 3: Configure UTM VM

1. **Set Architecture**: Make sure your UTM VM is set to ARM64 (which it already is)
2. **Mount ISO**: Mount the newly created `void-os-elite-aarch64.iso`
3. **Boot**: Start the VM

### Step 4: Alternative - Use x86_64 VM

If you prefer to use x86_64:

1. **Change UTM VM Architecture**:
   - Stop the VM
   - Edit VM settings
   - Change Architecture from "ARM64" to "x86_64"
   - Save

2. **Rebuild ISO for x86_64**:
   ```bash
   ARCH=x86_64 sudo bash create_void_iso.sh
   ```

3. **Mount and Boot**: Use `void-os-elite-x86_64.iso`

## Verification

After rebuilding, check the ISO was created correctly:

```bash
ls -lh ~/void_iso/void-os-elite-*.iso
```

The ISO should be several hundred MB in size.

## Troubleshooting

### Still seeing UEFI Shell?

1. **Check EFI bootloader exists**:
   ```bash
   # Mount ISO temporarily
   mkdir -p /tmp/iso_mount
   hdiutil attach ~/void_iso/void-os-elite-aarch64.iso -mountpoint /tmp/iso_mount
   ls -la /tmp/iso_mount/EFI/BOOT/
   # Should see BOOTAA64.EFI (for ARM64) or BOOTX64.EFI (for x86_64)
   hdiutil detach /tmp/iso_mount
   ```

2. **Check VM Boot Order**:
   - In UTM VM settings, ensure CD/DVD is first in boot order
   - Or manually select boot device when VM starts

3. **Try Different Boot Mode**:
   - Some VMs work better with BIOS boot instead of UEFI
   - Try changing boot mode in VM settings

### GRUB Installation Fails?

On macOS, GRUB installation via Homebrew can be complex. Options:

1. **Use Linux VM/WSL**: Build the ISO on a Linux system where GRUB is standard
2. **Use grub-mkrescue**: If available, it handles bootloader creation automatically
3. **Manual EFI Setup**: Copy EFI bootloader files manually (advanced)

## Quick Reference

```bash
# ARM64 ISO (for Apple Silicon Macs / ARM VMs)
ARCH=aarch64 sudo bash create_void_iso.sh

# x86_64 ISO (for Intel/AMD VMs)
ARCH=x86_64 sudo bash create_void_iso.sh

# Auto-detect (uses system architecture)
sudo bash create_void_iso.sh
```
