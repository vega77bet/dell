#!/bin/bash
# Repackage a previously built ISO tree, without rebuilding its root filesystem.
set -euo pipefail
cd "$(dirname "$0")/.."
test -s out/iso/live/filesystem.squashfs
test -s out/iso/live/vmlinuz
test -s out/iso/live/initrd.img
docker run --rm -v "$PWD:/src:ro" dell-os-iso-tools python3 /src/build/check-rootfs.py /src/out/iso/live/filesystem.squashfs
docker run --rm -v "$PWD/out:/work" dell-os-iso-tools grub-mkrescue -o /work/dell-os-v0.1-amd64.iso /work/iso -- -volid DELL_OS_01
(cd out && sha256sum dell-os-v0.1-amd64.iso > dell-os-v0.1-amd64.iso.sha256)
docker run --rm -v "$PWD/out:/work" dell-os-iso-tools xorriso -indev /work/dell-os-v0.1-amd64.iso -report_el_torito plain -report_system_area plain >out/iso-structure.txt 2>&1
printf '\nISO pronta: %s/out/dell-os-v0.1-amd64.iso\n' "$PWD"
