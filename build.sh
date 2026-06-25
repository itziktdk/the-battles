#!/bin/bash
# Build script for The Battles
# Usage: ./build.sh [linux|windows|clean]

set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"

case "${1:-linux}" in
    linux)
        echo "=== Building for Linux (native) ==="
        mkdir -p "$BUILD_DIR"
        cd "$BUILD_DIR"
        cmake "$PROJECT_DIR" -DCMAKE_BUILD_TYPE=Release
        make -j$(nproc)
        echo ""
        echo "✅ Build complete: $BUILD_DIR/TheBattles"
        echo "Run with: ./build/TheBattles"
        ;;
    
    windows)
        echo "=== Cross-compiling for Windows (MinGW-w64) ==="
        echo ""
        echo "NOTE: You need SFML 2.5+ for MinGW-w64 installed."
        echo "Download from: https://www.sfml-dev.org/download/sfml/2.5.1/"
        echo "Extract to: /usr/x86_64-w64-mingw32/ or set SFML_DIR"
        echo ""
        
        BUILD_WIN="$PROJECT_DIR/build-win"
        mkdir -p "$BUILD_WIN"
        cd "$BUILD_WIN"
        cmake "$PROJECT_DIR" \
            -DCMAKE_TOOLCHAIN_FILE="$PROJECT_DIR/toolchain-mingw64.cmake" \
            -DCMAKE_BUILD_TYPE=Release
        make -j$(nproc)
        
        echo ""
        echo "✅ Build complete: $BUILD_WIN/TheBattles.exe"
        echo ""
        echo "To package:"
        echo "  cp /path/to/sfml/bin/*.dll $BUILD_WIN/"
        echo "  cd $BUILD_WIN && zip TheBattles-v1.0-win64.zip TheBattles.exe *.dll"
        ;;
    
    clean)
        echo "=== Cleaning build directories ==="
        rm -rf "$PROJECT_DIR/build" "$PROJECT_DIR/build-win"
        echo "✅ Clean complete"
        ;;
    
    *)
        echo "Usage: $0 [linux|windows|clean]"
        exit 1
        ;;
esac
