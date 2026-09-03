#!/usr/bin/env bash

set -euo pipefail

CMAKEFILE="CMakeLists.txt"
PKGBUILD="PKGBUILD"
CHANGELOG="CHANGELOG.md"
CONTROL="debian/control"
DEBIAN_CHANGELOG="debian/changelog"
SPEC="rpm/SPECS/smilemenu.spec"

ARCH_MAINTAINER="SmileLulz <SmileLulz@users.noreply.github.com>"
DEBIAN_MAINTAINER="SmileLulz <SmileLulz@users.noreply.github.com>"
RPM_MAINTAINER="SmileLulz"

DEBIAN_DISTRIBUTION="bookworm" # bookworm or unstable

RED=$'\033[0;31m'
GREEN=$'\033[0;32m'
YELLOW=$'\033[1;33m'
NC=$'\033[0m'

usage() {
    cat <<USAGE
Usage:
  $0 1|--major          Bump major version (2.0 -> 3.0)
  $0 2|--minor          Bump minor version (2.0 -> 2.1)
  $0 -s|--set X.Y[.N]   Set explicit version (e.g. 4.0 or 4.0.1)
USAGE
    exit 1
}

get_current_version() {
    sed -nE '
        /^[[:space:]]*project\(SmileMenu[[:space:]]*$/,/^[[:space:]]*\)/ {
            s/^[[:space:]]*VERSION[[:space:]]+([0-9]+\.[0-9]+(\.[0-9]+)?).*$/\1/p
        }
        /^[[:space:]]*project\(SmileMenu[[:space:]]+VERSION[[:space:]]+/ {
            s/.*VERSION[[:space:]]+([0-9]+\.[0-9]+(\.[0-9]+)?).*$/\1/p
        }
    ' "$CMAKEFILE" | head -n1
}

get_pkgname() {
    sed -nE 's/^pkgname=([^[:space:]]+)$/\1/p' "$PKGBUILD" | head -n1
}

validate_version() {
    [[ "$1" =~ ^[0-9]+\.[0-9]+([.][0-9]+)?$ ]]
}

changelog_entries() {
    local version="$1"

    awk -v version="$version" '
        $0 ~ "^### v" version "[[:space:]]*$" {
            in_section=1
            next
        }

        in_section && $0 ~ /^### v/ {
            exit
        }

        in_section && $0 ~ /^- / {
            print substr($0, 3)
        }
    ' "$CHANGELOG"
}

validate_changelog() {
    local version="$1"
    local entries

    entries=$(changelog_entries "$version")

    if [ -z "$entries" ]; then
        echo "${RED}Error: CHANGELOG.md must contain '### v${version}' with at least one '- ' entry.${NC}" >&2
        exit 1
    fi

    echo "${GREEN}Changelog validated: v${version}${NC}"

    while IFS= read -r entry; do
        printf '  - %s\n' "$entry"
    done <<< "$entries"
}

bump_version() {
    local current="$1"
    local type="$2"
    local major minor

    if [[ ! "$current" =~ ^[0-9]+\.[0-9]+$ ]]; then
        echo "${RED}Error: --major/--minor require a clean X.Y version; current version is '$current'.${NC}" >&2
        echo "${YELLOW}Use --set for X.Y.N test versions.${NC}" >&2
        exit 1
    fi

    IFS=. read -r major minor <<< "$current"

    case "$type" in
        major)
            printf '%s.0\n' "$((major + 1))"
            ;;
        minor)
            printf '%s.%s\n' "$major" "$((minor + 1))"
            ;;
        *)
            exit 2
            ;;
    esac
}

update_cmake() {
    local version="$1"

    if grep -qE '^[[:space:]]*project\(SmileMenu[[:space:]]+VERSION[[:space:]]+' "$CMAKEFILE"; then
        sed -i -E \
            "s/^([[:space:]]*project\(SmileMenu[[:space:]]+VERSION[[:space:]]+)[0-9]+\.[0-9]+(\.[0-9]+)?/\1${version}/" \
            "$CMAKEFILE"
        return
    fi

    sed -i -E \
        "/^[[:space:]]*project\(SmileMenu[[:space:]]*$/,/^[[:space:]]*\)/ s/^([[:space:]]*VERSION[[:space:]]+)[0-9]+\.[0-9]+(\.[0-9]+)?/\1${version}/" \
        "$CMAKEFILE"
}

update_pkgbuild() {
    local version="$1"

    sed -i -E "s/^pkgver=.*/pkgver=${version}/" "$PKGBUILD"
    sed -i -E "s/^# Maintainer:.*/# Maintainer: ${ARCH_MAINTAINER}/" "$PKGBUILD"
}

update_control() {
    [ -f "$CONTROL" ] || return 0

    sed -i -E "s/^Maintainer:.*/Maintainer: ${DEBIAN_MAINTAINER}/" "$CONTROL"
}

update_debian_changelog() {
    [ -f "$DEBIAN_CHANGELOG" ] || return 0

    local version="$1"
    local pkgname
    local entries
    local tmp

    pkgname=$(get_pkgname)
    entries=$(changelog_entries "$version" | sed 's/^/  * /')

    tmp=$(mktemp)

    {
        printf '%s (%s-1) %s; urgency=medium\n\n' \
            "$pkgname" \
            "$version" \
            "$DEBIAN_DISTRIBUTION"

        printf '%s\n\n' "$entries"

        printf ' -- %s  %s\n\n' \
            "$DEBIAN_MAINTAINER" \
            "$(date -R)"

        cat "$DEBIAN_CHANGELOG"
    } > "$tmp"

    mv "$tmp" "$DEBIAN_CHANGELOG"
}

update_spec() {
    [ -f "$SPEC" ] || return 0

    local version="$1"
    local entries
    local date
    local tmp

    entries=$(changelog_entries "$version")

    sed -i -E \
        "s/^Version:[[:space:]]+.*/Version:        ${version}/" \
        "$SPEC"

    sed -i -E \
        "s/^Release:[[:space:]]+.*/Release:        1%{?dist}/" \
        "$SPEC"

    sed -i -E \
        "s|^URL:[[:space:]]+.*|URL:            https://github.com/SmileLulz/SmileMenu|" \
        "$SPEC"

    date=$(LC_ALL=C date '+%a %b %d %Y')

    tmp=$(mktemp)

    {
        awk '/^%changelog$/ { print; exit } { print }' "$SPEC"

        printf '* %s %s - %s-1\n' \
            "$date" \
            "$RPM_MAINTAINER" \
            "$version"

        while IFS= read -r entry; do
            printf -- '- %s\n' "$entry"
        done <<< "$entries"

        # printf '\n'
    } > "$tmp"

    mv "$tmp" "$SPEC"
}

if (($# == 0)); then
    usage
fi

case "$1" in
    1|--major)
        current=$(get_current_version)
        new_version=$(bump_version "$current" major)
        ;;
    2|--minor)
        current=$(get_current_version)
        new_version=$(bump_version "$current" minor)
        ;;
    -s|--set)
        (($# >= 2)) || {
            echo "${RED}Error: --set requires a version.${NC}" >&2
            usage
        }

        new_version="$2"
        ;;
    *)
        echo "${RED}Error: Unknown option '$1'.${NC}" >&2
        usage
        ;;
esac

validate_version "$new_version" || {
    echo "${RED}Error: invalid version '$new_version'. Use X.Y or X.Y.N.${NC}" >&2
    exit 1
}

validate_changelog "$new_version"

current=$(get_current_version)

echo "${YELLOW}Updating version: ${current} -> ${new_version}${NC}"

update_cmake "$new_version"
update_pkgbuild "$new_version"
update_control
update_debian_changelog "$new_version"
update_spec "$new_version"

printf '%sVersion updated to v%s%s\n' \
    "$GREEN" \
    "$new_version" \
    "$NC"
