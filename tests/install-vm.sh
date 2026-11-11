#!/bin/bash
# Disposable installation test. Only the explicitly named sparse image is created.
set -euo pipefail
cd "$(dirname "$0")/.."
mode="${1:-bios}"
test "$mode" = bios || test "$mode" = uefi
test -s out/dell-os-v0.1-amd64.iso
if [[ -e out/install-test.raw ]]; then
    printf 'out/install-test.raw already exists; refusing to replace it.\n' >&2
    exit 1
fi
docker run --rm --user "$(id -u):$(id -g)" -v "$PWD:/work" dell-os-iso-tools bash -c '
set -euo pipefail
truncate -s 20G /work/out/install-test.raw
extra=()
if [[ "$1" == uefi ]]; then
 cp /usr/share/OVMF/OVMF_VARS_4M.fd /work/out/install-VARS.fd
 extra=(-drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE_4M.fd -drive if=pflash,format=raw,file=/work/out/install-VARS.fd)
fi
exec qemu-system-x86_64 -accel tcg -m 3072 -smp 2 \
 -cdrom /work/out/dell-os-v0.1-amd64.iso -boot once=d \
 -drive file=/work/out/install-test.raw,format=raw,if=virtio,discard=unmap,detect-zeroes=unmap \
 -display none -vga std -usb -device usb-tablet \
 -qmp unix:/work/out/install.qmp,server=on,wait=off \
 -monitor unix:/work/out/install.monitor,server=on,wait=off \
 -serial file:/work/out/install-serial.log -nic user,model=e1000 \
 -audiodev driver=none,id=snd0 -device ich9-intel-hda -device hda-duplex,audiodev=snd0 "${extra[@]}"
' bash "$mode"
