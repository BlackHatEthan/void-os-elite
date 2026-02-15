# Void OS Elite - Complete User Guide

## 📚 Table of Contents

1. [Getting Started](#getting-started)
2. [Installation Guide](#installation-guide)
3. [Network & WiFi Setup](#network--wifi-setup)
4. [Kali Tools Installation](#kali-tools-installation)
5. [Command Reference](#command-reference)
6. [Using void-help](#using-void-help)
7. [Common Workflows](#common-workflows)
8. [Troubleshooting](#troubleshooting)

---

## Getting Started

### What is Void OS Elite?

Void OS Elite is a comprehensive CLI operating system with:
- **1,000+ Commands** organized into 14 categories
- **Native Network & WiFi Management** - Full internet connectivity
- **Kali Tools Integration** - Install and use security tools
- **Developer-Focused** - Git integration, project awareness
- **Extensible** - Plugin architecture for customization

### First Launch

```bash
# Build Void OS
cd void
make

# Launch
./void
```

You'll see the boot sequence and then the prompt:
```
[14:22:10] void@user:/home/user [main*][+0]$
```

---

## Installation Guide

### System Requirements

- **OS**: Linux (Alpine, Debian, Ubuntu, Fedora, Arch, etc.)
- **Architecture**: x86_64, ARM, ARM64
- **Memory**: 512MB minimum (2GB+ recommended)
- **Disk**: 100MB base + 2-3GB for Kali tools (optional)

### Building from Source

```bash
# Clone or extract Void OS
cd void

# Compile
make

# Optional: Install system-wide
sudo make install

# Run
./void
```

### Creating Portable USB

```bash
# Create portable package
make portable

# Copy to USB
cp -r portable /path/to/usb/

# Run from USB
cd /path/to/usb/portable
./run.sh
```

### Creating Bootable ISO

See `ISO_CREATION_GUIDE.md` for complete step-by-step instructions.

**Quick ISO Creation:**
```bash
cd scripts
sudo ./create_void_iso.sh
```

This creates `void-os-elite.iso` in `~/void_iso/` which can be:
- Burned to CD/DVD
- Written to USB drives
- Used in virtual machines
- Distributed for installation

---

## Network & WiFi Setup

### Step 1: Install Network Tools

```bash
sudo ./scripts/setup_network.sh
```

**What this installs:**
- NetworkManager (WiFi management)
- Wireless tools (iw, iwlist)
- DHCP client (dhcpcd/dhclient)
- Network utilities (ip, ifconfig, etc.)

### Step 2: Connect to WiFi

```bash
# Scan for available networks
wifi-scan

# Connect to a network
wifi-connect MyNetwork mypassword

# Check connection status
wifi-status

# Test Internet connectivity
net-test
```

### Step 3: Configure Network (if needed)

```bash
# Show all network interfaces
net-status

# Configure interface with DHCP
net-dhcp eth0

# Configure static IP
net-static eth0 192.168.1.100 255.255.255.0 192.168.1.1

# Restart interface
net-restart eth0
```

### WiFi Commands Reference

```bash
wifi-scan                    # Scan for WiFi networks
wifi-connect SSID password    # Connect to WiFi
wifi-disconnect              # Disconnect from WiFi
wifi-status                  # Show WiFi connection status
wifi-on                      # Enable WiFi radio
wifi-off                     # Disable WiFi radio
wifi-list                    # Alias for wifi-scan
```

### Network Management Commands

```bash
net-status                   # Show all network interfaces
net-up <interface>           # Bring up interface
net-down <interface>         # Bring down interface
net-restart <interface>     # Restart interface
net-dhcp <interface>         # Configure DHCP
net-static <interface> IP mask gateway  # Static IP
net-test                     # Test Internet connectivity
net-check                    # Alias for net-test
```

### DNS Management

```bash
dns-get                      # Show current DNS servers
dns-set <dns1> [dns2]        # Set DNS servers (requires root)
dns-test [hostname]          # Test DNS resolution
```

---

## Kali Tools Installation

### Prerequisites

Before installing Kali tools, ensure you have:

1. **Network connectivity** (WiFi or Ethernet)
2. **All dependencies installed**

### Complete Installation Process

#### Step 1: Check Dependencies

```bash
./scripts/check_kali_dependencies.sh
```

This shows which dependencies are installed and which are missing.

#### Step 2: Install Dependencies

```bash
sudo ./scripts/install_kali_dependencies.sh
```

**What gets installed:**
- git (for cloning repositories)
- gcc, make (for compiling tools)
- python3 (for SQLMap)
- ruby, bundler (for Metasploit, WPScan)
- perl (for Nikto)
- go (for Gobuster)
- rust, cargo (for Feroxbuster)
- Development libraries (libssl-dev, libssh-dev, opencl-headers)

#### Step 3: Verify Network

```bash
# Test Internet connectivity
net-test

# Should show:
# Testing DNS... OK
# Testing HTTP... OK
# Testing ping... OK
```

#### Step 4: Install Kali Tools

```bash
# List all available tools
void-install-kali list

# Install specific tool
void-install-kali sqlmap
void-install-kali metasploit
void-install-kali nmap

# Install all 10 tools (takes time)
void-install-kali all
```

#### Step 5: Use Tools

After installation, tools are available as regular commands:

```bash
# SQLMap
sqlmap -u http://target.com/page?id=1 --dbs

# Metasploit
metasploit
msfconsole

# Nmap
nmap -sS -p 1-1000 target.com

# Hashcat
hashcat -m 0 hash.txt wordlist.txt

# John the Ripper
john --wordlist=wordlist.txt hashfile.txt
```

### Available Kali Tools

1. **Metasploit Framework** - Penetration testing platform
2. **SQLMap** - SQL injection testing
3. **Nikto** - Web server scanner
4. **Nmap** - Network mapper
5. **WPScan** - WordPress vulnerability scanner
6. **Gobuster** - Directory/file brute-forcer
7. **Feroxbuster** - Fast content discovery
8. **John the Ripper** - Password cracker
9. **Hashcat** - Advanced password recovery
10. **Hydra** - Network login cracker

---

## Command Reference

### Using void-help

The `void-help` command provides comprehensive help for all commands:

```bash
# Main help menu
void-help

# Show all commands organized by category
void-help all

# Show commands in a specific category
void-help core              # Core shell commands
void-help filesystem        # Filesystem operations
void-help network           # Networking commands
void-help wifi              # WiFi commands
void-help security          # Security tools
void-help development       # Development tools

# Show help for specific command
void-help sqlmap
void-help wifi-connect
```

### Command Categories

#### 1. Core Shell Commands
Basic operations, navigation, text processing
- `exit`, `echo`, `pwd`, `cd`, `whoami`
- `history`, `history-search`, `history-clear`
- `cat`, `grep`, `sed`, `awk`, `sort`, `uniq`
- `head`, `tail`, `less`, `more`

#### 2. Void OS Styled Commands
Void-themed command names
- `pulse` (list files), `warp` (change directory)
- `vanish` (clear), `echoes` (view file)
- `shroud` (create), `transmute` (move/rename)
- `tether` (git), `anchor` (clone), `sync` (pull)
- `entropy` (system monitor)

#### 3. Filesystem Operations
File and directory management
- File ops: `touch`, `mkdir`, `rm`, `cp`, `mv`, `ln`
- File info: `stat`, `file`, `du`, `df`, `find`
- Permissions: `chmod`, `chown`, `chgrp`
- Compression: `gzip`, `bzip2`, `xz`, `zip`, `tar`
- Checksums: `md5sum`, `sha256sum`, `sha512sum`

#### 4. Process Management
Process control and monitoring
- Control: `kill`, `killall`, `pkill`, `nice`, `renice`
- Info: `ps`, `pstree`, `pgrep`, `pidof`, `top`
- Jobs: `jobs`, `fg`, `bg`, `wait`
- Scheduling: `cron`, `at`, `systemctl`

#### 5. Networking
Network tools and utilities
- Basic: `ping`, `traceroute`, `ifconfig`, `ip`, `route`
- Scanning: `nmap`, `masscan`, `arp-scan`
- Monitoring: `netstat`, `ss`, `tcpdump`
- Services: `curl`, `wget`, `ssh`, `scp`, `ftp`

#### 6. WiFi & Network Management
WiFi and network configuration
- WiFi: `wifi-scan`, `wifi-connect`, `wifi-status`
- Network: `net-status`, `net-dhcp`, `net-static`
- Testing: `net-test`, `dns-get`, `dns-set`

#### 7. System Administration
System management
- Info: `uname`, `uptime`, `free`, `df`, `lscpu`
- Users: `useradd`, `userdel`, `passwd`, `su`, `sudo`
- Packages: `apt`, `yum`, `pacman`, `pip`, `npm`
- Control: `shutdown`, `reboot`, `systemctl`

#### 8. Text Processing
Text editors and manipulation
- Editors: `vi`, `vim`, `nano`, `emacs`
- Tools: `expand`, `rev`, `shuf`, `column`

#### 9. Development Tools
Compilers, build systems, testing
- VCS: `svn`, `hg`, `bzr`, `cvs`
- Build: `make`, `cmake`, `gradle`, `maven`
- Compilers: `gcc`, `clang`, `rustc`, `javac`
- Analysis: `pylint`, `eslint`, `cppcheck`
- Testing: `pytest`, `mocha`, `jest`

#### 10. Database Operations
Database clients
- SQL: `mysql`, `psql`, `sqlite3`
- NoSQL: `mongo`, `redis-cli`, `cassandra-cli`

#### 11. Security & Encryption
Security tools and encryption
- Tools: `metasploit`, `sqlmap`, `nmap`, `hashcat`, `john`
- Encryption: `gpg`, `ssh-keygen`
- Installer: `void-install-kali`

#### 12. System Utilities
Date, time, calendar
- `date`, `cal`, `timedatectl`

#### 13. Void OS Specific
Void OS management
- System: `void-info`, `void-status`, `void-health`
- Config: `void-config`, `void-theme`, `void-plugins`
- Help: `void-help`, `void-docs`, `void-tutorial`

#### 14. Multimedia & Archives
Media and compression tools
- Image: `convert`, `ffmpeg`
- Audio: `sox`, `mpg123`
- Compression: `lzma`, `zstd`, `lz4`

---

## Using void-help

### Basic Usage

```bash
# Show main help menu
void-help

# Show all commands by category
void-help all

# Show commands in a category
void-help network
void-help wifi
void-help security
void-help filesystem

# Show help for specific command
void-help sqlmap
void-help wifi-connect
```

### Category Aliases

You can use category numbers or names:

```bash
void-help 1          # Core commands
void-help core       # Same as above
void-help network    # Networking
void-help 4          # Same as above
void-help wifi       # WiFi commands
void-help security   # Security tools
void-help 9          # Same as above
```

### Examples

```bash
# See all networking commands
void-help network

# See all WiFi commands
void-help wifi

# See all security tools
void-help security

# See all Void OS commands
void-help void-os

# See all filesystem commands
void-help filesystem
```

---

## Common Workflows

### Workflow 1: Initial Setup

```bash
# 1. Build Void OS
make

# 2. Setup network
sudo ./scripts/setup_network.sh

# 3. Connect to WiFi
wifi-connect MyNetwork password
net-test

# 4. Install dependencies
sudo ./scripts/install_kali_dependencies.sh

# 5. Install Kali tools
void-install-kali sqlmap
void-install-kali nmap
```

### Workflow 2: Daily Development

```bash
# Check project context
context

# View files
pulse

# Edit files
echoes file.txt

# Git operations
status
commit "Update code"
transmit

# Run Python script
summon script.py
```

### Workflow 3: Security Testing

```bash
# Security check
startup-check

# Install security tools
void-install-kali all

# Use tools
sqlmap -u http://target.com
nmap -sS target.com
hashcat -m 0 hash.txt wordlist.txt
```

### Workflow 4: Network Troubleshooting

```bash
# Check network status
net-status
wifi-status

# Test connectivity
net-test

# Check DNS
dns-get
dns-test google.com

# Restart network
net-restart wlan0
net-dhcp wlan0
```

---

## Troubleshooting

### Network Issues

**Problem**: Cannot connect to WiFi

**Solution**:
```bash
# Check WiFi status
wifi-status

# Enable WiFi
wifi-on

# Scan and connect
wifi-scan
wifi-connect SSID password

# Check NetworkManager
systemctl status NetworkManager
```

**Problem**: No Internet connectivity

**Solution**:
```bash
# Test connectivity
net-test

# Check DNS
dns-get
dns-test google.com

# Restart network
net-restart wlan0
net-dhcp wlan0
```

### Kali Tools Issues

**Problem**: "git: command not found"

**Solution**:
```bash
sudo ./scripts/install_kali_dependencies.sh
```

**Problem**: "Failed to clone repository"

**Solution**:
```bash
# Check network
net-test

# Verify git
which git
git --version
```

**Problem**: Tool installed but command not found

**Solution**:
```bash
# Check installation
ls ~/.void/packages/bin/

# Add to PATH
export PATH="$HOME/.void/packages/bin:$PATH"

# Add to ~/.voidrc for persistence
echo 'export PATH="$HOME/.void/packages/bin:$PATH"' >> ~/.voidrc
```

### Command Issues

**Problem**: Command not recognized

**Solution**:
```bash
# Check if command exists
void-help all | grep command_name

# Search by category
void-help <category>
```

### Build Issues

**Problem**: Compilation errors

**Solution**:
```bash
# Clean and rebuild
make clean
make

# Check dependencies
gcc --version
make --version
```

---

## Quick Reference Card

### Essential Commands

```bash
# Help
void-help              # Main help
void-help all          # All commands
void-help <category>   # Category commands

# Navigation
pulse                  # List files
warp /path            # Change directory
pwd                    # Current directory

# Files
echoes file.txt       # View file
shroud file.txt       # Create file
transmute old new     # Move/rename

# System
void-info             # System info
entropy               # System monitor
void-status           # Current status

# Network
wifi-scan             # Scan WiFi
wifi-connect SSID pass # Connect
net-test              # Test Internet

# Kali Tools
void-install-kali list    # List tools
void-install-kali <tool>  # Install tool
<tool> -h                 # Use tool
```

### Installation Scripts

```bash
# Network setup
sudo ./scripts/setup_network.sh

# Kali dependencies
sudo ./scripts/install_kali_dependencies.sh

# Check dependencies
./scripts/check_kali_dependencies.sh
```

---

## Additional Resources

### Documentation Files

- **`COMPLETE_TUTORIAL.md`** - Comprehensive tutorial
- **`QUICK_START.md`** - 5-minute quick start
- **`KALI_TOOLS_IMPLEMENTATION.md`** - Kali tools guide
- **`NETWORK_WIFI_GUIDE.md`** - Network setup
- **`KALI_DEPENDENCIES.md`** - Dependencies guide
- **`BOOTABLE_GUIDE.md`** - Bootable USB creation

### Getting Help

```bash
void-help              # Main help system
void-info              # System information
void-docs              # Documentation
void-tutorial          # Tutorial
void-examples          # Examples
```

---

## Summary

Void OS Elite provides:

✅ **1,000+ Commands** - Comprehensive command set  
✅ **Network & WiFi** - Full connectivity management  
✅ **Kali Tools** - Security tools integration  
✅ **Developer Tools** - Git, project awareness  
✅ **Help System** - Comprehensive `void-help` command  
✅ **Complete Documentation** - Tutorials and guides  

**Ready to use!** Type `void-help` to get started.

---

**End of User Guide**

