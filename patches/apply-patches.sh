#!/bin/bash
# Run from the root of the LineageOS 22.2 tree:
#   bash device/xiaomi/camellia/patches/apply-patches.sh
set -e
P=device/xiaomi/camellia/patches
for repo in build/soong hardware/mediatek hardware/st/nfc; do
  for patch in "$P/$repo"/*.patch; do
    if git -C "$repo" apply --reverse --check "$PWD/$patch" 2>/dev/null; then
      echo "already applied: $patch"
    else
      git -C "$repo" apply "$PWD/$patch" && echo "applied: $patch"
    fi
  done
done
