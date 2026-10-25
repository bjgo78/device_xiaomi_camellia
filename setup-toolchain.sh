#!/bin/bash
#
# SPDX-FileCopyrightText: 2026 bjgo78
# SPDX-License-Identifier: Apache-2.0
#
# Install the clang toolchain the camellia kernel is built with.
#
# The kernel uses Clang LTO + CFI and the stock vendor modules were built
# with clang 11, so it stays on AOSP clang r383902 (clang 11.0.1). That
# version is not part of the LineageOS 22.2 manifest; this script puts it
# where BoardConfig.mk expects it:
#   prebuilts/clang/host/linux-x86/clang-r383902
#
# Usage (from anywhere):
#   bash device/xiaomi/camellia/setup-toolchain.sh             # download
#   bash device/xiaomi/camellia/setup-toolchain.sh FILE.tar.gz # use a local copy
#

set -e

CLANG_VERSION=r383902
CLANG_URL="https://android.googlesource.com/platform/prebuilts/clang/host/linux-x86/+archive/refs/heads/android12-release/clang-${CLANG_VERSION}.tar.gz"
EXPECTED="based on ${CLANG_VERSION}"

MY_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ANDROID_ROOT="$(cd "${MY_DIR}/../../.." && pwd)"
CLANG_DIR="${ANDROID_ROOT}/prebuilts/clang/host/linux-x86/clang-${CLANG_VERSION}"

die() { echo "ERROR: $*" >&2; exit 1; }

check_clang() {
    "$1/bin/clang" --version 2>/dev/null | head -n1 | grep -qF "${EXPECTED}"
}

[ -d "${ANDROID_ROOT}/prebuilts/clang/host/linux-x86" ] ||
    die "${ANDROID_ROOT} does not look like a synced LineageOS tree"

if [ -d "${CLANG_DIR}" ]; then
    if check_clang "${CLANG_DIR}"; then
        echo "clang-${CLANG_VERSION} is already installed:"
        "${CLANG_DIR}/bin/clang" --version | head -n1
        exit 0
    fi
    die "${CLANG_DIR} exists but is not clang ${CLANG_VERSION}; remove it and run again"
fi

WORK="$(mktemp -d "${ANDROID_ROOT}/prebuilts/clang/host/linux-x86/.clang-${CLANG_VERSION}.XXXXXX")"
trap 'rm -rf "${WORK}"' EXIT

if [ -n "$1" ]; then
    [ -f "$1" ] || die "$1 not found"
    TARBALL="$(realpath "$1")"
    echo "Using local archive ${TARBALL}"
else
    command -v curl >/dev/null || die "curl is not installed"
    TARBALL="${WORK}/clang.tar.gz"
    echo "Downloading clang-${CLANG_VERSION} (several hundred MB)..."
    curl -fL --retry 3 -o "${TARBALL}" "${CLANG_URL}" ||
        die "download failed"
fi

echo "Extracting..."
mkdir "${WORK}/clang"
# googlesource +archive tarballs have no top-level folder
tar -xzf "${TARBALL}" -C "${WORK}/clang" || die "archive is damaged"

check_clang "${WORK}/clang" || die "archive does not contain clang ${CLANG_VERSION}"

mv "${WORK}/clang" "${CLANG_DIR}"
echo "Installed to ${CLANG_DIR}:"
"${CLANG_DIR}/bin/clang" --version | head -n1
