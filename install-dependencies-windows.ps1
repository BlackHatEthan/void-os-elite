# install-dependencies-windows.ps1
# Installs all dependencies needed to build and run Void OS Elite on Windows
# Note: Void OS Elite runs in WSL (Windows Subsystem for Linux) on Windows

Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "  Void OS Elite - Windows Dependencies" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""

# Check if running in WSL
if (Test-Path "/proc/version" -PathType Leaf) {
    $wslVersion = Get-Content "/proc/version"
    if ($wslVersion -match "Microsoft|WSL") {
        Write-Host "Detected WSL environment. Running Linux dependency installer..." -ForegroundColor Green
        Write-Host ""
        bash install-dependencies-linux.sh
        exit $LASTEXITCODE
    }
}

# Check for WSL
Write-Host "Checking for WSL (Windows Subsystem for Linux)..." -ForegroundColor Blue
$wslInstalled = $false

try {
    $wslOutput = wsl --list --quiet 2>&1
    if ($LASTEXITCODE -eq 0) {
        $wslInstalled = $true
        Write-Host "✓ WSL is installed" -ForegroundColor Green
        Write-Host ""
        Write-Host "Available WSL distributions:" -ForegroundColor Blue
        wsl --list
        Write-Host ""
        Write-Host "To install dependencies, run this script from within WSL:" -ForegroundColor Yellow
        Write-Host "  wsl" -ForegroundColor Cyan
        Write-Host "  cd /mnt/c/path/to/voidos-elite" -ForegroundColor Cyan
        Write-Host "  bash install-dependencies-linux.sh" -ForegroundColor Cyan
        Write-Host ""
        exit 0
    }
} catch {
    $wslInstalled = $false
}

if (-not $wslInstalled) {
    Write-Host "WSL is not installed." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Void OS Elite requires WSL to run on Windows." -ForegroundColor Yellow
    Write-Host ""
    Write-Host "To install WSL:" -ForegroundColor Blue
    Write-Host "  1. Open PowerShell as Administrator" -ForegroundColor Cyan
    Write-Host "  2. Run: wsl --install" -ForegroundColor Cyan
    Write-Host "  3. Restart your computer" -ForegroundColor Cyan
    Write-Host "  4. After restart, run this script again" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Or install manually:" -ForegroundColor Blue
    Write-Host "  - Enable WSL: Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux" -ForegroundColor Cyan
    Write-Host "  - Install Ubuntu: wsl --install -d Ubuntu" -ForegroundColor Cyan
    Write-Host ""
    
    $installWSL = Read-Host "Would you like to install WSL now? (requires admin) (Y/N)"
    if ($installWSL -eq "Y" -or $installWSL -eq "y") {
        Write-Host ""
        Write-Host "Installing WSL..." -ForegroundColor Blue
        try {
            Start-Process powershell -Verb RunAs -ArgumentList "-Command", "wsl --install" -Wait
            Write-Host ""
            Write-Host "WSL installation initiated. Please restart your computer and run this script again." -ForegroundColor Green
        } catch {
            Write-Host "Failed to install WSL. Please run PowerShell as Administrator and run: wsl --install" -ForegroundColor Red
        }
    }
    exit 1
}

# If we get here, we're in WSL but the script wasn't run from bash
Write-Host "Please run this script from within WSL:" -ForegroundColor Yellow
Write-Host "  wsl" -ForegroundColor Cyan
Write-Host "  bash install-dependencies-windows.sh" -ForegroundColor Cyan
Write-Host ""
exit 1
