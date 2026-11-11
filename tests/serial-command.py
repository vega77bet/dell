"""Issue diagnostic commands to the disposable install VM's Live console."""
import argparse
import json
import socket
import time

p = argparse.ArgumentParser()
p.add_argument('--login', action='store_true')
p.add_argument('--socket', default='/workspace/out/install.serial')
p.add_argument('--qmp', default='/workspace/out/install.qmp')
p.add_argument('--wait', type=float, default=1)
p.add_argument('--user', default='live')
p.add_argument('--password', default='live', help='Disposable VM credentials only')
p.add_argument('command')
args = p.parse_args()
if args.login:
    with socket.socket(socket.AF_UNIX) as q:
        q.connect(args.qmp)
        f = q.makefile('rwb', buffering=0)
        f.readline()
        for call in [{'execute':'qmp_capabilities'}, {'execute':'human-monitor-command', 'arguments':{'command-line':'chardev-change serial0 socket,path=/work/out/install.serial,server=on,wait=off'}}]:
            f.write((json.dumps(call)+'\n').encode())
            while True:
                result = json.loads(f.readline())
                if 'error' in result:
                    raise RuntimeError(result)
                if 'return' in result:
                    break
with socket.socket(socket.AF_UNIX) as s:
    s.connect(args.socket)
    s.settimeout(.5)
    def drain():
        data = b''
        while True:
            try:
                chunk = s.recv(65536)
                if not chunk:
                    break
                data += chunk
            except TimeoutError:
                break
        print(data.decode(errors='replace'), end='', flush=True)
    if args.login:
        for message, pause in [('\r',1), (args.user+'\r',1), (args.password+'\r',3)]:
            s.sendall(message.encode());time.sleep(pause);drain()
    s.sendall((args.command+'\r').encode())
    time.sleep(args.wait)
    drain()
