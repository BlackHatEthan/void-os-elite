# Void OS Elite

A high-performance, developer-grade, security-aware CLI operating system written in C and Assembly.

[![Version](https://img.shields.io/badge/version-1.0-blue.svg)](https://github.com/yourusername/void-os-elite)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)](https://github.com/yourusername/void-os-elite)

## 🚀 Features

- **1,000+ Commands** - Comprehensive command set organized into 14 categories
- **Native Implementation** - Pure C/Assembly, no external dependencies
- **Network & WiFi Management** - Full internet connectivity and WiFi support
- **Kali Tools Integration** - Install and use Kali Linux Top 10 security tools
- **Developer-Focused** - Git integration, project awareness, developer utilities
- **Extensible** - Plugin architecture for customization
- **Bootable** - Create bootable USB or ISO files

## 📦 Quick Start

### Build

```bash
git clone https://github.com/yourusername/void-os-elite.git
cd void-os-elite/void
make
./void
```

### Get Help

```bash
void-help              # Main help menu
void-help all          # All commands
void-help <category>   # Category commands
```

## 📚 Documentation

- **[COMPLETE_TUTORIAL.md](void/COMPLETE_TUTORIAL.md)** - Complete user guide and tutorial
- **[QUICK_START.md](void/QUICK_START.md)** - 5-minute quick start guide
- **[USER_GUIDE.md](void/USER_GUIDE.md)** - Complete user guide
- **[ISO_CREATION_GUIDE.md](void/ISO_CREATION_GUIDE.md)** - Create bootable ISO files
- **[BOOTABLE_GUIDE.md](void/BOOTABLE_GUIDE.md)** - Bootable USB creation
- **[DOCUMENTATION_INDEX.md](void/DOCUMENTATION_INDEX.md)** - Documentation navigation

## 🛠️ Installation

### From Source

```bash
cd void
make
sudo make install  # Optional
```

### Create Bootable ISO

```bash
cd scripts
sudo ./create_void_iso.sh
```

### Setup Network & WiFi

```bash
sudo ./scripts/setup_network.sh
wifi-connect MyNetwork password
```

### Install Kali Tools

```bash
sudo ./scripts/install_kali_dependencies.sh
void-install-kali all
```

## 📋 Command Categories

1. **Core Shell Commands** - Basic operations, navigation, text processing
2. **Void OS Styled** - Void-themed command names
3. **Filesystem Operations** - File and directory management
4. **Process Management** - Process control and monitoring
5. **Networking** - Network tools and utilities
6. **WiFi Management** - WiFi and network configuration
7. **System Administration** - System management
8. **Text Processing** - Text editors and manipulation
9. **Development Tools** - Compilers, build systems, testing
10. **Database Operations** - Database clients
11. **Security & Encryption** - Security tools and encryption
12. **System Utilities** - Date, time, calendar
13. **Void OS Specific** - Void OS management
14. **Multimedia & Archives** - Media and compression tools

## 🎯 Key Commands

```bash
# Help
void-help              # Comprehensive help system

# System
void-info              # System information
void-status            # Current status
entropy                # System monitoring

# Network
wifi-scan              # Scan WiFi networks
wifi-connect SSID pass # Connect to WiFi
net-test               # Test Internet connectivity

# Kali Tools
void-install-kali list    # List available tools
void-install-kali <tool>  # Install security tool
```

## 🏗️ Architecture

- **Language**: C (C11) with Assembly optimizations
- **Build System**: Makefile
- **Dependencies**: None (static binary available)
- **Platform**: Linux (x86_64, ARM, ARM64)
- **License**: MIT

## 📖 Examples

### Basic Usage

```bash
# Navigation
pulse                  # List files
warp /path            # Change directory

# Files
echoes file.txt       # View file
shroud file.txt       # Create file

# Git
status                # Git status
commit "message"      # Commit changes
```

### Network Setup

```bash
# Setup network tools
sudo ./scripts/setup_network.sh

# Connect to WiFi
wifi-scan
wifi-connect MyNetwork password
net-test
```

### Security Tools

```bash
# Install Kali tools
void-install-kali sqlmap
void-install-kali nmap

# Use tools
sqlmap -u http://target.com
nmap -sS target.com
```

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Inspired by modern shell environments
- Built with developer productivity in mind
- Security tools integration for security professionals

## 📞 Support

- **Documentation**: See `void/DOCUMENTATION_INDEX.md`
- **Help Command**: `void-help`
- **Issues**: Open an issue on GitHub

---

**Void OS Elite** - Operating System Wrapper v1.0

For complete documentation, see [void/COMPLETE_TUTORIAL.md](void/COMPLETE_TUTORIAL.md)

