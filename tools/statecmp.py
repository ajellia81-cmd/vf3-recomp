#!/usr/bin/env python3
# Compares two per-frame state files (the reference build's SM_STATE and vf3run's --state): 128 little-endian
# words a frame, the CPU's registers and a hash of RAM. Prints the first frame that differs and what differs.
#   tools/statecmp.py <reference.state> <ours.state> [--all]
import struct, sys

NAMES = {i: 'r%d' % i for i in range(32)}
for i in range(32):
    NAMES[32 + 2 * i] = 'f%d.hi' % i
    NAMES[33 + 2 * i] = 'f%d.lo' % i
NAMES.update({96: 'cr', 97: 'xer', 98: 'lr', 99: 'ctr', 100: 'msr', 101: 'npc', 102: 'srr0', 103: 'srr1', 104: 'dec',
              105: 'tbl', 106: 'tbu', 107: 'fpscr', 108: 'sprg0', 109: 'sprg1', 110: 'sprg2', 111: 'sprg3',
              112: 'hid0', 113: 'hid1', 114: 'ram.hi', 115: 'ram.lo', 116: 'frame', 117: 'irq_pending', 118: 'timer_frac'})

def frames(path):
    d = open(path, 'rb').read()
    return [struct.unpack('<128I', d[i:i + 512]) for i in range(0, len(d) - 511, 512)]

args = [a for a in sys.argv[1:] if not a.startswith('--')]
show_all = '--all' in sys.argv
ref, ours = frames(args[0]), frames(args[1])
n = min(len(ref), len(ours))
bad = 0
for f in range(n):
    diffs = [i for i in range(128) if ref[f][i] != ours[f][i]]
    if diffs:
        bad += 1
        if bad == 1 or show_all:
            print('frame %d differs:' % (f + 1))
            for i in diffs:
                print('  %-12s reference %08X  ours %08X' % (NAMES.get(i, 'w%d' % i), ref[f][i], ours[f][i]))
        if not show_all:
            break
if bad == 0:
    print('%d frames identical (reference %d frames, ours %d)' % (n, len(ref), len(ours)))
    sys.exit(0)
sys.exit(1)
