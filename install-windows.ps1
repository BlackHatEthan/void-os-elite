# Void OS Elite - Windows installer
# Void OS is a POSIX shell and builds on Linux/macOS. On Windows, use WSL.
#
# Run in PowerShell: .\install-windows.ps1
# Or: pwsh -File install-windows.ps1

Write-Host "=========================================="
Write-Host "  Void OS Elite - Windows"
Write-Host "=========================================="
Write-Host ""

$wslPath = Get-Command wsl -ErrorAction SilentlyContinue
if ($wslPath) {
    Write-Host "WSL detected. Installing Void OS Elite inside WSL..." -ForegroundColor Green
    Write-Host ""
    $scriptDir = (Get-Location).Path
    if ($MyInvocation.MyCommand.Path) {
        $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    }
    # Convert Windows path to WSL path (e.g. C:\Users\you\voidos-elite -> /mnt/c/Users/you/voidos-elite)
    $wslDir = wsl -e wslpath -u "$scriptDir" 2>$null
    if ($wslDir) {
        wsl -e bash -c "cd '$wslDir' && ./install-linux.sh $args"
        exit $LASTEXITCODE
    }
    wsl -e bash -c "cd ~ && [ -d voidos-elite ] && cd voidos-elite && ./install-linux.sh $args || echo 'Run this script from the voidos-elite folder (or clone it first).'"
    exit $LASTEXITCODE
}

Write-Host "WSL (Windows Subsystem for Linux) is required to run Void OS Elite." -ForegroundColor Yellow
Write-Host ""
Write-Host "Void OS Elite is a native Linux/macOS shell. On Windows:"
Write-Host "  1. Install WSL:  wsl --install"
Write-Host "  2. Restart if prompted"
Write-Host "  3. Open Ubuntu (or your WSL distro) from the Start menu"
Write-Host "  4. Clone and install:"
Write-Host "     git clone https://github.com/BlackHatEthan/void-os-elite.git"
Write-Host "     cd void-os-elite"
Write-Host "     ./install-linux.sh"
Write-Host ""
Write-Host "Then run 'void' from inside WSL." -ForegroundColor Cyan
Write-Host ""
Write-Host "Install WSL now? (opens Microsoft doc)" -ForegroundColor Yellow
$r = Read-Host "Open https://docs.microsoft.com/en-us/windows/wsl/install (y/N)"
if ($r -match '^[Yy]') {
    Start-Process "https://docs.microsoft.com/en-us/windows/wsl/install"
}
exit 1
