# Void OS Elite – Platform Support (Linux, macOS, Windows)

All tools are implemented to work across **Linux**, **macOS**, and **Windows** (via WSL) where the platform allows.

---

## Summary

| Platform | How to run | WiFi / network | System info | Process list | Notes |
|----------|------------|----------------|------------|--------------|--------|
| **Linux** | Build with `make`, run `./void` | Native nl80211 scan; connect via nmcli/wpa_supplicant if present | `/proc`, sysctl | `/proc`, native | Full support. |
| **macOS** | Build with `make`, run `./void` | Native scan via `airport -s`; connect via system WiFi | sysctl, vm_stat, system_profiler | `ps`, netstat | Full support. |
| **Windows** | Use **WSL**, then build and run as on Linux | Same as Linux (WSL is Linux) | Same as Linux | Same as Linux | Run Void inside WSL; all Linux features apply. |

---

## Linux

- **WiFi:** Native scan (nl80211); connect uses NetworkManager/wpa_supplicant when available.
- **System:** uptime, free, vmstat, lscpu, lspci, lsusb, etc. use `/proc` and sysfs.
- **Processes:** `ps`/`jobs` use `/proc`.
- **Network:** net_watch uses `/proc/net/tcp`, `/proc/net/udp`.

## macOS

- **WiFi:** Native scan via built-in `airport -s` (no extra install). Connect uses system WiFi (no nmcli).
- **System:** uptime (sysctl `kern.boottime`), free (sysctl + vm_stat), lscpu (sysctl), lspci/lsusb (system_profiler).
- **Processes:** `ps` uses `ps -eo pid,comm`.
- **Network:** net_watch uses `netstat -an`.

## Windows

- **Run Void in WSL** (Windows Subsystem for Linux). Then you are effectively on Linux:
  - Use `install-windows.ps1` or `install-windows.bat` to install inside WSL, or
  - Open WSL (e.g. Ubuntu), clone the repo, run `./install-linux.sh`.
- All Linux behavior (WiFi scan, system info, process list, etc.) applies inside WSL.
- There is no native Windows build of Void; the design is POSIX (Linux/macOS).

---

## Command coverage by platform

| Category | Linux | macOS | Windows (WSL) |
|----------|-------|-------|----------------|
| Core shell (echo, cd, pwd, etc.) | ✅ | ✅ | ✅ |
| WiFi scan | ✅ native | ✅ native | ✅ (Linux) |
| WiFi connect/status | ✅ | ✅ (system) | ✅ (Linux) |
| System (uptime, free, lscpu, etc.) | ✅ | ✅ | ✅ (Linux) |
| Process (ps, jobs, etc.) | ✅ | ✅ | ✅ (Linux) |
| Network (ping, net-watch, etc.) | ✅ | ✅ | ✅ (Linux) |
| Filesystem, text, dev tools | ✅ | ✅ | ✅ (Linux) |
| Kali/specialized tools | ✅ when available | Stub/limited | ✅ in WSL when available |

---

## Building per platform

- **Linux:** `./install-linux.sh` or `make` in `void/`.
- **macOS:** `./install-macos.sh` or `make` in `void/` (Xcode Command Line Tools required).
- **Windows:** Install WSL, then run `./install-linux.sh` from the repo root inside WSL.

This completes cross-platform support for development and testing.
