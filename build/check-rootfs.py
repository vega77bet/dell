"""Check traversal permissions before calling an image bootable."""
import pathlib
import subprocess
import sys

lines = subprocess.check_output(['unsquashfs', '-lln', sys.argv[1]], text=True).splitlines()
metadata = {}
owners = {}
for line in lines:
    parts = line.split(None, 5)
    if len(parts) == 6:
        metadata[parts[5]] = parts[0]
        owners[parts[5]] = parts[1]
required = ['', 'etc', 'usr', 'usr/bin', 'usr/share', 'usr/local', 'usr/local/bin',
            'etc/calamares', 'usr/share/xsessions']
for path in required:
    full = 'squashfs-root' + ('/'+path if path else '')
    mode = metadata.get(full, '')
    if not (len(mode) >= 10 and mode[0] == 'd' and mode[7] == 'r' and mode[9] == 'x'):
        raise SystemExit(f'Invalid system directory permissions: /{path}: {mode}')
    if owners.get(full) != '0/0':
        raise SystemExit(f'Invalid system directory ownership: /{path}: {owners.get(full)}')
print('Root filesystem traversal permissions: OK')
for path in ['etc/xdg/openbox/dell-rc.xml', 'etc/xdg/dunst/dunstrc',
             'usr/share/dell-os/start/index.html', 'usr/share/xsessions/dell.desktop',
             'usr/local/bin/install-dell-os', 'usr/local/sbin/dell-finalize']:
    mode = metadata.get('squashfs-root/'+path, '')
    if len(mode) < 10 or mode[7] != 'r':
        raise SystemExit(f'Unreadable system resource: /{path}: {mode}')
    if path.startswith('usr/local/') and mode[9] != 'x':
        raise SystemExit(f'Unexecutable script: /{path}: {mode}')
    if owners.get('squashfs-root/'+path) != '0/0':
        raise SystemExit(f'Non-root-owned system resource: /{path}')
for path in ['etc/sudoers.d', 'etc/sudoers.d/dell-live', 'usr/bin/dell-app', 'usr/bin/dell-shell', 'usr/bin/dell-session']:
    if owners.get('squashfs-root/'+path) != '0/0':
        raise SystemExit(f'Invalid privileged resource ownership: /{path}')
print('System resource and script permissions: OK')
for path in ['usr/lib/x86_64-linux-gnu/qt6/plugins/iconengines/libqsvgicon.so',
             'usr/lib/x86_64-linux-gnu/qt6/plugins/imageformats/libqsvg.so']:
    if 'squashfs-root/'+path not in metadata:
        raise SystemExit('Missing Qt SVG plugin: /'+path)
print('SVG icon and image plugins: OK')
for path in ['usr/sbin/hwclock', 'usr/lib/systemd/systemd-timesyncd']:
    mode = metadata.get('squashfs-root/'+path, '')
    if len(mode) < 10 or mode[9] != 'x':
        raise SystemExit('Missing clock utility: /'+path)
print('Clock utilities: OK')
