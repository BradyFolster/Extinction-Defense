#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-linux"
DIST_DIR="$PROJECT_ROOT/dist/linux-build"
EXE_NAME="ExtinctionDefense"
ICON_PATH="$PROJECT_ROOT/assets/images/icon.png"
JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"

cd "$PROJECT_ROOT"

require_command(){
    if ! command -v "$1" >/dev/null 2>&1; then
        echo "Missing required command: $1"
        echo "Install the required Linux build dependencies, including cmake, make, pkg-config, SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, and nlohmann-json."
        exit 1
    fi
}

require_command cmake
require_command make
require_command pkg-config
require_command ldd

if [[ ! -f "$ICON_PATH" ]]; then
    echo "Warning: Linux icon not found: $ICON_PATH"
    echo "The build will continue without copying a launcher icon."
fi

echo "==> Configuring Linux build..."
cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

echo "==> Building ExtinctionDefense..."
cmake --build "$BUILD_DIR" --config Release --parallel "$JOBS" --target ExtinctionDefense

EXE_PATH="$BUILD_DIR/$EXE_NAME"
if [[ ! -f "$EXE_PATH" ]]; then
    echo "Could not find built executable: $EXE_PATH"
    exit 1
fi

echo "==> Creating Linux release folder..."
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"
mkdir -p "$DIST_DIR/lib"

cp "$EXE_PATH" "$DIST_DIR/"
cp -r "$PROJECT_ROOT/assets" "$DIST_DIR/"

if [[ -f "$PROJECT_ROOT/settings.json" ]]; then
    cp "$PROJECT_ROOT/settings.json" "$DIST_DIR/"
fi

if [[ -f "$ICON_PATH" ]]; then
    cp "$ICON_PATH" "$DIST_DIR/extinction-defense.png"
fi

copy_linux_libraries(){
    local binary="$1"
    local lib_dir="$2"

    while IFS= read -r lib_path; do
        [[ -n "$lib_path" ]] || continue

        local lib_name
        lib_name="$(basename "$lib_path")"

        case "$lib_path" in
            /lib/*|/lib64/*|/usr/lib/*|/usr/lib64/*)
                case "$lib_name" in
                    linux-vdso.so.*|ld-linux*.so.*|libc.so.*|libm.so.*|libdl.so.*|libpthread.so.*|librt.so.*|libresolv.so.*|libnsl.so.*|libutil.so.*)
                        continue
                        ;;
                esac
                ;;
        esac

        cp -n "$lib_path" "$lib_dir/"
        echo "    Copied: $lib_name"
    done < <(ldd "$binary" | awk '
        /=> \/.* \(/ {print $3}
        /^	\/.* \(/ {print $1}
    ')
}

echo "==> Copying Linux shared libraries..."
copy_linux_libraries "$EXE_PATH" "$DIST_DIR/lib"

LIB_COUNT="$(
    find "$DIST_DIR/lib" \
        -maxdepth 1 \
        -type f \
        -name '*.so*' |
        wc -l |
        tr -d ' '
)"

echo "==> Copied $LIB_COUNT shared libraries."

cat > "$DIST_DIR/run.sh" <<'RUNEOF'
#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"
export LD_LIBRARY_PATH="$PWD/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
export SDL_VIDEO_X11_WMCLASS="ExtinctionDefense"
exec ./ExtinctionDefense
RUNEOF
chmod +x "$DIST_DIR/run.sh"

cat > "$DIST_DIR/ExtinctionDefense.desktop" <<DESKTOPEOF
[Desktop Entry]
Type=Application
Name=Extinction Defense
Comment=A dinosaur tower defense game
Exec=$DIST_DIR/run.sh
Icon=$DIST_DIR/extinction-defense.png
Path=$DIST_DIR
StartupWMClass=ExtinctionDefense
Terminal=false
Categories=Game;
DESKTOPEOF

chmod +x "$DIST_DIR/ExtinctionDefense.desktop"

echo "==> Done. Linux release folder:"
echo "    $DIST_DIR"
echo ""
echo "Run this to test:"
echo "    cd '$DIST_DIR'"
echo "    ./run.sh"
