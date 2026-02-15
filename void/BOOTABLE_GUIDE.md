# Creating a Bootable Void OS Elite USB

## Overview

Void OS Elite is a shell environment, not a kernel, so it needs an underlying operating system. This guide shows you how to create a bootable USB that boots directly into Void OS Elite.

## Option 1: Portable USB (Easiest - Recommended)

**Best for:** Running Void OS on any Linux system from USB

### Quick Setup

```bash
cd void
make portable
```

This creates a `portable/` directory with:
- Static Void OS binary (no dependencies)
- Run script
- README

### Manual Setup

1. **Build static binary:**
   ```bash
   make static
   ```

2. **Copy to USB:**
   ```bash
   cp void /path/to/usb/void_os/
   chmod +x /path/to/usb/void_os/void
   ```

3. **Create launcher script:**
   ```bash
   echo '#!/bin/bash
   cd "$(dirname "$0")"
   ./void "$@"' > /path/to/usb/void_os/run.sh
   chmod +x /path/to/usb/void_os/run.sh
   ```

4. **Use on any Linux system:**
   ```bash
   cd /path/to/usb/void_os
   ./run.sh
   ```

**Pros:**
- ✅ Works on any Linux system
- ✅ No kernel/init system needed
- ✅ Simple setup
- ✅ Portable

**Cons:**
- ❌ Requires existing Linux system to run
- ❌ Not truly "bootable" (needs host OS)

---

## Option 2: Minimal Linux + Void OS (True Bootable)

**Best for:** Standalone bootable USB that boots directly into Void OS

### Method A: Using Alpine Linux (Recommended)

Alpine Linux is perfect for this - it's only ~5MB and boots quickly.

#### Automated Script

```bash
cd void/scripts
sudo ./create_minimal_linux.sh /dev/sdb
```

(Replace `/dev/sdb` with your USB device)

#### Manual Steps

1. **Download Alpine Linux:**
   ```bash
   wget https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/alpine-standard-3.19-x86_64.iso
   ```

2. **Write to USB:**
   ```bash
   sudo dd if=alpine-standard-3.19-x86_64.iso of=/dev/sdb bs=4M status=progress
   ```

3. **Boot from USB** and login (root, no password)

4. **Install Void OS:**
   ```bash
   # Copy void binary to USB (from another system)
   # Or compile on the Alpine system:
   apk add gcc make musl-dev
   cd /path/to/void
   make
   cp void /usr/local/bin/
   ```

5. **Set as default shell:**
   ```bash
   chsh -s /usr/local/bin/void
   # Or edit /etc/passwd:
   # root:x:0:0:root:/root:/usr/local/bin/void
   ```

6. **Reboot** - Void OS will be the default shell

### Method B: Using Tiny Core Linux

Tiny Core is even smaller (~15MB) and designed for minimal systems.

1. Download Tiny Core Linux
2. Write to USB
3. Boot and install Void OS
4. Configure as default shell

### Method C: Custom Buildroot System

For maximum control, build a custom minimal Linux:

1. Use Buildroot to create minimal Linux
2. Include Void OS in the root filesystem
3. Set Void OS as init or default shell
4. Create bootable image

---

## Option 3: Include in Existing Linux Distribution

Add Void OS to any existing bootable Linux USB:

1. Boot into the Linux system
2. Install Void OS:
   ```bash
   make install  # Installs to /usr/local/bin/void
   ```
3. Set as default shell:
   ```bash
   chsh -s /usr/local/bin/void
   ```
4. Create custom ISO with Void OS included

---

## Technical Requirements

### For True Bootable USB:

1. **Linux Kernel** (vmlinuz)
   - Minimum: ~5MB
   - Recommended: Alpine Linux kernel (~2MB)

2. **Init System**
   - Minimal: Simple init script
   - Standard: BusyBox init, systemd, or OpenRC

3. **Root Filesystem**
   - Minimum: ~50MB
   - Recommended: Alpine Linux (~100MB)

4. **Bootloader**
   - GRUB (most common)
   - syslinux (lighter)
   - UEFI boot manager

5. **Void OS Binary**
   - Static binary: ~500KB
   - Or dynamic with libraries

### Architecture Support

- **x86_64** - Full support, easiest
- **ARM** - Requires cross-compilation
- **ARM64** - Requires cross-compilation

---

## Step-by-Step: Alpine Linux + Void OS

### Prerequisites

- USB drive (4GB+ recommended)
- Linux system for preparation
- Root/sudo access

### Steps

1. **Prepare USB:**
   ```bash
   # Find USB device
   lsblk
   # Format (WARNING: destroys all data!)
   sudo fdisk /dev/sdb
   # Create partition, write, quit
   sudo mkfs.ext4 /dev/sdb1
   ```

2. **Install Alpine Linux:**
   ```bash
   # Download Alpine
   wget https://dl-cdn.alpinelinux.org/alpine/v3.19/releases/x86_64/alpine-standard-3.19-x86_64.iso
   
   # Write to USB
   sudo dd if=alpine-standard-3.19-x86_64.iso of=/dev/sdb bs=4M
   ```

3. **Boot Alpine from USB:**
   - Boot target machine from USB
   - Login as root (no password)
   - Run `setup-alpine` for installation (optional)

4. **Install Void OS:**
   ```bash
   # On Alpine, install build tools
   apk add gcc make musl-dev
   
   # Copy Void OS source to USB or compile directly
   # Then:
   make
   make install
   ```

5. **Configure Void OS as Shell:**
   ```bash
   # Edit /etc/passwd
   vi /etc/passwd
   # Change: root:x:0:0:root:/root:/bin/ash
   # To:     root:x:0:0:root:/root:/usr/local/bin/void
   ```

6. **Reboot** - Void OS Elite will start automatically!

---

## Quick Reference

### Portable USB
```bash
make portable
# Copy portable/ directory to USB
# Run ./run.sh on any Linux system
```

### Static Binary
```bash
make static
# Binary is self-contained, no dependencies
# Copy to USB, run on any Linux system
```

### Bootable USB (Alpine)
```bash
cd scripts
sudo ./create_minimal_linux.sh /dev/sdb
# Follow prompts
```

### Create ISO File
```bash
cd scripts
sudo ./create_void_iso.sh
# Creates void-os-elite.iso in ~/void_iso/
# See ISO_CREATION_GUIDE.md for detailed instructions
```

---

## Troubleshooting

### USB Won't Boot
- Check BIOS/UEFI boot order
- Ensure USB is formatted correctly
- Try different USB port
- Verify bootloader is installed

### Void OS Not Starting
- Check /etc/passwd shell path
- Verify void binary is executable
- Check file permissions
- Review logs in ~/.void_logs

### Missing Dependencies
- Use static binary: `make static`
- Or include required libraries in rootfs

---

## Next Steps

Once you have a bootable USB:

1. **Customize** - Edit `~/.voidrc` for your preferences
2. **Install plugins** - Use `forge install` to add functionality
3. **Configure** - Set up aliases, environment variables
4. **Expand** - Add your own tools and scripts

---

**Void OS Elite** - Now bootable and portable! 🚀

