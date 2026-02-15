#!/bin/bash

# Void OS - Kali Tools Dependencies Checker
# Checks if all required dependencies are installed (non-root)

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
GREY='\033[0;90m'
WHITE='\033[0;37m'
RESET='\033[0m'

echo -e "${WHITE}Void OS - Kali Tools Dependencies Checker${RESET}"
echo -e "${GREY}═══════════════════════════════════════════════${RESET}"
echo ""

check_command() {
    local cmd=$1
    local name=$2
    local required=${3:-true}
    
    if command -v "$cmd" &> /dev/null; then
        local version=""
        case $cmd in
            git) version=$(git --version | cut -d' ' -f3) ;;
            gcc) version=$(gcc --version | head -n1 | cut -d' ' -f3) ;;
            make) version=$(make --version | head -n1 | cut -d' ' -f3) ;;
            python3) version=$(python3 --version | cut -d' ' -f2) ;;
            ruby) version=$(ruby --version | cut -d' ' -f2) ;;
            perl) version=$(perl --version | grep "This is perl" | cut -d' ' -f4) ;;
            go) version=$(go version | cut -d' ' -f3) ;;
            rustc) version=$(rustc --version | cut -d' ' -f2) ;;
            cargo) version=$(cargo --version | cut -d' ' -f2) ;;
            bundle|bundler) version=$(bundle --version 2>/dev/null || bundler --version 2>/dev/null | cut -d' ' -f3) ;;
        esac
        echo -e "${GREEN}✓ $name${RESET} - $version"
        return 0
    else
        if [ "$required" = "true" ]; then
            echo -e "${RED}✗ $name - NOT FOUND${RESET}"
            return 1
        else
            echo -e "${YELLOW}⚠ $name - NOT FOUND (optional)${RESET}"
            return 0
        fi
    fi
}

check_library() {
    local lib=$1
    local name=$2
    
    # Check for library files
    if find /usr/lib* /lib* -name "*$lib*" 2>/dev/null | grep -q .; then
        echo -e "${GREEN}✓ $name${RESET} - Found"
        return 0
    else
        echo -e "${RED}✗ $name - NOT FOUND${RESET}"
        return 1
    fi
}

main() {
    local all_ok=true
    local missing=()
    
    echo -e "${WHITE}Essential Tools:${RESET}"
    echo -e "${GREY}─────────────────────────────────────${RESET}"
    
    check_command "git" "git" || { all_ok=false; missing+=("git"); }
    check_command "gcc" "gcc" || { all_ok=false; missing+=("gcc"); }
    check_command "make" "make" || { all_ok=false; missing+=("make"); }
    check_command "python3" "python3" || { all_ok=false; missing+=("python3"); }
    check_command "ruby" "ruby" || { all_ok=false; missing+=("ruby"); }
    check_command "perl" "perl" || { all_ok=false; missing+=("perl"); }
    check_command "go" "go" || { all_ok=false; missing+=("go"); }
    check_command "rustc" "rust" || { all_ok=false; missing+=("rust"); }
    check_command "cargo" "cargo" || { all_ok=false; missing+=("cargo"); }
    
    echo ""
    echo -e "${WHITE}Optional Tools:${RESET}"
    echo -e "${GREY}─────────────────────────────────────${RESET}"
    
    check_command "bundle" "bundler" "false" || check_command "bundler" "bundler" "false"
    
    echo ""
    echo -e "${WHITE}Development Libraries:${RESET}"
    echo -e "${GREY}─────────────────────────────────────${RESET}"
    
    check_library "libssl" "libssl-dev" || { all_ok=false; missing+=("libssl-dev"); }
    check_library "libssh" "libssh-dev" || { all_ok=false; missing+=("libssh-dev"); }
    check_library "OpenCL" "opencl-headers" || { all_ok=false; missing+=("opencl-headers"); }
    
    echo ""
    echo -e "${GREY}─────────────────────────────────────${RESET}"
    
    if [ "$all_ok" = true ]; then
        echo -e "${GREEN}✓ All dependencies are installed!${RESET}"
        echo ""
        echo -e "${WHITE}You can now install Kali tools:${RESET}"
        echo -e "${GREY}  void-install-kali <tool_name>${RESET}"
        echo -e "${GREY}  void-install-kali all${RESET}"
        return 0
    else
        echo -e "${RED}✗ Some dependencies are missing${RESET}"
        echo ""
        echo -e "${YELLOW}Missing dependencies:${RESET}"
        printf "${GREY}  - %s${RESET}\n" "${missing[@]}"
        echo ""
        echo -e "${WHITE}To install missing dependencies, run:${RESET}"
        echo -e "${GREY}  sudo ./scripts/install_kali_dependencies.sh${RESET}"
        return 1
    fi
}

main

