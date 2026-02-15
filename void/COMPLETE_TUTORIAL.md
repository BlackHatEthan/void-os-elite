# Void OS Elite - Complete Tutorial & User Guide

**Version**: 1.0  
**Last Updated**: 2024  
**Status**: Complete Reference Guide

---

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [First Steps](#first-steps)
4. [Basic Usage](#basic-usage)
5. [Network & WiFi Setup](#network--wifi-setup)
6. [Installing Kali Tools](#installing-kali-tools)
7. [Command Reference](#command-reference)
8. [Advanced Features](#advanced-features)
9. [Troubleshooting](#troubleshooting)
10. [Quick Reference](#quick-reference)

---

## Introduction

### What is Void OS Elite?

Void OS Elite is a high-performance, developer-grade, security-aware CLI operating system written in C and Assembly. It provides a comprehensive shell environment with over 1,000 built-in commands, native network management, WiFi support, and integrated security tools.

### Key Features

- **1,000+ Commands**: Comprehensive command set covering all aspects of system management
- **Native Implementation**: All core features implemented in C/Assembly, no external dependencies
- **WiFi & Network Management**: Full network and WiFi configuration capabilities
- **Kali Tools Integration**: Install and use Kali Linux Top 10 security tools
- **Git Integration**: Native Git operations without external git binary
- **Developer Tools**: Built-in utilities for development workflows
- **Plugin Architecture**: Extensible through dynamic plugins
- **Security Awareness**: Built-in security auditing and monitoring tools

### System Requirements

- **Operating System**: Linux (Alpine, Debian, Ubuntu, Fedora, Arch, etc.)
- **Architecture**: x86_64, ARM, ARM64
- **Memory**: 512MB minimum (2GB+ recommended)
- **Disk Space**: 100MB base + 2-3GB for Kali tools (optional)
- **Network**: Internet connection for tool installation

---

## Installation

### Method 1: From Source (Recommended)

```bash
# Clone or download Void OS
cd void

# Compile
make

# Install (optional)
sudo make install

# Run
./void
```

### Method 2: Portable USB

```bash
# Create portable package
make portable

# Copy to USB
cp -r portable /path/to/usb/

# Run from USB on any Linux system
cd /path/to/usb/portable
./run.sh
```

### Method 3: Bootable ISO

See `ISO_CREATION_GUIDE.md` for complete instructions on creating ISO files.

**Quick ISO Creation:**
```bash
cd scripts
sudo ./create_void_iso.sh
# Creates void-os-elite.iso in ~/void_iso/
```

**Write ISO to USB:**
```bash
sudo dd if=~/void_iso/void-os-elite.iso of=/dev/sdb bs=4M status=progress
```

---

## First Steps

### 1. Launch Void OS

```bash
./void
```

You'll see the boot sequence:
```
╔═══════════════════════════════════════╗
║         VOID OS ELITE                 ║
╚═══════════════════════════════════════╝

Piercing the Veil…
Mounting Reality…
Initializing Process Manager…
Loading Environment Matrix…
Binding Developer Tools…
Detecting Security Capabilities…
Synchronizing Git Interface…
Stabilizing Void Core…

Void OS Elite Initialized.
```

### 2. Understanding the Prompt

```
[14:22:10] void@user:/projects/api [main*][+1]$
```

- `[14:22:10]` - Current time
- `void@user` - Shell name and username
- `/projects/api` - Current directory
- `[main*]` - Git branch (with * for dirty state)
- `[+1]` - Background job count

### 3. Basic Commands

```bash
# Navigation
pwd                    # Print working directory
warp /path/to/dir      # Change directory (cd)
pulse                  # List files (ls)

# File operations
echoes file.txt        # View file (cat)
shroud file.txt        # Create file (touch)
transmute old new      # Move/rename (mv)

# System info
void-info              # System information
void-status            # Current status
entropy                # System monitoring
```

### 4. Getting Help

```bash
# Main help menu
void-help

# Show all commands by category
void-help all

# Show commands in a category
void-help network
void-help wifi
void-help security

# Show help for specific command
void-help sqlmap
```

---

## Basic Usage

### Filesystem Operations

#### Void-Styled Commands

```bash
pulse                  # List files with details
warp /path            # Change directory
vanish                 # Clear terminal
echoes file.txt       # Display file contents
shroud file.txt       # Create file
transmute old new     # Move/rename
veil file.txt         # Show permissions
seal file.txt 755     # Change permissions (chmod)
watch-dir /path       # Monitor directory for changes
```

#### Standard Commands

```bash
# File operations
touch file.txt
mkdir directory
rm file.txt
cp source dest
mv old new
ln -s target link

# File information
stat file.txt
file file.txt
du -h directory
df -h

# Search
find /path -name "*.txt"
grep "pattern" file.txt
```

### Process Management

```bash
# Run in background
spawn command &

# List jobs
jobs

# Process information
ps                    # List processes
pstree                # Process tree
pgrep process_name    # Find process by name

# Control processes
terminate <pid>       # Kill process
foreground <pid>      # Bring to foreground
kill <pid>            # Kill process
```

### Environment & Aliases

```bash
# Environment variables
env                   # Show all variables
export VAR=value      # Set variable
unset VAR             # Remove variable

# Aliases
alias ll='pulse -l'   # Create alias
aliases               # List all aliases
unalias ll            # Remove alias
```

### Git Integration

```bash
# Git operations (native)
tether status         # Git status
tether branch         # Show branch
anchor https://...    # Clone repository
sync                  # Pull changes
transmit              # Push changes
commit "message"      # Commit changes
status                # Rich git status
```

### History

```bash
history               # Show command history
history-search term   # Search history
history-clear         # Clear history
```

---

## Network & WiFi Setup

### Initial Network Setup

#### Step 1: Install Network Tools

```bash
sudo ./scripts/setup_network.sh
```

This installs:
- NetworkManager
- Wireless tools (iw, iwlist)
- DHCP client
- Network utilities

#### Step 2: Connect to WiFi

```bash
# Scan for networks
wifi-scan

# Connect to network
wifi-connect MyNetwork mypassword

# Check status
wifi-status

# Test connectivity
net-test
```

#### Step 3: Configure Network (if needed)

```bash
# Show network interfaces
net-status

# Configure DHCP
net-dhcp eth0

# Configure static IP
net-static eth0 192.168.1.100 255.255.255.0 192.168.1.1

# Test Internet
net-test
```

### WiFi Commands

```bash
wifi-scan              # Scan for WiFi networks
wifi-connect SSID pass # Connect to WiFi
wifi-disconnect        # Disconnect from WiFi
wifi-status            # Show WiFi status
wifi-on                # Enable WiFi
wifi-off               # Disable WiFi
```

### Network Management Commands

```bash
net-status             # Show all interfaces
net-up eth0            # Bring up interface
net-down eth0          # Bring down interface
net-restart eth0     # Restart interface
net-dhcp eth0          # Configure DHCP
net-static eth0 IP mask gateway  # Static IP
net-test               # Test Internet connectivity
```

### DNS Management

```bash
dns-get                # Show current DNS servers
dns-set 8.8.8.8 8.8.4.4  # Set DNS servers (requires root)
dns-test google.com    # Test DNS resolution
```

---

## Installing Kali Tools

### Prerequisites

Before installing Kali tools, you need:

1. **Network connectivity** (WiFi or Ethernet)
2. **Dependencies installed** (git, build tools, language runtimes)

### Step-by-Step Installation

#### Step 1: Install Dependencies

```bash
# Check current dependencies
./scripts/check_kali_dependencies.sh

# Install all dependencies
sudo ./scripts/install_kali_dependencies.sh
```

This installs:
- git (for cloning)
- gcc, make (for compiling)
- python3 (for SQLMap)
- ruby, bundler (for Metasploit, WPScan)
- perl (for Nikto)
- go (for Gobuster)
- rust, cargo (for Feroxbuster)
- Development libraries

#### Step 2: Verify Network

```bash
# Test Internet connectivity
net-test

# Should show:
# Testing DNS... OK
# Testing HTTP... OK
# Testing ping... OK
```

#### Step 3: Install Kali Tools

```bash
# List available tools
void-install-kali list

# Install specific tool
void-install-kali sqlmap
void-install-kali metasploit
void-install-kali nmap

# Install all tools (takes time)
void-install-kali all
```

#### Step 4: Use Tools

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

1. **Metasploit Framework** - Penetration testing
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

### Getting Help

```bash
void-help              # Main help menu
void-help all          # Show all commands
void-help <category>   # Show category commands
void-help <command>    # Help for specific command
```

### Command Categories

#### Category 1: Core Shell Commands
- Basic operations: `exit`, `echo`, `pwd`, `cd`, `whoami`
- History: `history`, `history-search`, `history-clear`
- Text processing: `cat`, `grep`, `sed`, `awk`, `sort`, `uniq`
- File viewing: `head`, `tail`, `less`, `more`

#### Category 2: Filesystem Operations
- File operations: `touch`, `mkdir`, `rm`, `cp`, `mv`, `ln`
- File info: `stat`, `file`, `du`, `df`, `find`
- Permissions: `chmod`, `chown`, `chgrp`, `umask`
- Compression: `gzip`, `bzip2`, `xz`, `zip`, `tar`
- Checksums: `md5sum`, `sha256sum`, `sha512sum`

#### Category 3: Process Management
- Process control: `kill`, `killall`, `pkill`, `nice`, `renice`
- Process info: `ps`, `pstree`, `pgrep`, `pidof`, `top`
- Job control: `jobs`, `fg`, `bg`, `wait`
- Scheduling: `cron`, `at`, `systemctl`

#### Category 4: Networking
- Basic: `ping`, `traceroute`, `ifconfig`, `ip`, `route`
- Scanning: `nmap`, `masscan`, `arp-scan`
- Monitoring: `netstat`, `ss`, `tcpdump`, `wireshark`
- Services: `curl`, `wget`, `ssh`, `scp`, `ftp`

#### Category 5: System Administration
- System info: `uname`, `uptime`, `free`, `df`, `lscpu`
- User management: `useradd`, `userdel`, `passwd`, `su`, `sudo`
- Package management: `apt`, `yum`, `pacman`, `pip`, `npm`
- System control: `shutdown`, `reboot`, `systemctl`

#### Category 6: Text Processing
- Editors: `vi`, `vim`, `nano`, `emacs`
- Text tools: `expand`, `rev`, `shuf`, `column`

#### Category 7: Development Tools
- Version control: `svn`, `hg`, `bzr`, `cvs`
- Build systems: `make`, `cmake`, `gradle`, `maven`
- Compilers: `gcc`, `clang`, `rustc`, `javac`
- Code analysis: `pylint`, `eslint`, `cppcheck`
- Testing: `pytest`, `mocha`, `jest`

#### Category 8: Database Operations
- SQL: `mysql`, `psql`, `sqlite3`
- NoSQL: `mongo`, `redis-cli`, `cassandra-cli`

#### Category 9: Security & Encryption
- Security tools: `metasploit`, `sqlmap`, `nmap`, `hashcat`, `john`
- Encryption: `gpg`, `ssh-keygen`
- Kali tools: Install with `void-install-kali`

#### Category 10: System Utilities
- Date/time: `date`, `cal`, `timedatectl`

#### Category 11: Void OS Specific
- System: `void-info`, `void-status`, `void-health`
- Configuration: `void-config`, `void-theme`, `void-plugins`
- Help: `void-help`, `void-docs`, `void-tutorial`

#### Category 12: Multimedia
- Image: `convert`, `identify`, `ffmpeg`
- Audio: `sox`, `mpg123`, `flac`
- Video: `ffmpeg`, `vlc`, `mplayer`

#### Category 13: Archive & Compression
- Compression: `lzma`, `zstd`, `lz4`, `brotli`
- Archives: `ar`, `cpio`, `pax`

#### Category 14: Miscellaneous
- Fun: `fortune`, `cowsay`, `figlet`, `banner`
- Random: `random`, `dice`, `coin`, `shuffle`

### Void OS Styled Commands

Void OS includes unique command names with a "Void" aesthetic:

```bash
pulse          # List files (ls)
warp           # Change directory (cd)
vanish         # Clear terminal (clear)
echoes         # Display file (cat)
shroud         # Create file (touch)
transmute       # Move/rename (mv)
veil           # Show permissions
seal           # Change permissions (chmod)
scavenge       # Find files
marrow         # Show file contents with line numbers
dissect        # Analyze file
entropy        # System monitoring
tether         # Git command wrapper
anchor         # Clone repository
sync           # Pull changes
transmit       # Push changes
summon         # Execute Python script
forgepy        # Python REPL
multitool      # Utility panel
```

---

## Advanced Features

### Plugin System

```bash
# List plugins
forge list

# Install plugin
forge install /path/to/plugin.so

# Plugins auto-load from ~/.void/plugins/
```

### Project Awareness

Void OS automatically detects project types:

```bash
# Shows project information
context

# Detects:
# - Git repositories
# - Python projects (requirements.txt)
# - Node.js projects (package.json)
# - Docker projects (Dockerfile)
# - Environment files (.env)
```

### Security Tools

```bash
# Security check
startup-check

# Permission audit
perm-audit /path/to/dir

# Network monitoring
net-watch
```

### System Monitoring

```bash
# Live system stats
entropy

# System information
vitals

# Process monitoring
pulse-graph
```

### Developer Utilities

```bash
# Multitool panel
multitool

# Available tools:
# - hash (MD5, SHA256)
# - base64 (encode/decode)
# - json (validate/format)
# - uuid (generate)
# - timestamp (convert)
```

---

## Troubleshooting

### Network Issues

**Problem**: Cannot connect to WiFi

```bash
# Check WiFi status
wifi-status

# Enable WiFi
wifi-on

# Scan for networks
wifi-scan

# Try connecting again
wifi-connect SSID password

# Check NetworkManager
systemctl status NetworkManager
```

**Problem**: No Internet connectivity

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

### Kali Tools Installation Issues

**Problem**: "git: command not found"

```bash
# Install dependencies
sudo ./scripts/install_kali_dependencies.sh
```

**Problem**: "Failed to clone repository"

```bash
# Check network
net-test

# Verify git is installed
which git
git --version

# Try manual clone
git clone https://github.com/user/repo.git
```

**Problem**: Tool installed but not found

```bash
# Check installation
ls ~/.void/packages/bin/

# Check PATH
echo $PATH

# Add to PATH manually
export PATH="$HOME/.void/packages/bin:$PATH"

# Add to ~/.voidrc for persistence
echo 'export PATH="$HOME/.void/packages/bin:$PATH"' >> ~/.voidrc
```

### Command Not Found

**Problem**: Command not recognized

```bash
# Check if command exists
void-help all | grep command_name

# Check command type
type command_name

# Search for similar commands
void-help <category>
```

### Build Issues

**Problem**: Compilation errors

```bash
# Clean and rebuild
make clean
make

# Check dependencies
gcc --version
make --version

# Install build tools
sudo apt install build-essential  # Debian/Ubuntu
sudo apk add gcc make              # Alpine
```

---

## Quick Reference

### Essential Commands

```bash
# Navigation
pulse                  # List files
warp /path            # Change directory
pwd                    # Current directory

# File operations
echoes file.txt       # View file
shroud file.txt       # Create file
transmute old new     # Move/rename

# System
void-info             # System info
entropy               # System monitor
void-help             # Help system

# Network
wifi-scan             # Scan WiFi
wifi-connect SSID pass # Connect
net-test              # Test Internet

# Kali tools
void-install-kali list    # List tools
void-install-kali sqlmap  # Install tool
sqlmap -u target          # Use tool
```

### Configuration Files

- `~/.voidrc` - Void OS configuration
- `~/.void/plugins/` - Plugin directory
- `~/.void/packages/` - Installed packages
- `~/.void_logs` - Log files

### Getting Help

```bash
void-help              # Main help
void-help all          # All commands
void-help <category>   # Category commands
void-help <command>    # Command help
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

## Examples

### Example 1: Basic File Operations

```bash
# Create and edit file
shroud test.txt
echoes test.txt

# List files
pulse

# Change permissions
seal test.txt 755
veil test.txt
```

### Example 2: Network Setup

```bash
# Setup network tools
sudo ./scripts/setup_network.sh

# Connect to WiFi
wifi-scan
wifi-connect MyNetwork password
wifi-status

# Test connectivity
net-test
```

### Example 3: Installing and Using Kali Tools

```bash
# Install dependencies
sudo ./scripts/install_kali_dependencies.sh

# Install SQLMap
void-install-kali sqlmap

# Use SQLMap
sqlmap -u http://target.com/page?id=1 --dbs
```

### Example 4: Git Workflow

```bash
# Clone repository
anchor https://github.com/user/repo.git

# Check status
status

# Commit changes
commit "Updated code"

# Push changes
transmit
```

### Example 5: System Monitoring

```bash
# Monitor system
entropy

# Check processes
ps
top

# Monitor network
net-watch
throb
```

---

## Additional Resources

### Documentation Files

- `README.md` - Basic overview
- `BOOTABLE_GUIDE.md` - Creating bootable USB
- `KALI_TOOLS_IMPLEMENTATION.md` - Kali tools guide
- `KALI_DEPENDENCIES.md` - Dependency installation
- `NETWORK_WIFI_GUIDE.md` - Network setup
- `COMPLETE_TUTORIAL.md` - This file

### Commands for Information

```bash
void-info              # System information
void-status            # Current status
void-version           # Version information
void-credits           # Credits
void-docs              # Documentation
void-examples          # Examples
void-tutorial          # Tutorial
void-cheatsheet        # Cheat sheet
void-shortcuts         # Keyboard shortcuts
```

---

## Support

### Getting Help

1. Use `void-help` command
2. Check documentation files
3. Review error messages (Void-style messages)
4. Check logs in `~/.void_logs`

### Common Issues

- **Network not working**: Run `sudo ./scripts/setup_network.sh`
- **Tools not installing**: Install dependencies with `sudo ./scripts/install_kali_dependencies.sh`
- **Command not found**: Use `void-help all` to see available commands
- **Permission denied**: Some commands require root (use `sudo`)

---

## Conclusion

Void OS Elite provides a comprehensive, developer-focused CLI environment with:

- ✅ 1,000+ built-in commands
- ✅ Full network and WiFi management
- ✅ Kali tools integration
- ✅ Native Git operations
- ✅ Developer utilities
- ✅ Security tools
- ✅ Plugin architecture
- ✅ Complete documentation

**Enjoy using Void OS Elite!**

For help, use: `void-help`

---

**End of Complete Tutorial**

