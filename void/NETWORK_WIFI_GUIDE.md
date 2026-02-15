# Void OS - Network & WiFi Management Guide

## Overview

Void OS includes comprehensive network and WiFi management capabilities, allowing you to configure internet connectivity, connect to WiFi networks, and manage network interfaces directly from the shell.

## Quick Start

### 1. Setup Network Tools (One-Time, Requires Root)

```bash
sudo ./scripts/setup_network.sh
```

This installs:
- NetworkManager (for WiFi management)
- Wireless tools (iw, iwlist)
- DHCP client (dhcpcd/dhclient)
- Network utilities (ip, ifconfig, etc.)

### 2. Check Network Status

```bash
net-status          # Show all network interfaces
wifi-status        # Show WiFi connection status
net-test           # Test Internet connectivity
```

### 3. Connect to WiFi

```bash
wifi-scan                    # Scan for available networks
wifi-connect MyNetwork pass  # Connect to WiFi network
wifi-status                  # Check connection status
```

---

## WiFi Commands

### `wifi-scan` / `wifi-list`
Scan for available WiFi networks.

```bash
wifi-scan
```

**Output:**
- Lists all available WiFi networks
- Shows signal strength
- Shows security type (WPA, WPA2, Open, etc.)

**Requirements:**
- NetworkManager or wireless-tools installed
- WiFi interface enabled

### `wifi-connect <SSID> [password]`
Connect to a WiFi network.

```bash
wifi-connect MyNetwork
wifi-connect MyNetwork mypassword
```

**Examples:**
```bash
# Open network (no password)
wifi-connect PublicWiFi

# WPA/WPA2 network
wifi-connect HomeNetwork mypassword123

# Network with spaces in name
wifi-connect "My Home Network" password
```

**Requirements:**
- NetworkManager installed and running
- WiFi interface enabled
- Root/sudo access (for some operations)

### `wifi-disconnect`
Disconnect from current WiFi network.

```bash
wifi-disconnect
```

### `wifi-status`
Show current WiFi connection status.

```bash
wifi-status
```

**Shows:**
- Connected interface name
- SSID (network name)
- IP address
- Connection status

### `wifi-on` / `wifi-off`
Enable or disable WiFi radio.

```bash
wifi-on    # Enable WiFi
wifi-off   # Disable WiFi
```

---

## Network Interface Management

### `net-status`
Show status of all network interfaces.

```bash
net-status
```

**Shows:**
- Interface names (eth0, wlan0, etc.)
- IP addresses (IPv4 and IPv6)
- Interface status

### `net-up <interface>`
Bring up a network interface.

```bash
net-up eth0
net-up wlan0
```

### `net-down <interface>`
Bring down a network interface.

```bash
net-down eth0
```

### `net-restart <interface>`
Restart a network interface (down then up).

```bash
net-restart eth0
```

### `net-dhcp <interface>`
Configure interface to use DHCP (automatic IP).

```bash
net-dhcp eth0
net-dhcp wlan0
```

**What it does:**
- Requests IP address from DHCP server
- Configures default gateway
- Sets DNS servers

### `net-static <interface> <IP> <netmask> [gateway]`
Configure static IP address.

```bash
net-static eth0 192.168.1.100 255.255.255.0 192.168.1.1
```

**Example:**
```bash
# Set static IP without gateway
net-static eth0 192.168.1.100 255.255.255.0

# Set static IP with gateway
net-static eth0 192.168.1.100 255.255.255.0 192.168.1.1
```

---

## Connectivity Testing

### `net-test` / `net-check`
Test Internet connectivity.

```bash
net-test
```

**Tests:**
- DNS resolution (google.com)
- HTTP connectivity (www.google.com)
- Ping connectivity (8.8.8.8)

**Output:**
```
Testing Internet Connectivity...
─────────────────────────────────────
Testing DNS... OK
Testing HTTP... OK
Testing ping... OK
```

---

## DNS Management

### `dns-get`
Show current DNS servers.

```bash
dns-get
```

**Output:**
```
Current DNS Servers:
─────────────────────────────────────
  8.8.8.8
  8.8.4.4
```

### `dns-set <dns1> [dns2]`
Set DNS servers (requires root).

```bash
sudo dns-set 8.8.8.8 8.8.4.4
sudo dns-set 1.1.1.1
```

**Popular DNS servers:**
- Google: `8.8.8.8`, `8.8.4.4`
- Cloudflare: `1.1.1.1`, `1.0.0.1`
- Quad9: `9.9.9.9`, `149.112.112.112`

### `dns-test [hostname]`
Test DNS resolution.

```bash
dns-test google.com
dns-test github.com
```

---

## Common Workflows

### Connecting to WiFi for the First Time

```bash
# 1. Check if WiFi is enabled
wifi-status

# 2. Scan for networks
wifi-scan

# 3. Connect to network
wifi-connect MyNetwork mypassword

# 4. Verify connection
wifi-status
net-test
```

### Setting Up Static IP

```bash
# 1. Bring down interface
net-down eth0

# 2. Configure static IP
net-static eth0 192.168.1.100 255.255.255.0 192.168.1.1

# 3. Bring up interface
net-up eth0

# 4. Verify
net-status
net-test
```

### Troubleshooting Network Issues

```bash
# 1. Check interface status
net-status

# 2. Test connectivity
net-test

# 3. Check DNS
dns-get
dns-test google.com

# 4. Restart interface
net-restart eth0

# 5. Reconfigure DHCP
net-dhcp eth0
```

---

## Requirements

### System Packages

The network commands require these system packages:

- **NetworkManager** - WiFi and network management
- **wireless-tools** - WiFi scanning (iw, iwlist)
- **iw** - Modern wireless tools
- **wpa_supplicant** - WPA/WPA2 support
- **dhcpcd** or **dhclient** - DHCP client
- **iproute2** - Modern network tools (ip command)
- **net-tools** - Legacy network tools (ifconfig)

### Installation

Install all required packages:

```bash
sudo ./scripts/setup_network.sh
```

Or install manually:

**Debian/Ubuntu:**
```bash
sudo apt install network-manager wireless-tools iw wpasupplicant dhcpcd5 iproute2 net-tools
```

**Alpine Linux:**
```bash
sudo apk add network-manager wireless-tools iw wpa_supplicant dhcpcd iproute2 net-tools
```

**Fedora/RHEL:**
```bash
sudo dnf install NetworkManager wireless-tools iw wpa_supplicant dhcpcd iproute net-tools
```

---

## Integration with Kali Tools

Once network is configured, you can download and install Kali tools:

```bash
# 1. Ensure network is working
net-test

# 2. Install Kali tools dependencies
sudo ./scripts/install_kali_dependencies.sh

# 3. Install Kali tools
void-install-kali all
```

---

## Troubleshooting

### "NetworkManager not found"
```bash
sudo ./scripts/setup_network.sh
```

### "WiFi scanning failed"
- Ensure WiFi interface is enabled: `wifi-on`
- Check if NetworkManager is running: `systemctl status NetworkManager`
- Try: `sudo iw dev wlan0 scan` manually

### "Cannot connect to WiFi"
- Verify SSID is correct: `wifi-scan`
- Check password is correct
- Ensure NetworkManager is running
- Check interface: `wifi-status`

### "No Internet connectivity"
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

### "Permission denied"
Some commands require root access:
```bash
sudo wifi-connect MyNetwork password
sudo net-static eth0 192.168.1.100 255.255.255.0
sudo dns-set 8.8.8.8
```

---

## Advanced Usage

### Using with NetworkManager CLI

Void OS commands wrap NetworkManager. You can also use `nmcli` directly:

```bash
# List networks
nmcli device wifi list

# Connect
nmcli device wifi connect MyNetwork password mypass

# Show status
nmcli device status
```

### Manual Network Configuration

For advanced users, you can configure networks manually:

```bash
# Using ip command
ip addr add 192.168.1.100/24 dev eth0
ip route add default via 192.168.1.1

# Using ifconfig (legacy)
ifconfig eth0 192.168.1.100 netmask 255.255.255.0
route add default gw 192.168.1.1
```

---

## Security Notes

- WiFi passwords are passed as command arguments (visible in process list)
- Use `wifi-connect` in private terminals
- Consider using NetworkManager keyring for password storage
- Static IP configuration requires root access

---

## Next Steps

1. ✅ Setup network: `sudo ./scripts/setup_network.sh`
2. ✅ Connect to WiFi: `wifi-connect <SSID> <password>`
3. ✅ Test connectivity: `net-test`
4. ✅ Install Kali tools: `void-install-kali all`

---

**Status**: ✅ Complete - Full network and WiFi management integrated

