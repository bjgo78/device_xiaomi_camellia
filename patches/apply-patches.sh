#!/bin/bash
# Run from the root of the SuperiorOS tree:
#   bash device/xiaomi/camellia/patches/apply-patches.sh
set -e
P=device/xiaomi/camellia/patches
for repo in frameworks/base hardware/mediatek hardware/st/nfc; do
  for patch in "$P/$repo"/*.patch; do
    files=$(git -C "$repo" apply --numstat "$PWD/$patch" | cut -f3)
    # The synced repos are clean, so untouched files mean the patch is not applied yet.
    # (A reverse --check alone is not enough: for a patch that only deletes lines it can
    # succeed on the unpatched file too.)
    if git -C "$repo" diff --quiet -- $files; then
      git -C "$repo" apply "$PWD/$patch" && echo "applied: $patch"
    elif git -C "$repo" apply --reverse --check "$PWD/$patch" 2>/dev/null; then
      echo "already applied: $patch"
    else
      echo "FAILED: $patch (its files are changed, but not by this patch)" >&2
      exit 1
    fi
  done
done
