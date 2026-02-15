# Void OS Elite - Quick Start Guide

## 🚀 5-Minute Setup

### Step 1: Build Void OS

```bash
cd void
make
./void
```

### Step 2: Setup Network (if using WiFi)

```bash
# Install network tools
sudo ./scripts/setup_network.sh

# Connect to WiFi
wifi-scan
wifi-connect YourNetwork password
net-test
```

### Step 3: Install Kali Tools (Optional)

```bash
# Install dependencies
sudo ./scripts/install_kali_dependencies.sh

# Install tools
void-install-kali sqlmap
void-install-kali nmap
```

### Step 4: Get Help

```bash
void-help              # Main help
void-help all          # All commands
void-help wifi         # WiFi commands
void-help security     # Security tools
```

---

## Essential Commands

```bash
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
void-help             # Help system

# Network
wifi-scan             # Scan WiFi
wifi-connect SSID pass # Connect
net-test              # Test Internet

# Kali Tools
void-install-kali list    # List tools
sqlmap -u target          # Use tool
```

---

## Full Documentation

See `COMPLETE_TUTORIAL.md` for comprehensive guide.

---

**Ready to use!** Type `void-help` for more information.

