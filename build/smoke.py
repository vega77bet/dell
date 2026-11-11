"""Boot the actual ISO through the BIOS and UEFI firmware (not direct kernel boot)."""
import argparse
import hashlib
import json
import pathlib
import shutil
import subprocess
import time

p = argparse.ArgumentParser()
p.add_argument('--timeout', type=int, default=600)
p.add_argument('--mode', choices=['bios', 'uefi', 'both'], default='both')
p.add_argument('--media', choices=['usb', 'cdrom'], default='usb')
args = p.parse_args()
out = pathlib.Path('/work/out')
(out / 'boot-report.json').unlink(missing_ok=True)
with open(out / 'dell-os-v0.1-amd64.iso', 'rb') as iso:
    digest = hashlib.file_digest(iso, 'sha256').hexdigest()
results = []
for mode in (['bios', 'uefi'] if args.mode == 'both' else [args.mode]):
    log = out / f'boot-{mode}.log'
    log.unlink(missing_ok=True)
    monitor = out / f'qemu-{mode}.sock'
    monitor.unlink(missing_ok=True)
    (out / f'qmp-{mode}.sock').unlink(missing_ok=True)
    cmd = ['qemu-system-x86_64', '-accel', 'tcg', '-m', '3072', '-smp', '2',
           '-display', 'none', '-vga', 'std', '-serial', f'file:{log}',
           '-monitor', f'unix:{monitor},server=on,wait=off',
           '-qmp', f'unix:{out}/qmp-{mode}.sock,server=on,wait=off',
           '-usb', '-device', 'usb-tablet',
           '-nic', 'user,model=e1000', '-no-reboot']
    if args.media == 'usb':
        cmd += ['-device', 'usb-ehci,id=usbtest',
                '-drive', f'if=none,id=stick,file={out}/dell-os-v0.1-amd64.iso,format=raw,readonly=on',
                '-device', 'usb-storage,bus=usbtest.0,drive=stick,bootindex=1']
    else:
        cmd += ['-cdrom', str(out / 'dell-os-v0.1-amd64.iso'), '-boot', 'd']
    if mode == 'uefi':
        code = pathlib.Path('/usr/share/OVMF/OVMF_CODE_4M.fd')
        template = pathlib.Path('/usr/share/OVMF/OVMF_VARS_4M.fd')
        var = out / 'OVMF_VARS-test.fd'
        shutil.copyfile(template, var)
        cmd += ['-drive', f'if=pflash,format=raw,readonly=on,file={code}',
                '-drive', f'if=pflash,format=raw,file={var}']
    start = time.monotonic()
    with open(out / f'qemu-{mode}.log', 'w') as err:
        proc = subprocess.Popen(cmd, stderr=err)
        ok = False
        try:
            while time.monotonic() - start < args.timeout and proc.poll() is None:
                if log.exists() and 'DELL_OS_DESKTOP_READY' in log.read_text(errors='replace'):
                    ok = True
                    break
                time.sleep(2)
            if proc.poll() is None:
                import socket
                with socket.socket(socket.AF_UNIX) as s:
                    s.connect(str(monitor))
                    s.sendall(f'screendump {out}/desktop-{mode}.ppm\n'.encode())
                    time.sleep(1)
        finally:
            proc.terminate()
            try:
                proc.wait(timeout=10)
            except subprocess.TimeoutExpired:
                proc.kill()
                proc.wait()
    result = {'firmware': mode, 'media': args.media, 'desktop_ready': ok, 'seconds': round(time.monotonic()-start, 1), 'ram_mib': 3072, 'iso_sha256': digest}
    print(json.dumps(result), flush=True)
    results.append(result)
(out / 'boot-report.json').write_text(json.dumps(results, indent=2)+'\n')
raise SystemExit(0 if all(r['desktop_ready'] for r in results) else 1)
