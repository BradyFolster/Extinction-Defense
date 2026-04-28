#!/usr/bin/env bash

# Exit immediately if any command fails

set -e

echo "==> Removing old build directory..."
rm -rf build

echo "==> Creating build directory (if it doesn't exist)..."
mkdir -p build

echo "==> Entering build directory..."
cd build

echo "==> Running CMake..."
cmake ..

echo "==> Building project..."
make -j$(nproc)

echo "==> Running game..."
cd ..
./build/ExtinctionDefense
