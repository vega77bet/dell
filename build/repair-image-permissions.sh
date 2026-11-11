#!/bin/bash
# Repair an existing development squashfs created before permission normalization.
# Build artifacts only; this script never accesses a physical disk.
set -euo pipefail
cd "$(dirname "$0")/.."
docker run --rm -v "$PWD:/src:ro" -v "$PWD/out:/work" dell-os-iso-tools sh -ec '
 if [ ! -s /work/iso/live/filesystem.squashfs ]; then
  test -s /work/dell-os-v0.1-amd64.iso
  xorriso -osirrox on -indev /work/dell-os-v0.1-amd64.iso -extract /live/filesystem.squashfs /work/iso/live/filesystem.squashfs
 fi
 if [ -e /work/repack-root ]; then
  grep -q "ID=dell-os" /work/repack-root/etc/os-release
 fi
 rm -f /work/dell-os-v0.1-amd64.iso
 unsquashfs -f -processors 2 -no-progress -d /work/repack-root /work/iso/live/filesystem.squashfs
 grep -q "ID=dell-os" /work/repack-root/etc/os-release
 cp -a /src/os/overlay/. /work/repack-root/
 find /src/out/stage /src/os/overlay -printf "/work/repack-root/%P\0" | xargs -0 chown 0:0
 find /src/out/stage /src/os/overlay -type d -printf "/work/repack-root/%P\0" | xargs -0 chmod 0755
 find /src/os/overlay -type f -printf "/work/repack-root/%P\0" | xargs -0 chmod 0644
 chmod 0755 /work/repack-root/usr/bin/dell-session /work/repack-root/usr/local/bin/dell-* /work/repack-root/usr/local/sbin/dell-* /work/repack-root/usr/local/bin/install-dell-os
 chmod 0440 /work/repack-root/etc/sudoers.d/dell-live
 apt-get update -qq
 cd /tmp
 apt-get download qt6-svg-plugins util-linux-extra systemd-timesyncd
 mkdir -p /work/repack-root/tmp/dell-extra-packages
 cp ./*.deb /work/repack-root/tmp/dell-extra-packages/
 chroot /work/repack-root /bin/sh -c "dpkg -i /tmp/dell-extra-packages/*.deb && rm -r /tmp/dell-extra-packages"
 chroot /work/repack-root systemctl enable systemd-timesyncd
 chroot /work/repack-root dpkg-query -W > /work/repack-root/usr/share/dell-os/package-manifest.txt
 chmod 0755 /work/repack-root
 rm -f /work/iso/live/filesystem.squashfs
 mksquashfs /work/repack-root /work/iso/live/filesystem.squashfs -noappend -root-mode 755 -root-uid 0 -root-gid 0 -comp zstd -Xcompression-level 8 -processors 2 -no-progress
 python3 /src/build/check-rootfs.py /work/iso/live/filesystem.squashfs
 rm -rf /work/repack-root
'
bash build/package.sh
