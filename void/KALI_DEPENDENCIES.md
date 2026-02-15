# Kali Tools Dependencies Installation Guide

## Overview

To install and use the Kali Top 10 security tools in Void OS, you need several dependencies including Git, build tools, and language runtimes.

## Quick Start

### 1. Check Current Status (No Root Required)

```bash
./scripts/check_kali_dependencies.sh
```

This will show you which dependencies are installed and which are missing.

### 2. Install All Dependencies (Requires Root)

```bash
sudo ./scripts/install_kali_dependencies.sh
```

This script will:
- Detect your Linux distribution
- Install all required packages using the appropriate package manager
- Verify installations
- Show you what's ready

## Required Dependencies

### Essential Tools

| Tool | Purpose | Required For |
|------|---------|--------------|
| **git** | Version control | Cloning repositories |
| **gcc** | C compiler | Compiling tools (John, Hashcat, Hydra, Nmap) |
| **make** | Build system | Compiling tools |
| **python3** | Python interpreter | SQLMap |
| **ruby** | Ruby interpreter | Metasploit, WPScan |
| **bundler** | Ruby dependency manager | Metasploit, WPScan |
| **perl** | Perl interpreter | Nikto |
| **go** | Go compiler | Gobuster |
| **rust** | Rust compiler | Feroxbuster |
| **cargo** | Rust package manager | Feroxbuster |

### Development Libraries

| Library | Purpose | Required For |
|---------|---------|--------------|
| **libssl-dev** | SSL/TLS development | Various tools |
| **libssh-dev** | SSH development | Hydra |
| **opencl-headers** | OpenCL headers | Hashcat |

## Supported Distributions

The installer script supports:

- ✅ **Alpine Linux** (apk)
- ✅ **Debian/Ubuntu** (apt)
- ✅ **Fedora/RHEL/CentOS** (dnf/yum)
- ✅ **Arch Linux/Manjaro** (pacman)

## Manual Installation

If your distribution isn't supported, install these packages manually:

### Alpine Linux
```bash
apk add git gcc make python3 ruby ruby-bundler perl go rust cargo \
         openssl-dev libssh-dev opencl-headers
```

### Debian/Ubuntu
```bash
apt-get install git gcc make build-essential python3 python3-pip \
                ruby ruby-dev bundler perl libperl-dev golang-go \
                rustc cargo libssl-dev libssh-dev opencl-headers
```

### Fedora/RHEL/CentOS
```bash
dnf install git gcc make gcc-c++ python3 python3-devel python3-pip \
            ruby ruby-devel rubygem-bundler perl perl-devel golang \
            rust cargo openssl-devel libssh-devel opencl-headers
```

### Arch Linux
```bash
pacman -S git gcc make base-devel python python-pip ruby ruby-bundler \
          perl go rust cargo openssl libssh opencl-headers
```

## Verification

After installation, verify everything is working:

```bash
# Check dependencies
./scripts/check_kali_dependencies.sh

# Test git
git --version

# Test python3
python3 --version

# Test ruby
ruby --version

# Test go
go version

# Test rust
rustc --version
cargo --version
```

## Installing Kali Tools

Once dependencies are installed, you can install Kali tools:

```bash
# List available tools
void-install-kali list

# Install a specific tool
void-install-kali metasploit
void-install-kali sqlmap
void-install-kali nmap

# Install all tools
void-install-kali all
```

## Troubleshooting

### "git: command not found"
- Install git: `sudo ./scripts/install_kali_dependencies.sh`
- Or manually: `sudo apt install git` (Debian/Ubuntu)

### "python3: command not found"
- Install python3: `sudo apt install python3` (Debian/Ubuntu)
- Or use the installer script

### "bundle: command not found"
- Install bundler: `gem install bundler`
- Or: `sudo apt install bundler` (Debian/Ubuntu)

### "go: command not found"
- Install go: `sudo apt install golang-go` (Debian/Ubuntu)
- Or use the installer script

### "cargo: command not found"
- Install rust and cargo: `sudo apt install rust cargo` (Debian/Ubuntu)
- Or use the installer script

### Compilation Errors
- Make sure you have `gcc`, `make`, and development libraries installed
- Check that `libssl-dev` and `libssh-dev` are installed
- For Hashcat, ensure `opencl-headers` is installed

## Disk Space Requirements

Installing all dependencies requires approximately:
- **Alpine Linux**: ~200-300MB
- **Debian/Ubuntu**: ~500-800MB
- **Fedora/RHEL**: ~600-900MB

Installing all Kali tools requires additional:
- **Metasploit**: ~500MB+
- **Other tools**: ~50-200MB each
- **Total**: ~2-3GB for all tools

## Network Requirements

- **Internet connection** required for:
  - Installing packages from repositories
  - Cloning GitHub repositories
  - Downloading tool dependencies

## Next Steps

1. ✅ Install dependencies: `sudo ./scripts/install_kali_dependencies.sh`
2. ✅ Verify installation: `./scripts/check_kali_dependencies.sh`
3. ✅ Install Kali tools: `void-install-kali <tool_name>`
4. ✅ Use the tools: `sqlmap -u http://target.com`

---

**Note**: The installer script requires root/sudo access to install system packages. The checker script can be run without root access.

