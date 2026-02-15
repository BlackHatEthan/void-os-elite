#!/bin/bash

# Void OS - Kali Tools Dependencies Installer
# Installs all required dependencies for Kali Top 10 tools

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
GREY='\033[0;90m'
WHITE='\033[0;37m'
RESET='\033[0m'

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
    echo -e "${RED}Error: This script must be run as root (use sudo)${RESET}"
    exit 1
fi

echo -e "${WHITE}Void OS - Kali Tools Dependencies Installer${RESET}"
echo -e "${GREY}═══════════════════════════════════════════════${RESET}"
echo ""

# Detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        VERSION=$VERSION_ID
    elif [ -f /etc/redhat-release ]; then
        DISTRO="rhel"
    elif [ -f /etc/debian_version ]; then
        DISTRO="debian"
    elif [ -f /etc/alpine-release ]; then
        DISTRO="alpine"
    else
        DISTRO="unknown"
    fi
}

# Install packages based on distribution
install_packages() {
    case $DISTRO in
        alpine)
            echo -e "${GREY}Detected: Alpine Linux${RESET}"
            echo -e "${GREY}Installing packages with apk...${RESET}"
            apk update
            apk add --no-cache \
                git \
                gcc \
                make \
                musl-dev \
                python3 \
                python3-dev \
                py3-pip \
                ruby \
                ruby-dev \
                ruby-bundler \
                perl \
                perl-dev \
                go \
                rust \
                cargo \
                openssl-dev \
                libssh-dev \
                opencl-headers \
                opencl-dev \
                linux-headers \
                bash
            ;;
        debian|ubuntu)
            echo -e "${GREY}Detected: Debian/Ubuntu${RESET}"
            echo -e "${GREY}Installing packages with apt...${RESET}"
            apt-get update
            apt-get install -y \
                git \
                gcc \
                make \
                build-essential \
                python3 \
                python3-dev \
                python3-pip \
                ruby \
                ruby-dev \
                bundler \
                perl \
                libperl-dev \
                golang-go \
                rustc \
                cargo \
                libssl-dev \
                libssh-dev \
                opencl-headers \
                ocl-icd-opencl-dev \
                pkg-config
            ;;
        fedora|rhel|centos)
            echo -e "${GREY}Detected: Fedora/RHEL/CentOS${RESET}"
            if command -v dnf &> /dev/null; then
                echo -e "${GREY}Installing packages with dnf...${RESET}"
                dnf install -y \
                    git \
                    gcc \
                    make \
                    gcc-c++ \
                    python3 \
                    python3-devel \
                    python3-pip \
                    ruby \
                    ruby-devel \
                    rubygem-bundler \
                    perl \
                    perl-devel \
                    golang \
                    rust \
                    cargo \
                    openssl-devel \
                    libssh-devel \
                    opencl-headers \
                    ocl-icd-devel \
                    pkgconfig
            else
                echo -e "${GREY}Installing packages with yum...${RESET}"
                yum install -y \
                    git \
                    gcc \
                    make \
                    gcc-c++ \
                    python3 \
                    python3-devel \
                    python3-pip \
                    ruby \
                    ruby-devel \
                    rubygem-bundler \
                    perl \
                    perl-devel \
                    golang \
                    rust \
                    cargo \
                    openssl-devel \
                    libssh-devel \
                    opencl-headers \
                    ocl-icd-devel \
                    pkgconfig
            fi
            ;;
        arch|manjaro)
            echo -e "${GREY}Detected: Arch Linux/Manjaro${RESET}"
            echo -e "${GREY}Installing packages with pacman...${RESET}"
            pacman -Sy --noconfirm \
                git \
                gcc \
                make \
                base-devel \
                python \
                python-pip \
                ruby \
                ruby-bundler \
                perl \
                go \
                rust \
                cargo \
                openssl \
                libssh \
                opencl-headers \
                ocl-icd \
                pkgconf
            ;;
        *)
            echo -e "${RED}Error: Unsupported Linux distribution: $DISTRO${RESET}"
            echo -e "${YELLOW}Please install the following packages manually:${RESET}"
            echo "  - git"
            echo "  - gcc, make"
            echo "  - python3"
            echo "  - ruby, bundler"
            echo "  - perl"
            echo "  - go"
            echo "  - rust, cargo"
            echo "  - libssl-dev / openssl-devel"
            echo "  - libssh-dev / libssh-devel"
            echo "  - opencl-headers"
            exit 1
            ;;
    esac
}

# Verify installations
verify_installations() {
    echo ""
    echo -e "${WHITE}Verifying installations...${RESET}"
    echo -e "${GREY}─────────────────────────────────────${RESET}"
    
    local all_ok=true
    
    # Check git
    if command -v git &> /dev/null; then
        echo -e "${GREEN}✓ git${RESET} - $(git --version | cut -d' ' -f3)"
    else
        echo -e "${RED}✗ git - NOT FOUND${RESET}"
        all_ok=false
    fi
    
    # Check gcc
    if command -v gcc &> /dev/null; then
        echo -e "${GREEN}✓ gcc${RESET} - $(gcc --version | head -n1 | cut -d' ' -f3)"
    else
        echo -e "${RED}✗ gcc - NOT FOUND${RESET}"
        all_ok=false
    fi
    
    # Check make
    if command -v make &> /dev/null; then
        echo -e "${GREEN}✓ make${RESET} - $(make --version | head -n1 | cut -d' ' -f3)"
    else
        echo -e "${RED}✗ make - NOT FOUND${RESET}"
        all_ok=false
    fi
    
    # Check python3
    if command -v python3 &> /dev/null; then
        echo -e "${GREEN}✓ python3${RESET} - $(python3 --version | cut -d' ' -f2)"
    else
        echo -e "${RED}✗ python3 - NOT FOUND${RESET}"
        all_ok=false
    fi
    
    # Check ruby
    if command -v ruby &> /dev/null; then
        echo -e "${GREEN}✓ ruby${RESET} - $(ruby --version | cut -d' ' -f2)"
    else
        echo -e "${RED}✗ ruby - NOT FOUND${RESET}"
        all_ok=false
    fi
    
    # Check bundler
    if command -v bundle &> /dev/null || command -v bundler &> /dev/null; then
        echo -e "${GREEN}✓ bundler${RESET} - $(bundle --version 2>/dev/null || bundler --version 2>/dev/null | cut -d' ' -f3)"
    else
        echo -e "${YELLOW}⚠ bundler - NOT FOUND (may need: gem install bundler)${RESET}"
    fi
    
    # Check perl
    if command -v perl &> /dev/null; then
        echo -e "${GREEN}✓ perl${RESET} - $(perl --version | grep "This is perl" | cut -d' ' -f4)"
    else
        echo -e "${RED}✗ perl - NOT FOUND${RESET}"
        all_ok=false
    fi
    
    # Check go
    if command -v go &> /dev/null; then
        echo -e "${GREEN}✓ go${RESET} - $(go version | cut -d' ' -f3)"
    else
        echo -e "${RED}✗ go - NOT FOUND${RESET}"
        all_ok=false
    fi
    
    # Check rust
    if command -v rustc &> /dev/null; then
        echo -e "${GREEN}✓ rust${RESET} - $(rustc --version | cut -d' ' -f2)"
    else
        echo -e "${RED}✗ rust - NOT FOUND${RESET}"
        all_ok=false
    fi
    
    # Check cargo
    if command -v cargo &> /dev/null; then
        echo -e "${GREEN}✓ cargo${RESET} - $(cargo --version | cut -d' ' -f2)"
    else
        echo -e "${RED}✗ cargo - NOT FOUND${RESET}"
        all_ok=false
    fi
    
    echo ""
    if [ "$all_ok" = true ]; then
        echo -e "${GREEN}All essential dependencies installed successfully!${RESET}"
        return 0
    else
        echo -e "${YELLOW}Some dependencies may be missing. Please check above.${RESET}"
        return 1
    fi
}

# Install bundler if missing
install_bundler() {
    if ! command -v bundle &> /dev/null && ! command -v bundler &> /dev/null; then
        echo -e "${GREY}Installing bundler...${RESET}"
        if command -v gem &> /dev/null; then
            gem install bundler
        else
            echo -e "${YELLOW}Warning: gem not found. Install Ruby development tools.${RESET}"
        fi
    fi
}

# Main execution
main() {
    detect_distro
    echo -e "${WHITE}Distribution: ${DISTRO}${RESET}"
    echo ""
    
    echo -e "${GREY}Installing dependencies...${RESET}"
    echo -e "${GREY}This may take a few minutes.${RESET}"
    echo ""
    
    install_packages
    
    echo ""
    echo -e "${GREEN}Package installation complete!${RESET}"
    
    # Install bundler if needed
    install_bundler
    
    # Verify installations
    verify_installations
    
    echo ""
    echo -e "${WHITE}Next steps:${RESET}"
    echo -e "${GREY}1. You can now install Kali tools with: void-install-kali <tool_name>${RESET}"
    echo -e "${GREY}2. Or install all tools: void-install-kali all${RESET}"
    echo ""
}

# Run main function
main

