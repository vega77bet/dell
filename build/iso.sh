#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
mkdir -p out/iso/live out/iso/boot/grub
if [[ ! -x out/stage/usr/bin/dell-shell ]]; then bash build/sdk.sh; fi
if [[ "${1:-}" == --reuse-images ]]; then
    docker image inspect dell-os-system dell-os-iso-tools >/dev/null
else
    docker build --target system -f build/Dockerfile -t dell-os-system . 2>&1 | tee out/system-build.log
    docker build --target iso-tools -f build/Dockerfile -t dell-os-iso-tools . 2>&1 | tee out/tools-build.log
fi
driver=$(docker info --format '{{.Driver}}')
source_dir=""
if [[ "$driver" == vfs ]]; then
    source_dir=$(docker image inspect dell-os-system --format '{{index .GraphDriver.Data "SourceDir"}}')
fi
container=""
trap 'if [[ -n "$container" ]]; then docker rm "$container" >/dev/null; fi' EXIT
if [[ "$driver" == vfs && "$source_dir" == /* ]]; then
    # VFS creates a full rootfs copy for every container. Read the completed image
    # snapshot without modifying it on low-space VFS builders.
    mkdir -p out/kernel
    docker run --rm -v "$source_dir:/rootfs:ro" -v "$PWD/out:/work" dell-os-iso-tools cp -a /rootfs/boot/. /work/kernel/
else
    container=$(docker create dell-os-system)
    docker cp "$container:/boot/." out/kernel
fi
kernel=$(find out/kernel -maxdepth 1 -name 'vmlinuz-*' -type f | sort -V | tail -1)
initrd=$(find out/kernel -maxdepth 1 -name 'initrd.img-*' -type f | sort -V | tail -1)
test -n "$kernel" && test -n "$initrd"
cp "$kernel" out/iso/live/vmlinuz
cp "$initrd" out/iso/live/initrd.img
cp build/grub.cfg out/iso/boot/grub/grub.cfg
# Stream the exported rootfs directly: no privileged mounts, loop devices or duplicate rootfs tree.
if [[ -z "$container" ]]; then
    docker run --rm -v "$source_dir:/rootfs:ro" -v "$PWD/out:/work" dell-os-iso-tools mksquashfs /rootfs /work/iso/live/filesystem.squashfs -noappend -root-mode 755 -root-uid 0 -root-gid 0 -comp zstd -Xcompression-level 8 -processors 2 -no-progress
else
    docker export "$container" | docker run --rm -i -v "$PWD/out:/work" dell-os-iso-tools mksquashfs - /work/iso/live/filesystem.squashfs -tar -noappend -root-mode 755 -root-uid 0 -root-gid 0 -comp zstd -Xcompression-level 8 -processors 2 -no-progress
fi
bash build/package.sh
