#!/bin/bash
# Boot only the installed disposable disk: no ISO and no USB image attached.
set -euo pipefail
cd "$(dirname "$0")/.."
test -s out/install-test.raw
mode="${1:-bios}"
test "$mode" = bios || test "$mode" = uefi
docker run --rm --user "$(id -u):$(id -g)" -v "$PWD:/work" dell-os-iso-tools bash -c '
set -euo pipefail
extra=()
if [[ "$1" == uefi ]]; then
 test -s /work/out/install-VARS.fd
 extra=(-drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd -drive if=pflash,format=raw,file=/work/out/install-VARS.fd)
fi
exec qemu-system-x86_64 -accel tcg -m 3072 -smp 2 \
 -drive file=/work/out/install-test.raw,format=raw,if=virtio,discard=unmap,detect-zeroes=unmap \
 -display none -vga std -usb -device usb-tablet \
 -qmp unix:/work/out/install.qmp,server=on,wait=off \
 -monitor unix:/work/out/install.monitor,server=on,wait=off \
 -serial file:/work/out/installed-serial.log -nic user,model=e1000 \
 -audiodev driver=none,id=snd0 -device ich9-intel-hda -device hda-duplex,audiodev=snd0 "${extra[@]}"
' bash "$mode"
