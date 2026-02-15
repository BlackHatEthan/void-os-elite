#!/bin/bash

# Void OS - Network Setup Script
# Installs network management tools and configures basic networking

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

echo -e "${WHITE}Void OS - Network Setup${RESET}"
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

# Install network packages
install_network_packages() {
    case $DISTRO in
        alpine)
            echo -e "${GREY}Detected: Alpine Linux${RESET}"
            echo -e "${GREY}Installing network packages...${RESET}"
            apk update
            apk add --no-cache \
                network-manager \
                wireless-tools \
                iw \
                wpa_supplicant \
                dhcpcd \
                iproute2 \
                net-tools \
                curl \
                wget
            ;;
        debian|ubuntu)
            echo -e "${GREY}Detected: Debian/Ubuntu${RESET}"
            echo -e "${GREY}Installing network packages...${RESET}"
            apt-get update
            apt-get install -y \
                network-manager \
                wireless-tools \
                iw \
                wpasupplicant \
                dhcpcd5 \
                iproute2 \
                net-tools \
                curl \
                wget
            ;;
        fedora|rhel|centos)
            echo -e "${GREY}Detected: Fedora/RHEL/CentOS${RESET}"
            if command -v dnf &> /dev/null; then
                echo -e "${GREY}Installing network packages...${RESET}"
                dnf install -y \
                    NetworkManager \
                    wireless-tools \
                    iw \
                    wpa_supplicant \
                    dhcpcd \
                    iproute \
                    net-tools \
                    curl \
                    wget
            else
                echo -e "${GREY}Installing network packages...${RESET}"
                yum install -y \
                    NetworkManager \
                    wireless-tools \
                    iw \
                    wpa_supplicant \
                    dhcpcd \
                    iproute \
                    net-tools \
                    curl \
                    wget
            fi
            ;;
        arch|manjaro)
            echo -e "${GREY}Detected: Arch Linux/Manjaro${RESET}"
            echo -e "${GREY}Installing network packages...${RESET}"
            pacman -Sy --noconfirm \
                networkmanager \
                wireless_tools \
                iw \
                wpa_supplicant \
                dhcpcd \
                iproute2 \
                net-tools \
                curl \
                wget
            ;;
        *)
            echo -e "${RED}Error: Unsupported Linux distribution: $DISTRO${RESET}"
            echo -e "${YELLOW}Please install the following packages manually:${RESET}"
            echo "  - network-manager / NetworkManager"
            echo "  - wireless-tools"
            echo "  - iw"
            echo "  - wpa_supplicant"
            echo "  - dhcpcd"
            echo "  - iproute2 / iproute"
            echo "  - net-tools"
            exit 1
            ;;
    esac
}

# Enable NetworkManager
enable_networkmanager() {
    echo -e "${GREY}Enabling NetworkManager...${RESET}"
    
    if command -v systemctl &> /dev/null; then
        systemctl enable NetworkManager 2>/dev/null || systemctl enable network-manager 2>/dev/null || true
        systemctl start NetworkManager 2>/dev/null || systemctl start network-manager 2>/dev/null || true
        echo -e "${GREEN}NetworkManager enabled${RESET}"
    else
        echo -e "${YELLOW}Note: systemctl not found. Start NetworkManager manually.${RESET}"
    fi
}

# Verify installations
verify_network_tools() {
    echo ""
    echo -e "${WHITE}Verifying network tools...${RESET}"
    echo -e "${GREY}─────────────────────────────────────${RESET}"
    
    local all_ok=true
    
    # Check NetworkManager
    if command -v nmcli &> /dev/null; then
        echo -e "${GREEN}✓ NetworkManager (nmcli)${RESET}"
    else
        echo -e "${YELLOW}⚠ NetworkManager not found${RESET}"
    fi
    
    # Check iw
    if command -v iw &> /dev/null; then
        echo -e "${GREEN}✓ iw (wireless tools)${RESET}"
    else
        echo -e "${YELLOW}⚠ iw not found${RESET}"
    fi
    
    # Check ip
    if command -v ip &> /dev/null; then
        echo -e "${GREEN}✓ ip (iproute2)${RESET}"
    else
        echo -e "${YELLOW}⚠ ip not found${RESET}"
    fi
    
    # Check dhcpcd
    if command -v dhcpcd &> /dev/null || command -v dhclient &> /dev/null; then
        echo -e "${GREEN}✓ DHCP client${RESET}"
    else
        echo -e "${YELLOW}⚠ DHCP client not found${RESET}"
    fi
    
    # Check curl
    if command -v curl &> /dev/null; then
        echo -e "${GREEN}✓ curl${RESET}"
    else
        echo -e "${YELLOW}⚠ curl not found${RESET}"
    fi
    
    # Check wget
    if command -v wget &> /dev/null; then
        echo -e "${GREEN}✓ wget${RESET}"
    else
        echo -e "${YELLOW}⚠ wget not found${RESET}"
    fi
    
    echo ""
}

# Main execution
main() {
    detect_distro
    echo -e "${WHITE}Distribution: ${DISTRO}${RESET}"
    echo ""
    
    echo -e "${GREY}Installing network management tools...${RESET}"
    install_network_packages
    
    echo ""
    echo -e "${GREEN}Network packages installed!${RESET}"
    
    enable_networkmanager
    
    verify_network_tools
    
    echo ""
    echo -e "${WHITE}Network setup complete!${RESET}"
    echo ""
    echo -e "${GREY}Available Void OS network commands:${RESET}"
    echo -e "${WHITE}  wifi-scan${RESET}          - Scan for WiFi networks"
    echo -e "${WHITE}  wifi-connect <SSID> [pass]${RESET} - Connect to WiFi"
    echo -e "${WHITE}  wifi-status${RESET}        - Show WiFi status"
    echo -e "${WHITE}  net-status${RESET}         - Show network interfaces"
    echo -e "${WHITE}  net-test${RESET}           - Test Internet connectivity"
    echo -e "${WHITE}  dns-get${RESET}            - Show DNS servers"
    echo ""
}

# Run main function
main

