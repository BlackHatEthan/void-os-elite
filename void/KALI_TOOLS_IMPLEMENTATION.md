# Kali Top 10 Tools Implementation

## ✅ Implementation Complete

All Kali Top 10 security tools have been integrated into Void OS Elite with installation and execution support.

---

## Implemented Tools

### 1. **Metasploit Framework**
- **GitHub**: https://github.com/rapid7/metasploit-framework
- **Dependencies**: Ruby, PostgreSQL, Bundler
- **Binaries**: `msfconsole`, `msfvenom`, `msfd`, `msfrpc`
- **Command**: `metasploit` or `metasploit venom` (for msfvenom)

### 2. **SQLMap**
- **GitHub**: https://github.com/sqlmapproject/sqlmap
- **Dependencies**: Python3
- **Binaries**: `sqlmap`
- **Command**: `sqlmap`

### 3. **Nikto**
- **GitHub**: https://github.com/sullo/nikto
- **Dependencies**: Perl
- **Binaries**: `nikto`
- **Command**: `nikto`

### 4. **Nmap**
- **GitHub**: https://github.com/nmap/nmap
- **Dependencies**: None (or build tools for compilation)
- **Binaries**: `nmap`, `ncat`
- **Command**: `nmap` (falls back to native scan if not installed)

### 5. **WPScan**
- **GitHub**: https://github.com/wpscanteam/wpscan
- **Dependencies**: Ruby
- **Binaries**: `wpscan`
- **Command**: `wpscan`

### 6. **Gobuster**
- **GitHub**: https://github.com/OJ/gobuster
- **Dependencies**: Go compiler
- **Binaries**: `gobuster`
- **Command**: `gobuster`

### 7. **Feroxbuster**
- **GitHub**: https://github.com/epi052/feroxbuster
- **Dependencies**: Rust compiler, Cargo
- **Binaries**: `feroxbuster`
- **Command**: `feroxbuster`

### 8. **John the Ripper**
- **GitHub**: https://github.com/openwall/john
- **Dependencies**: Build tools
- **Binaries**: `john`, `unshadow`, `unafs`
- **Command**: `john`

### 9. **Hashcat**
- **GitHub**: https://github.com/hashcat/hashcat
- **Dependencies**: Build tools, OpenCL headers
- **Binaries**: `hashcat`, `hashcat64`
- **Command**: `hashcat`

### 10. **Hydra**
- **GitHub**: https://github.com/vanhauser-thc/thc-hydra
- **Dependencies**: Build tools, libssl-dev, libssh-dev
- **Binaries**: `hydra`, `pw-inspector`
- **Command**: `hydra`

---

## Installation System

### Installer Command: `void-install-kali`

**Usage:**
```bash
# List all available tools
void-install-kali list

# Install a specific tool
void-install-kali metasploit
void-install-kali sqlmap
void-install-kali nmap

# Install all tools at once
void-install-kali all
```

### Installation Process

1. **Repository Cloning**: Tools are cloned from GitHub to `~/.void/packages/cache/<tool_name>/`
2. **Dependency Installation**: Install scripts are run (e.g., `bundle install` for Ruby tools)
3. **Binary Symlinking**: Binaries are symlinked to `~/.void/packages/bin/`
4. **PATH Integration**: The bin directory is added to PATH automatically

### Installation Directory Structure

```
~/.void/packages/
├── cache/
│   ├── metasploit-framework/
│   ├── sqlmap/
│   ├── nikto/
│   ├── nmap/
│   ├── wpscan/
│   ├── gobuster/
│   ├── feroxbuster/
│   ├── john/
│   ├── hashcat/
│   └── hydra/
└── bin/
    ├── msfconsole -> ../cache/metasploit-framework/msfconsole
    ├── sqlmap -> ../cache/sqlmap/sqlmap.py
    ├── nikto -> ../cache/nikto/nikto.pl
    └── ... (other binaries)
```

---

## Tool Execution

### Automatic Detection

All tool commands automatically:
1. Check if the tool is installed
2. If installed, execute the tool with provided arguments
3. If not installed, display installation instructions

### Execution Examples

```bash
# Metasploit
metasploit                    # Launches msfconsole
metasploit venom -p windows/meterpreter/reverse_tcp LHOST=192.168.1.1

# SQLMap
sqlmap -u http://target.com/page?id=1 --dbs

# Nikto
nikto -h http://target.com

# Nmap
nmap -sS -p 1-1000 target.com

# WPScan
wpscan --url http://target.com

# Gobuster
gobuster dir -u http://target.com -w wordlist.txt

# Feroxbuster
feroxbuster -u http://target.com

# John the Ripper
john --wordlist=wordlist.txt hashfile.txt

# Hashcat
hashcat -m 0 hashfile.txt wordlist.txt

# Hydra
hydra -l admin -P passwords.txt ssh://target.com
```

---

## Implementation Details

### Files Created

1. **`void/src/commands/kali_tools.h`**: Header file with tool catalog and function declarations
2. **`void/src/commands/kali_tools.c`**: Implementation of installation and execution system

### Key Functions

- `install_kali_tool()`: Install a specific tool
- `install_all_kali_tools()`: Install all 10 tools
- `check_kali_tool_installed()`: Check if tool is installed
- `execute_kali_tool()`: Execute installed tool
- `find_tool_binary()`: Locate tool binary in system
- `list_kali_tools()`: Display all tools and their status

### Integration Points

- **Security Commands**: Updated `security_ext.c` to use Kali tools system
- **Network Commands**: Updated `nmap` to check for Kali tools installation
- **Executor**: Registered `void-install-kali` command

---

## Requirements

### System Requirements

- **Git**: Required for cloning repositories
- **Build Tools**: Required for compiling tools (gcc, make, etc.)
- **Language Runtimes**: 
  - Ruby (for Metasploit, WPScan)
  - Python3 (for SQLMap)
  - Perl (for Nikto)
  - Go (for Gobuster)
  - Rust/Cargo (for Feroxbuster)

### Disk Space

Installing all tools will require significant disk space:
- **Metasploit**: ~500MB+
- **Other tools**: ~50-200MB each
- **Total**: ~2-3GB for all tools

---

## Usage Workflow

### First-Time Setup

```bash
# 1. List available tools
void-install-kali list

# 2. Install tools you need
void-install-kali metasploit
void-install-kali sqlmap
void-install-kali nmap

# 3. Verify installation
metasploit --version
sqlmap --version
nmap --version
```

### Daily Usage

```bash
# Tools are now available as regular commands
sqlmap -u http://target.com
nmap -sS target.com
metasploit
```

---

## Notes

- Tools are installed to user's home directory (`~/.void/packages/`)
- No root/sudo access required for installation
- Tools can be updated by re-running `void-install-kali <tool>`
- Installation scripts are run automatically (e.g., `bundle install`)
- PATH is automatically updated, but you may need to add to `~/.voidrc` for persistence

---

## Future Enhancements

- [ ] Package version management
- [ ] Automatic updates
- [ ] Dependency conflict resolution
- [ ] Binary caching for faster installations
- [ ] Integration with `void-install` package manager (when implemented)

---

**Status**: ✅ Complete - All 10 Kali Top tools integrated and ready for installation

