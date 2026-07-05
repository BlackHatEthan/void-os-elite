@echo off
REM Void OS Elite - Windows (WSL required)
REM Run: install-windows.bat
REM Or use PowerShell: .\install-windows.ps1

echo ==========================================
echo   Void OS Elite - Windows
echo ==========================================
echo.

where wsl >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo WSL detected. Running Linux installer inside WSL...
    echo.
    for /f "delims=" %%i in ('wsl wslpath -u "%CD%" 2^>nul') do set "WSLDIR=%%i"
    if defined WSLDIR (
        wsl bash -c "cd '%WSLDIR%' && ./install-linux.sh"
    ) else (
        echo Could not resolve path. Run from WSL instead: cd voidos-elite ^&^& ./install-linux.sh
    )
    exit /b 0
)

echo Void OS Elite runs on Linux and macOS.
echo On Windows, use WSL (Windows Subsystem for Linux).
echo.
echo 1. Install WSL:  wsl --install
echo 2. Restart if prompted, then open Ubuntu from Start
echo 3. In WSL: git clone https://github.com/BlackHatEthan/void-os-elite.git
echo 4. In WSL: cd void-os-elite  ^&^&  ./install-linux.sh
echo.
echo See INSTALL.md for details.
pause
