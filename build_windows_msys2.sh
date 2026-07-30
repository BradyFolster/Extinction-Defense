#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-windows"
DIST_ROOT="$PROJECT_ROOT/dist/windows"
DIST_DIR="$DIST_ROOT/ExtinctionDefense"
EXE_NAME="ExtinctionDefense.exe"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

cd "$PROJECT_ROOT"

if [[ "${MSYSTEM:-}" != MINGW64 && "${MSYSTEM:-}" != UCRT64 && "${MSYSTEM:-}" != CLANG64 ]]; then
    echo "Warning: This script should be run from an MSYS2 MinGW shell, not plain MSYS."
    echo "Current MSYSTEM='${MSYSTEM:-unknown}'. MINGW64 is recommended."
fi

require_command(){
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "Missing required command: $1"
        echo "Install dependencies in MSYS2, for example:"
        echo "  pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc mingw-w64-x86_64-pkgconf mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-nlohmann-json make"
        exit 1
    fi
}

require_command cmake
require_command mingw32-make
require_command pkg-config

GENERATOR="MinGW Makefiles"

echo "==> Configuring Windows build..."
cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" -G "$GENERATOR" -DCMAKE_BUILD_TYPE=Release

echo "==> Building ExtinctionDefense..."
cmake --build "$BUILD_DIR" --config Release --parallel "$JOBS" --target ExtinctionDefense

EXE_PATH="$BUILD_DIR/$EXE_NAME"
if [[ ! -f "$EXE_PATH" ]]; then
    echo "Could not find built executable: $EXE_PATH"
    exit 1
fi

echo "==> Creating release folder..."
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

cp "$EXE_PATH" "$DIST_DIR/"
cp -r "$PROJECT_ROOT/assets" "$DIST_DIR/"

if [[ -f "$PROJECT_ROOT/settings.json" ]]; then
    cp "$PROJECT_ROOT/settings.json" "$DIST_DIR/"
fi

copy_dll(){
    local dll_path="$1"

    if [[ -f "$dll_path" ]]; then
        cp -n "$dll_path" "$DIST_DIR/"
    fi
}

echo "==> Copying runtime DLLs..."
if command -v ntldd >/dev/null 2>&1; then
    while IFS= read -r dll_path; do
        copy_dll "$dll_path"
    done < <(ntldd -R "$EXE_PATH" | awk '/=> \/mingw/ {print $3}')
else
    echo "ntldd not found; copying common SDL/runtime DLLs from the active MinGW bin directory."
    echo "For better automatic DLL collection, install ntldd: pacman -S mingw-w64-x86_64-ntldd"

    MINGW_BIN="$(dirname "$(command -v gcc)")"
    for dll in \
        SDL2.dll \
        SDL2_image.dll \
        SDL2_ttf.dll \
        SDL2_mixer.dll \
        libgcc_s_seh-1.dll \
        libstdc++-6.dll \
        libwinpthread-1.dll; do
        copy_dll "$MINGW_BIN/$dll"
    done
fi

echo "==> Done. Windows release folder:"
echo "    $DIST_DIR"
echo ""
echo "Run this from MSYS2 to test:"
echo "    cd '$DIST_DIR'"
echo "    ./ExtinctionDefense.exe"
