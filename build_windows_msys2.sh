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
require_command objdump

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

copy_dependency_tree() {
    local root_binary="$1"
    local mingw_bin="${MINGW_PREFIX:-/mingw64}/bin"

    local -a pending=("$root_binary")
    declare -A seen=()

    while ((${#pending[@]} > 0)); do
        local binary="${pending[0]}"
        pending=("${pending[@]:1}")

        while IFS= read -r dll_name; do
            # Remove possible Windows carriage return.
            dll_name="${dll_name//$'\r'/}"

            [[ -n "$dll_name" ]] || continue

            local key="${dll_name,,}"

            if [[ -n "${seen[$key]:-}" ]]; then
                continue
            fi

            seen["$key"]=1

            # Locate the DLL case-insensitively in the active MinGW bin folder.
            local dll_path
            dll_path="$(
                find "$mingw_bin" \
                    -maxdepth 1 \
                    -type f \
                    -iname "$dll_name" \
                    -print \
                    -quit
            )"

            if [[ -n "$dll_path" ]]; then
                cp -f "$dll_path" "$DIST_DIR/"
                echo "    Copied: $(basename "$dll_path")"

                # Inspect this DLL for its own dependencies.
                pending+=("$dll_path")
            fi
        done < <(
            objdump -p "$binary" |
                sed -n 's/^[[:space:]]*DLL Name:[[:space:]]*//p'
        )
    done
}

copy_dependency_tree "$EXE_PATH"

DLL_COUNT="$(
    find "$DIST_DIR" \
        -maxdepth 1 \
        -type f \
        -iname '*.dll' |
        wc -l |
        tr -d ' '
)"

if [[ "$DLL_COUNT" -eq 0 ]]; then
    echo "Error: No runtime DLLs were copied."
    echo ""
    echo "Dependencies reported by objdump:"
    objdump -p "$EXE_PATH" |
        sed -n 's/^[[:space:]]*DLL Name:[[:space:]]*/    /p'
    exit 1
fi

echo "==> Copied $DLL_COUNT runtime DLLs."

echo "==> Done. Windows release folder:"
echo "    $DIST_DIR"
echo ""
echo "Run this from MSYS2 to test:"
echo "    cd '$DIST_DIR'"
echo "    ./ExtinctionDefense.exe"
