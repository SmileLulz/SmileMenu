#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

echo "Cleaning..."

rm -rf \
    build \
    pkg \
    src \
    debian/.debhelper \
    debian/debhelper-build-stamp \
    debian/files \
    debian/*.substvars \
    debian/*.debhelper \
    debian/*.log \
    smilemenu-*.pkg.tar.zst \
    ../smilemenu_*.deb \
    ../smilemenu_*.buildinfo \
    ../smilemenu_*.changes

rm -f CMakeCache.txt Makefile cmake_install.cmake
rm -rf CMakeFiles _CPack_Packages CPackConfig.cmake CPackSourceConfig.cmake

printf '%s\n' "Cleanup complete"
