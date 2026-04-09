# Build script for brsnmp on Windows
# Automatically sets up the MSYS2 32-bit MinGW environment and runs make

param(
    [ValidateSet('debug', 'release', 'clean', 'all')]
    [string]$Target = 'release',
    
    [switch]$Help
)

if ($Help) {
    Write-Host @"
brsnmp build script for Windows

Usage: .\build.ps1 [target] [-Help]

Targets:
  release (default)  Build release executable with optimizations
  debug              Build debug executable with symbols
  clean              Remove build outputs
  all                Equivalent to release

Options:
  -Help              Show this help message

Examples:
  .\build.ps1                 # Build release
  .\build.ps1 debug           # Build debug
  .\build.ps1 clean release   # Clean then build release
  .\build.ps1 -Help           # Show this help
"@
    exit 0
}

# Ensure MSYS2 32-bit MinGW is available
$mingw32_bin = 'C:\msys64\mingw32\bin'
if (-not (Test-Path $mingw32_bin)) {
    Write-Error @"
Error: MSYS2 32-bit MinGW compiler not found at $mingw32_bin

Please install MSYS2 from https://www.msys2.org/ and install the mingw-w64-i686-toolchain package.

Installation steps:
1. Download and run the MSYS2 installer from https://www.msys2.org/
2. Open MSYS2 MINGW32 terminal
3. Run: pacman -S --noconfirm --needed mingw-w64-i686-toolchain
"@
    exit 1
}

# Set up PATH for MSYS2 MinGW32
$env:Path = "$mingw32_bin;$env:Path"

# Run make with appropriate targets
Write-Host "Building $Target with 32-bit MinGW..." -ForegroundColor Cyan

$targets = if ($Target -eq 'all') { 'release' } else { $Target }
mingw32-make SHELL=cmd $targets

$exitCode = $LASTEXITCODE
if ($exitCode -eq 0) {
    Write-Host "`nBuild succeeded!" -ForegroundColor Green
    if ($Target -in @('release', 'all')) {
        Write-Host "Output: bin\Release\brsnmp.exe" -ForegroundColor Green
    }
} else {
    Write-Host "`nBuild failed with exit code $exitCode" -ForegroundColor Red
}

exit $exitCode
