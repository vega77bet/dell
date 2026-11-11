"""Small QMP helper for manually verifying the real installer in a disposable VM.

Use a QEMU launched with -qmp unix:/work/out/install.qmp,server=on,wait=off
and -device usb-tablet. This script does not create or partition any disk.
"""
import argparse
import json
import socket
import time

p = argparse.ArgumentParser()
p.add_argument('--socket', default='/workspace/out/install.qmp')
sub = p.add_subparsers(dest='command', required=True)
s = sub.add_parser('screenshot'); s.add_argument('path')
s = sub.add_parser('key'); s.add_argument('keys', help='QEMU qcodes separated by +, e.g. alt+f4')
s = sub.add_parser('click'); s.add_argument('x', type=int); s.add_argument('y', type=int); s.add_argument('--width', type=int, default=1024); s.add_argument('--height', type=int, default=768)
s = sub.add_parser('type'); s.add_argument('text')
args = p.parse_args()
with socket.socket(socket.AF_UNIX) as conn:
    conn.connect(args.socket)
    stream = conn.makefile('rwb', buffering=0)
    stream.readline()
    def command(name, arguments=None):
        stream.write((json.dumps({'execute': name, 'arguments': arguments or {}})+'\n').encode())
        while True:
            line = json.loads(stream.readline())
            if 'error' in line:
                raise RuntimeError(line['error'])
            if 'return' in line:
                return line['return']
    command('qmp_capabilities')
    def keys(seq):
        command('send-key', {'keys': [{'type':'qcode','data':k} for k in seq], 'hold-time': 30})
        time.sleep(.2)
    if args.command == 'screenshot':
        command('screendump', {'filename': args.path})
    elif args.command == 'key':
        keys(args.keys.split('+'))
    elif args.command == 'type':
        for c in args.text:
            if c == ' ': keys(['spc'])
            elif c == '\n': keys(['ret'])
            elif c == '-': keys(['minus'])
            elif c == '/': keys(['slash'])
            elif c == '.': keys(['dot'])
            elif c.isascii() and c.isalnum(): keys((['shift'] if c.isupper() else []) + [c.lower()])
            else: raise ValueError('Only simple ASCII typing is supported')
    elif args.command == 'click':
        command('input-send-event', {'events':[
            {'type':'abs','data':{'axis':'x','value':round(args.x*32767/args.width)}},
            {'type':'abs','data':{'axis':'y','value':round(args.y*32767/args.height)}},
            {'type':'btn','data':{'button':'left','down':True}}]})
        time.sleep(.5)
        command('input-send-event', {'events':[
            {'type':'btn','data':{'button':'left','down':False}}]})
