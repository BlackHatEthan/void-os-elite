# Void OS Elite – WiFi Architecture

## What “independent WiFi” means here

Void OS Elite is a **userspace shell**. It always runs on top of an operating system (Linux, macOS, or Windows via WSL). That OS has a **kernel** that owns the hardware, including the WiFi adapter.

- **We do not wrap host *tools*:** WiFi scanning and (where possible) connection are implemented **inside Void** using only the **kernel API** (e.g. Linux netlink/nl80211). Void does **not** depend on `iw`, `nmcli`, or `wpa_supplicant` being installed.
- **We still use the host *kernel*:** The WiFi radio is driven by the kernel and its driver. Void talks to the kernel via system calls (netlink on Linux). That is the only way a userspace program can use WiFi. There is no way for a normal application to have “WiFi without the host” — the host is the machine.

To have WiFi with **no** host at all, Void would need to be the **OS kernel** you boot (with its own drivers and network stack). That would be a different project (a full OS), not an extension of this shell.

## Current design

| Platform | Scan | Connect |
|----------|------|--------|
| **Linux** | Native via nl80211 netlink (no `iw`/`nmcli`) | Native for **open** networks; WPA/WPA2 still uses kernel + optional `wpa_supplicant` (or future native WPA in Void) |
| **macOS** | Native via system API where available; otherwise clear message | Uses system networking (no dependency on Linux tools) |
| **Windows** | Via WSL (Linux path) | Via WSL (Linux path) |

So:

- **No dependency on host WiFi *tools*:** Void uses only its own code and the kernel (or OS) API.
- **No “wrapping” of `iw`/`nmcli`/`wpa_supplicant`:** Those are not required for scanning (and for open networks on Linux, not for connect either).
- **Kernel/OS still in charge:** WiFi is still accessed through the running OS (Linux/macOS/Windows). That cannot be removed without turning Void into a kernel.

## Implementation notes

- **Linux:** `src/utils/wifi_native.c` uses **netlink (nl80211)** to trigger a scan and read results. No `iw` or `nmcli`. Optional native connect for open APs via nl80211.
- **macOS:** Uses system facilities (no Linux binaries). No netlink (Linux-only).
- **WPA/WPA2:** Connecting to secured networks requires the 4-way handshake and crypto. Void can either call `wpa_supplicant` as a fallback or, in the future, implement WPA client logic natively; in both cases the kernel still handles the actual radio.
