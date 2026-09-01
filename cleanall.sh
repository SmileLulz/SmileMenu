#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

echo "Cleaning..."

rm -rf \
    build \
    pkg \
    src \
    obj-*-linux-gnu \
    debian/smilemenu \
    debian/.debhelper \
    debian/debhelper-build-stamp \
    debian/files \
    debian/*.substvars \
    debian/*.debhelper \
    debian/*.log \
    rpm/BUILD/* \
    rpm/BUILDROOT/* \
    rpm/RPMS/* \
    rpm/SOURCES/* \
    rpm/SRPMS/* \
    smilemenu-*.pkg.tar.zst \
    ../smilemenu_*.deb \
    ../smilemenu_*.buildinfo \
    ../smilemenu_*.changes \

rm -f CMakeCache.txt Makefile cmake_install.cmake
rm -rf CMakeFiles _CPack_Packages CPackConfig.cmake CPackSourceConfig.cmake

printf '%s\n' "Cleanup complete"
