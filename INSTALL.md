# Void OS Elite – Installation

Install Void OS Elite on **Linux**, **macOS**, or **Windows** (via WSL).

---

## Quick install by OS

| OS | What to run | Download / use |
|----|-------------|----------------|
| **Linux** | `./install.sh` or `./install-linux.sh` | `install-linux.sh` |
| **macOS** | `./install.sh` or `./install-macos.sh` | `install-macos.sh` |
| **Windows** | WSL + `./install-linux.sh` or `.\install-windows.ps1` | `install-windows.ps1` |

---

## 1. Get the repo

```bash
git clone https://github.com/BlackHatEthan/void-os-elite.git
cd void-os-elite
```

Or download and extract the [ZIP from GitHub](https://github.com/BlackHatEthan/void-os-elite), then `cd voidos-elite`.

---

## 2. Run the installer

### Auto-detect (Linux / macOS)

From the repo root:

```bash
chmod +x install.sh
./install.sh
```

This detects your OS and runs the right install script.

### Linux only

```bash
chmod +x install-linux.sh
./install-linux.sh
```

- Installs build tools if you need a reminder.
- Builds the `void` binary in `void/`.
- Asks whether to install to `/usr/local/bin` (optional).

Install to system without prompt:

```bash
./install-linux.sh --install
```

### macOS only

```bash
chmod +x install-macos.sh
./install-macos.sh
```

- Needs Xcode Command Line Tools (`xcode-select --install` if you don’t have them).
- Builds in `void/`, then optionally installs to `/usr/local/bin`.

Install to system without prompt:

```bash
./install-macos.sh --install
```

### Windows (WSL)

Void OS is a POSIX shell; it runs inside **WSL** (Windows Subsystem for Linux), not natively in Windows.

**Option A – PowerShell script (recommended)**

From the repo folder in PowerShell:

```powershell
.\install-windows.ps1
```

- If WSL is installed: runs the Linux installer inside WSL.
- If not: prints instructions to install WSL, then clone and run `./install-linux.sh` inside WSL.

**Option B – Manual with WSL**

1. Install WSL: `wsl --install` (then restart if asked).
2. Open your WSL distro (e.g. Ubuntu).
3. Clone and install:

   ```bash
   git clone https://github.com/BlackHatEthan/void-os-elite.git
   cd void-os-elite
   ./install-linux.sh
   ```

Run `void` from the WSL terminal.

---

## 3. Run Void OS

- **System install:** run `void` in any terminal.
- **Local build only:** run `./void` from the `void/` directory, or add `void/` to your `PATH`.

Help:

```bash
void-help
```

---

## File overview

| File | Purpose |
|------|--------|
| `install.sh` | Detects OS (Linux/macOS) and runs the right installer. |
| `install-linux.sh` | Build + optional install on Linux (and WSL). |
| `install-macos.sh` | Build + optional install on macOS. |
| `install-windows.ps1` | On Windows: uses WSL to run Linux install, or explains how to install WSL. |
| `INSTALL.md` | This file. |

---

## Requirements

- **Linux:** `gcc` (or `cc`), `make`.  
  - Debian/Ubuntu: `sudo apt install build-essential`  
  - Fedora: `sudo dnf install gcc make`  
  - Arch: `sudo pacman -S base-devel`
- **macOS:** Xcode Command Line Tools: `xcode-select --install`
- **Windows:** WSL (e.g. Ubuntu). No native Windows build.
