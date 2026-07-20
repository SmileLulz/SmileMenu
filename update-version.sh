#!/bin/bash

set -e

PYPROJECT="pyproject.toml"
PKGBUILD="PKGBUILD"
CHANGELOG="debian/changelog"

CHANGELOG_MAINTAINER="SmileLulz <>"
PKGBUILD_MAINTAINER="SmileLulz <smilelulz@noreply.codeberg.org>"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

usage() {
    cat << EOF
${YELLOW}Usage:${NC}
  $0 -mj|--major          Bump major version (1.4.3 -> 2.0.0)
  $0 -mn|--minor          Bump minor version (1.4.3 -> 1.5.0)
  $0 -pc|--patch          Bump patch version (1.4.3 -> 1.4.4)
  $0 -s|--set X.Y.Z       Set explicit version
EOF
    exit 1
}

get_current_version() {
    grep -m1 '^version = ' "$PYPROJECT" | cut -d'"' -f2
}

get_pkgname() {
    grep -m1 '^pkgname=' "$PKGBUILD" | cut -d'=' -f2
}

bump_version() {
    local current="$1"
    local type="$2"

    IFS='.' read -r major minor patch <<< "$current"

    case "$type" in
        major) echo "$((major + 1)).0.0" ;;
        minor) echo "${major}.$((minor + 1)).0" ;;
        patch) echo "${major}.${minor}.$((patch + 1))" ;;
    esac
}

update_files() {
    local new_version="$1"
    local pkgname=$(get_pkgname)
    local full_version="${new_version}-1"
    local date=$(date -R)

    sed -i "s/^version = \".*\"/version = \"${new_version}\"/" "$PYPROJECT"
    sed -i "s/^pkgver=.*/pkgver=${new_version}/" "$PKGBUILD"

    if grep -q '^# Maintainer:' "$PKGBUILD"; then
        sed -i "s/^# Maintainer:.*/# Maintainer: ${PKGBUILD_MAINTAINER}/" "$PKGBUILD"
    fi

    local new_entry="${pkgname} (${full_version}) unstable; urgency=medium

  * Version bump to ${new_version}

 -- ${CHANGELOG_MAINTAINER}  ${date}

"
    echo -e "${new_entry}$(cat "$CHANGELOG")" > "$CHANGELOG"

    echo -e "${GREEN}✓ Updated pyproject.toml${NC}"
    echo -e "${GREEN}✓ Updated PKGBUILD${NC}"
    echo -e "${GREEN}✓ Updated debian/changelog${NC}"
}

if [[ $# -lt 1 ]]; then
    usage
fi

case "$1" in
    -mj|--major|-mn|--minor|-pc|--patch)
        current_version=$(get_current_version)
        bump_type="${1#-}"
        bump_type="${bump_type#-}"
        case "$bump_type" in
            mj|major) new_version=$(bump_version "$current_version" "major") ;;
            mn|minor) new_version=$(bump_version "$current_version" "minor") ;;
            pc|patch) new_version=$(bump_version "$current_version" "patch") ;;
        esac
        echo -e "${YELLOW}Bumping ${bump_type} version: ${current_version} -> ${new_version}${NC}"
        ;;
    -s|--set)
        if [[ -z "$2" ]]; then
            echo -e "${RED}Error: Version required with --set${NC}"
            usage
        fi
        new_version="$2"
        echo -e "${YELLOW}Setting version to: ${new_version}${NC}"
        ;;
    *)
        echo -e "${RED}Error: Unknown option '$1'${NC}"
        usage
        ;;
esac

if ! [[ "$new_version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo -e "${RED}Error: Invalid version format. Use X.Y.Z${NC}"
    exit 1
fi

update_files "$new_version"

echo -e "${GREEN}✓ Version successfully updated to ${new_version}-1${NC}"