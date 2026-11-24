#!/bin/bash
# Run from the root of the VoltageOS tree:
#   bash device/xiaomi/camellia/patches/apply-patches.sh
set -e
P=device/xiaomi/camellia/patches
for repo in device/mediatek/sepolicy_vndr frameworks/base hardware/mediatek hardware/st/nfc vendor/gapps; do
  for patch in "$P/$repo"/*.patch; do
    if git -C "$repo" apply --reverse --check "$PWD/$patch" 2>/dev/null; then
      echo "already applied: $patch"
    else
      git -C "$repo" apply "$PWD/$patch" && echo "applied: $patch"
    fi
  done
done
