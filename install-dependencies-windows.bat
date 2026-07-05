@echo off
REM install-dependencies-windows.bat
REM Installs all dependencies needed to build and run Void OS Elite on Windows
REM Note: Void OS Elite runs in WSL (Windows Subsystem for Linux) on Windows

echo ==========================================
echo   Void OS Elite - Windows Dependencies
echo ==========================================
echo.

REM Check for WSL
echo Checking for WSL (Windows Subsystem for Linux)...
wsl --list >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo WSL is installed.
    echo.
    echo Available WSL distributions:
    wsl --list
    echo.
    echo To install dependencies, run this script from within WSL:
    echo   wsl
    echo   cd /mnt/c/path/to/voidos-elite
    echo   bash install-dependencies-linux.sh
    echo.
    pause
    exit /b 0
) else (
    echo WSL is not installed.
    echo.
    echo Void OS Elite requires WSL to run on Windows.
    echo.
    echo To install WSL:
    echo   1. Open PowerShell as Administrator
    echo   2. Run: wsl --install
    echo   3. Restart your computer
    echo   4. After restart, run this script again
    echo.
    echo Or install manually:
    echo   - Enable WSL: Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Windows-Subsystem-Linux
    echo   - Install Ubuntu: wsl --install -d Ubuntu
    echo.
    set /p installWSL="Would you like to install WSL now? (requires admin) (Y/N): "
    if /i "%installWSL%"=="Y" (
        echo.
        echo Installing WSL...
        powershell -Command "Start-Process powershell -Verb RunAs -ArgumentList '-Command', 'wsl --install' -Wait"
        echo.
        echo WSL installation initiated. Please restart your computer and run this script again.
    )
    pause
    exit /b 1
)

pause
