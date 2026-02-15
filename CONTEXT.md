# Void OS Elite - Native CLI Operating System Wrapper

## Project Overview

**Void OS Elite** is a high-performance, developer-grade, security-aware CLI operating system wrapper written in **C and Assembly**. It functions as a native, persistent shell layer that directly interfaces with the host operating system through system calls (syscalls), providing comprehensive filesystem control, process management, environment management, Git integration, developer tooling, networking utilities, system monitoring, project awareness, plugin architecture, and security-conscious workflow tools.

**Critical Architecture Note**: Void OS Elite is **NOT a sandbox** — it is a **native shell environment** that:
- Executes **real system calls** directly to the host OS kernel
- Runs as a **native binary** (compiled C/ASM), not an interpreted environment
- Has **full system access** equivalent to bash/zsh
- Wraps the host OS at the **system call level**, not through an interpreter
- Uses **direct OS APIs** (POSIX syscalls, Win32 API, etc.) for all operations

**Important**: Void OS is a **custom shell environment** (bash/zsh equivalent) — not a kernel. It wraps the host OS and executes real system commands through native system calls. Even if Python scripts are executed as utilities, the core shell engine is **native C/ASM** and operates at the system level, not within any sandbox.

## Sandbox Clarification

**Void OS Elite is NOT a Python Sandbox:**

1. **Core Engine**: The shell itself is a **compiled C/ASM binary** that executes natively on the host OS. There is no Python interpreter running the shell.

2. **System Calls**: All operations (file I/O, process management, networking) use **direct system calls** to the OS kernel. No sandboxing layer exists.

3. **Python Execution**: If Python scripts are executed (via `summon` command), they run as **external processes** through `fork()` + `exec()`, just like any other program. The shell does not interpret Python code.

4. **Full System Access**: Void OS has the same permissions and system access as the user running it. It can:
   - Execute any command in PATH
   - Access all files the user can access
   - Create/manage processes
   - Make network connections
   - Modify environment variables
   - All operations are at the **system level**, not sandboxed

5. **No Isolation**: Unlike sandboxed environments (Docker containers, Python virtual environments for isolation, etc.), Void OS operates directly on the host system with full privileges of the executing user.

**Comparison:**
- ❌ **NOT like**: Python REPL, Jupyter notebook, Docker container, chroot jail
- ✅ **IS like**: bash, zsh, fish shell - a native system shell with full OS access

### Design Principles

- **Stability**: Robust error handling, never crash to terminal unless fatal
- **Performance**: Non-blocking UI, background tasks, minimal startup delay
- **Clean Architecture**: Modular command loading, extensible design
- **Extensibility**: Plugin architecture for custom commands
- **Professional Developer Usability**: Git-aware, project-aware, intuitive workflow
- **Monochrome Noir Aesthetic**: White/grey primary, dim grey secondary, muted blue accent, minimal visual noise

---

## Core Components

### 1. Boot Identity & Initialization

**Display on startup:**
- ASCII VOID logo
- Host OS information
- System architecture (x86_64, ARM, etc.)
- Username
- Current working directory
- Git version (if installed)
- Void OS version

**Initialization stages** (with rich progress indicators):
1. Piercing the Veil…
2. Mounting Reality…
3. Initializing Process Manager…
4. Loading Environment Matrix…
5. Binding Developer Tools…
6. Detecting Security Capabilities…
7. Synchronizing Git Interface…
8. Stabilizing Void Core…

**Completion message:**
```
Void OS Elite Initialized.
```

### 2. Intelligent Prompt System

**Format:**
```
[HH:MM:SS] void@user:/path [branch][jobs]$
```

**Features:**
- Current directory (abbreviated if long)
- Git branch detection (if in repository)
- Dirty indicator (*) for uncommitted changes
- Background job count
- Session-configurable username

**Example:**
```
[14:22:10] void@ethan:/projects/api [main*][+1]$
```

### 3. Core Shell Engine

**Requirements:**
- Persistent input loop (native C event loop)
- Global error handling (C error handling, signal handlers)
- Command parsing with arguments (custom tokenizer or POSIX-compliant parser)
- Alias resolution (before execution)
- Environment variable expansion (direct `getenv`/`setenv` syscalls)
- Unknown command fallback via PATH search (execvp/execvpe syscalls)

**Implementation:**
- **C Core**: Main shell loop, command parsing, process management
- **Assembly**: Performance-critical sections (string operations, syscalls)
- **System Calls**: Direct POSIX syscalls (fork, exec, wait, chdir, etc.)
- **No Interpreter**: Native binary execution, no Python VM or sandbox

**Key System Interfaces:**
- POSIX syscalls: `fork()`, `exec()`, `wait()`, `chdir()`, `getcwd()`, `stat()`
- Environment: `getenv()`, `setenv()`, `unsetenv()`
- Process: `kill()`, `getpid()`, `getppid()`
- Filesystem: `open()`, `read()`, `write()`, `close()`, `stat()`, `chmod()`

### 4. Filesystem Control Commands

| Command | Description |
|---------|-------------|
| `pulse` | List files with size, date, type, permissions |
| `warp [dir]` | Change directory (cd wrapper) |
| `vanish` | Clear terminal |
| `echoes [file]` | Display file with syntax highlighting |
| `shroud [name]` | Create file/folder |
| `transmute [old] [new]` | Move/rename files |
| `veil [file]` | Show file permissions |
| `seal [file] [mode]` | chmod wrapper |
| `watch [dir]` | Monitor directory for changes (file created/modified/deleted) |

### 5. Process & Job Management

**Implementation:** 
- Use native `fork()` and `exec()` syscalls for process creation
- Background jobs: `fork()`, then `exec()` with process group management
- Job tracking: Maintain PID table in memory (linked list or hash table)
- Signal handling: `SIGCHLD` handler for job completion detection

**Commands:**
- `spawn [command]` - Run command in background (fork + exec)
- `ps` / `jobs` - List running processes/jobs (read from `/proc` or syscalls)
- `terminate [pid]` - Kill process by PID (`kill()` syscall)
- `foreground [pid]` - Bring background job to foreground (`tcsetpgrp()`)

**Integration:** Prompt shows active job count in `[jobs]` indicator.

**System Calls:**
- `fork()` - Create child process
- `execv()`, `execvp()`, `execvpe()` - Execute programs
- `waitpid()` - Wait for process completion
- `kill()` - Send signals to processes
- `getpgid()`, `setpgid()` - Process group management

### 6. Environment System

**Internal Environment:** Direct access to process environment via `environ` pointer and syscalls.

**Implementation:**
- Use `extern char **environ` for environment access
- `getenv()`, `setenv()`, `unsetenv()` syscalls
- Custom environment table for session variables
- Inherit from parent shell on startup

**Commands:**
- `env` - Display environment variables (iterate `environ`)
- `export KEY=VALUE` - Set environment variable (`setenv()`)
- `unset KEY` - Remove environment variable (`unsetenv()`)
- `reload` - Reload environment from persistent storage

**Persistent Storage:**
```
~/.voidrc
```
(Plain text or binary format, parsed on startup)

### 7. Alias System

**Commands:**
- `alias name=command` - Create alias
- `unalias name` - Remove alias
- `aliases` - List all aliases

**Resolution:** Aliases resolved before command execution.

### 8. Git Developer Suite

**Commands:**
- `tether [args]` - Pass arguments directly to git
- `anchor [url]` - Clone repository
- `sync` - Pull latest changes
- `transmit` - Push changes
- `commit "msg"` - Add all changes + commit with message
- `status` - Rich formatted git status

**Prompt Integration:**
- Auto-detect current branch
- Auto-detect dirty state (uncommitted changes)
- Display in prompt: `[branch*]` format

### 9. Developer Utilities

**Commands:**
- `summon [file.py]` - Execute Python script (if Python installed) via `execvp("python3", ...)`
- `forgepy` - Interactive Python REPL (if Python installed) - launches external Python interpreter
- `multitool` - Native C utility panel with:
  - Hash generator (MD5/SHA256) - using OpenSSL or native crypto libraries
  - Base64 encode/decode - native C implementation
  - JSON formatter - using cJSON or similar C library
  - UUID generator - using system UUID library or native implementation
  - Timestamp converter - native C time functions

**Note:** Python execution is **optional** and runs as an **external process** via `exec()`. The core shell does not depend on Python. All utilities are implemented in native C/ASM.

### 10. Networking & Local Dev Tools

**Commands:**
- `intercept [url]` - HTTP request preview (using libcurl or native socket syscalls)
- `ping [host]` - System ping wrapper (executes system `ping` command)
- `ports` - Show listening ports (read from `/proc/net/tcp` on Linux, or use `netstat`/`lsof` syscalls)
- `scan [host]` - Wrapper for nmap if installed (executes external `nmap` binary)
- `serve [port]` - Start local HTTP server (native C HTTP server or execute Python's `http.server`)
- `tunnel-info` - Display local IP addresses (using `getifaddrs()` or `ioctl(SIOCGIFCONF)`)

**Implementation:**
- Socket operations: `socket()`, `bind()`, `listen()`, `accept()`, `connect()`
- Network interfaces: `getifaddrs()`, `ioctl()` for interface enumeration
- Process network info: Read `/proc/net/tcp` or use `netstat`/`ss` commands

**Error Handling:** If required tool missing:
```
Required tool not found in this reality.
```

### 11. System Monitoring

**Command:** `entropy`

**Display live metrics** (native C terminal UI with ANSI escape codes or ncurses):
- CPU → "Void Stability" (read from `/proc/stat` or syscalls)
- RAM → "Memory Leakage" (read from `/proc/meminfo` or syscalls)
- Disk → "Reality Allocation" (using `statvfs()` syscall)
- Process count (read from `/proc` or use syscalls)

**Implementation:**
- Read system stats via `/proc` filesystem (Linux) or syscalls
- Use `ncurses` or raw ANSI codes for live updating terminal UI
- Poll system stats in background thread/process

### 12. Project Awareness Engine

**Auto-detection** when entering directory:
- `.git` - Git repository (check for `.git` directory via `stat()`)
- `requirements.txt` - Python dependencies (file existence check)
- `pyproject.toml` - Python project config (file existence check)
- `package.json` - Node.js project (file existence check)
- `Dockerfile` - Docker project (file existence check)
- `.env` - Environment variables (file existence check)

**Implementation:**
- Use `stat()` or `access()` syscalls to check file existence
- Directory scanning via `opendir()`, `readdir()`, `closedir()`

**Command:** `context`

**Displays:**
- Project type
- Virtual environment status
- Git status
- Dependency hints

### 13. Security & OPSEC Awareness (Defensive Tools)

**Commands:**
- `startup-check` - Display:
  - Git global config email
  - SSH key presence
  - `.env` files in current directory
  - Exposed secrets risk warnings

- `perm-audit [dir]` - Find:
  - World-writable files
  - Executables in unexpected locations

- `net-watch` - Show active network connections (read from `/proc/net/tcp`, `/proc/net/udp` or use `netstat`/`ss`)

**Implementation:**
- File permissions: `stat()` syscall to read file mode
- Network connections: Parse `/proc/net/tcp` and `/proc/net/udp` or execute `netstat`/`ss`
- Process scanning: Read `/proc` directory or use `ps` command

**Note:** These are **defensive awareness tools only** - not offensive security tools.

### 14. Plugin Architecture

**Plugin Directory:**
```
~/.void/plugins/
```

**Auto-loading:** Dynamically load shared libraries (`.so` on Linux, `.dylib` on macOS, `.dll` on Windows) from plugin directory.

**Plugin Format:**
```c
// Plugin must export these functions:
void* void_plugin_init(void* shell_context);
void void_plugin_register_command(void* shell_context, const char* name, void (*command_func)(int argc, char** argv));
void void_plugin_cleanup(void* shell_context);
```

**Implementation:**
- Use `dlopen()`, `dlsym()`, `dlclose()` for dynamic loading (POSIX)
- On Windows: `LoadLibrary()`, `GetProcAddress()`, `FreeLibrary()`
- Plugin ABI versioning for compatibility

**Command:**
- `forge install <path>` - Install plugin from path (copy shared library to plugin directory)

### 15. Developer Tool Detection

**On boot, detect** by searching PATH using `access()` or `execvp()`:
- `git` - Check if `git` executable exists in PATH
- `python3` - Check if `python3` executable exists in PATH
- `pip` - Check if `pip` executable exists in PATH
- `nmap` - Check if `nmap` executable exists in PATH
- `node` - Check if `node` executable exists in PATH
- `docker` (optional) - Check if `docker` executable exists in PATH

**Implementation:**
- Use `access()` syscall with `X_OK` flag to check executable existence
- Search PATH environment variable (split by `:` on Unix, `;` on Windows)
- Cache results for performance

**Display:**
```
Developer Tools Bound: git, python3, pip, ...
```

### 16. User Identity Layer

**Commands:**
- `whoami` - Display current session username
- `identity [name]` - Set cosmetic session username

### 17. Logging & Stability

**Log File:**
```
~/.void_logs
```

**Log:**
- Errors
- Crashes
- Command failures

**Requirement:** Shell must never crash to terminal unless fatal. All errors should be caught and logged.

### 18. Performance Requirements

- Non-blocking UI where possible
- Background tasks for watchers
- Minimal startup delay (< 2 seconds)
- Modular command loading (lazy load commands)

### 19. Error Messaging (Void Style)

| Error | Message |
|-------|---------|
| File not found | "Target lost in the Void." |
| Permission denied | "Access denied by the Veil." |
| Command not found (after fallback fails) | "Unknown signal detected." |
| Process error | "Process destabilized." |

### 20. Aesthetic Rules

**Monochrome Noir Theme:**
- White / grey primary text
- Dim grey secondary text
- Muted blue accent (for highlights)
- Use ANSI escape codes or ncurses for terminal formatting
- Minimal visual noise
- Professional terminal feel

**Implementation:**
- ANSI escape sequences for colors: `\033[0;37m` (white), `\033[0;90m` (grey), `\033[0;34m` (blue)
- Or use `ncurses` library for advanced terminal control
- Consistent formatting across all output

### 21. Behavioral Goals

Void OS Elite should feel like:
- A personal developer operating environment
- Git-aware and project-aware
- Security-conscious
- Extensible via plugins
- Stable for daily use
- Comparable to a customized professional shell (bash/zsh with customizations)

### 22. Scope & Limitations

**Void OS Elite:**
- ✅ Wraps the host OS
- ✅ Executes real system commands
- ✅ Acts as a full-featured shell environment
- ✅ Designed for long-term personal development and expansion

**Void OS Elite does NOT:**
- ❌ Implement a kernel
- ❌ Replace the operating system
- ❌ Modify system-level components

---

## Technical Stack

**Core Implementation:**
- **C (C11/C17 standard)** - Primary language for shell engine, command implementations
- **Assembly (x86_64/ARM)** - Performance-critical sections, optimized syscalls
- **POSIX System Calls** - Direct kernel interface (fork, exec, wait, chdir, stat, etc.)
- **Native Binaries** - Compiled to machine code, no interpreter or VM

**Core Libraries:**
- **Standard C Library (libc)** - `stdio.h`, `stdlib.h`, `string.h`, `stdlib.h`, `unistd.h`, `sys/stat.h`, etc.
- **ncurses** (optional) - Advanced terminal UI and formatting
- **libcurl** (optional) - HTTP client functionality
- **OpenSSL** (optional) - Cryptographic functions (hashing, etc.)
- **cJSON** (optional) - JSON parsing and formatting

**System Interfaces:**
- **POSIX API** - File operations, process management, signals
- **Linux**: `/proc` filesystem for system information
- **macOS**: `sysctl()` and system frameworks
- **Windows**: Win32 API (if Windows support is added)

**External Tools (Optional, executed via exec):**
- `git` (system tool) - Version control
- `python3` (system tool) - Python script execution (if needed)
- `nmap` (system tool) - Network scanning
- `docker` (system tool) - Container management
- `node` (system tool) - Node.js execution

**Build System:**
- **Make** or **CMake** - Build system
- **GCC** or **Clang** - Compiler
- **GAS** or **NASM** - Assembler (for ASM components)

---

## File Structure

```
void/
├── Makefile              # Build configuration
├── CMakeLists.txt        # Alternative build system
├── src/
│   ├── main.c            # Entry point
│   ├── core/
│   │   ├── shell.c       # Main shell engine
│   │   ├── shell.h       # Shell header
│   │   ├── prompt.c      # Prompt system
│   │   ├── prompt.h
│   │   ├── parser.c      # Command parser
│   │   ├── parser.h
│   │   ├── executor.c    # Command executor
│   │   ├── executor.h
│   │   └── errors.c      # Error handling
│   ├── commands/
│   │   ├── filesystem.c  # Filesystem commands
│   │   ├── process.c     # Process management
│   │   ├── git.c         # Git integration
│   │   ├── network.c     # Networking tools
│   │   ├── system.c      # System monitoring
│   │   ├── security.c    # Security tools
│   │   └── utils.c       # Developer utilities
│   ├── utils/
│   │   ├── logger.c      # Logging system
│   │   ├── config.c      # Configuration management
│   │   ├── project.c     # Project awareness
│   │   └── string.c      # String utilities
│   ├── asm/
│   │   ├── syscalls.asm  # Optimized syscall wrappers
│   │   └── string.asm    # Fast string operations
│   └── plugins/
│       ├── loader.c      # Dynamic library loader
│       └── loader.h
├── include/
│   └── void.h            # Main header
├── plugins/               # User plugins directory
└── build/                 # Build output
```

---

## Implementation Notes

1. **Command Registration**: Use function pointer table or hash table for command dispatch
2. **Error Handling**: Use C error handling patterns (return codes, errno, signal handlers)
3. **Background Jobs**: Track PIDs in linked list or hash table structure
4. **Git Detection**: Execute `git rev-parse --git-dir` via `fork()` + `exec()` or check `.git` directory with `stat()`
5. **Project Detection**: Scan directory on `warp` command using `opendir()`/`readdir()`
6. **Plugin Loading**: Use `dlopen()`/`dlsym()` (POSIX) or `LoadLibrary()`/`GetProcAddress()` (Windows) for dynamic loading
7. **Logging**: Custom logging system using `fopen()`/`fprintf()` to `~/.void_logs`
8. **Configuration**: Parse `~/.voidrc` as plain text or use simple key-value format (no JSON/TOML dependency required)
9. **Memory Management**: Manual memory management with `malloc()`/`free()`, careful to avoid leaks
10. **Signal Handling**: Register signal handlers for `SIGINT`, `SIGCHLD`, `SIGTERM` using `signal()` or `sigaction()`
11. **Thread Safety**: Use mutexes/pthreads if multi-threaded components needed
12. **Portability**: Use POSIX-compliant code, conditional compilation for platform-specific features

---

## Compilation & Execution Model

**Build Process:**
1. C source files compiled to object files (`.o`) by GCC/Clang
2. Assembly source files assembled to object files by GAS/NASM
3. Object files linked into single native binary executable
4. Binary is architecture-specific (x86_64, ARM, etc.)
5. No interpreter, no VM, no bytecode - pure machine code

**Execution Model:**
- Binary executed directly by OS kernel
- Runs in same process space as user
- Has same privileges as user account
- Makes direct syscalls to kernel
- No sandboxing, no isolation, no interpretation layer

**Example:**
```bash
# Build
make

# Execute (native binary, not Python script)
./void

# This is a real shell, equivalent to:
bash
zsh
```

## Development Priorities

1. **Phase 1**: Core shell engine (C), prompt system, basic command parsing
2. **Phase 2**: Filesystem operations (syscalls), process management (fork/exec), environment
3. **Phase 3**: Git integration (exec external git), project awareness (directory scanning)
4. **Phase 4**: Developer utilities (native C), networking tools (socket syscalls)
5. **Phase 5**: Security tools (file permissions, network monitoring), system monitoring (/proc reading)
6. **Phase 6**: Plugin architecture (dlopen), extensibility, performance optimization (ASM)

