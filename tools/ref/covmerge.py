#!/usr/bin/env python3
# Merges coverage runs (the out folders written by the patched Supermodel) and prints a Markdown summary:
# instructions found per run and how many each run added, indirect control flow, and every store onto code
# that had already run. Addresses and counts only, so the summary can be committed; the run folders can't.
#   tools/ref/covmerge.py <out-folder>... [--map merged-folder]
import argparse, os, re, sys

ap = argparse.ArgumentParser()
ap.add_argument('runs', nargs='+')
ap.add_argument('--map', help='write the merged flags and indirect targets to this folder')
a = ap.parse_args()

WORDS = 0x800000 // 4
ram_all = bytearray(WORDS)
rom_all = bytearray(WORDS)
edges = {}          # (kind, src, dst) -> count
rows = []
smc = {}            # writer -> [stores, changed, lo, hi]
modexec = set()
loaders = {}

def count(b):
    return sum(1 for x in b if x & 1)

for run in a.runs:
    name = os.path.basename(os.path.dirname(os.path.abspath(run))) if os.path.basename(run) == 'out' else os.path.basename(run)
    ram = open(os.path.join(run, 'flags_ram.bin'), 'rb').read()
    rom = open(os.path.join(run, 'flags_rom.bin'), 'rb').read()
    new_ram = sum(1 for i in range(WORDS) if ram[i] & 1 and not ram_all[i] & 1)
    new_rom = sum(1 for i in range(WORDS) if rom[i] & 1 and not rom_all[i] & 1)
    for i in range(WORDS):
        ram_all[i] |= ram[i]
        rom_all[i] |= rom[i]
    summary = open(os.path.join(run, 'summary.txt')).read()
    frames = int(re.search(r'^frames (\d+)', summary, re.M).group(1))
    rows.append((name, frames, count(ram), count(rom), new_ram, new_rom))
    for line in open(os.path.join(run, 'indirect.txt')):
        if line.startswith('#'):
            continue
        k, s, d, c = line.split()
        edges[(k, s, d)] = edges.get((k, s, d), 0) + int(c)
    for m in re.finditer(r'writer (\w+): (\d+) stores, (\d+) changed, addresses (\w+)-(\w+)', summary):
        w = smc.setdefault(m.group(1), [0, 0, 0xFFFFFFFF, 0])
        w[0] += int(m.group(2)); w[1] += int(m.group(3))
        w[2] = min(w[2], int(m.group(4), 16)); w[3] = max(w[3], int(m.group(5), 16))
    for m in re.finditer(r'^  ([0-9A-F]{8}) ran \d+ times modified', summary, re.M):
        modexec.add(m.group(1))
    for m in re.finditer(r'^  (pc \w+|never-written) (\d+) words (\w+)-(\w+) frames (\d+)-(\d+)', summary, re.M):
        l = loaders.setdefault(m.group(1), [0, 0xFFFFFFFF, 0, 1 << 32, 0])
        l[0] = max(l[0], int(m.group(2)))
        l[1] = min(l[1], int(m.group(3), 16)); l[2] = max(l[2], int(m.group(4), 16))
        l[3] = min(l[3], int(m.group(5))); l[4] = max(l[4], int(m.group(6)))

print('| run | frames | RAM instructions | CROM instructions | new RAM | new CROM |')
print('|---|---:|---:|---:|---:|---:|')
for r in rows:
    print('| %s | %d | %d | %d | %d | %d |' % r)
print('| **all** | | **%d** | **%d** | | |' % (count(ram_all), count(rom_all)))
print()

def ranges(flags, base):
    out, i = [], 0
    while i < WORDS:
        if not flags[i] & 1:
            i += 1
            continue
        s = i
        while i < WORDS and flags[i] & 1:
            i += 1
        out.append((base + 4 * s, base + 4 * i - 4))
    return out

rr = ranges(ram_all, 0)
if rr:
    print('RAM code spans %08X-%08X in %d runs of consecutive instructions.' % (rr[0][0], rr[-1][1], len(rr)))
print()
print('Who wrote the RAM code that ran (largest count over the runs):')
for k, l in sorted(loaders.items()):
    print('- %s: %d words, %08X-%08X, written in frames %d-%d' % (k, l[0], l[1], l[2], l[3], l[4]))
print()
kinds = {}
for (k, s, d), c in edges.items():
    e = kinds.setdefault(k, [0, set(), 0])
    e[0] += 1; e[1].add(d); e[2] += c
print('| indirect flow | edges | targets | times |')
print('|---|---:|---:|---:|')
for k, e in sorted(kinds.items()):
    print('| %s | %d | %d | %d |' % (k, e[0], len(e[1]), e[2]))
print()
print('Stores onto code that had already run: %d store PCs, %d stores, %d that changed the word.' %
      (len(smc), sum(w[0] for w in smc.values()), sum(w[1] for w in smc.values())))
for k, w in sorted(smc.items()):
    print('- writer %s: %d stores, %d changed, addresses %08X-%08X' % (k, w[0], w[1], w[2], w[3]))
print('Code that ran again after being changed: %d addresses %s' % (len(modexec), ' '.join(sorted(modexec))))

if a.map:
    os.makedirs(a.map, exist_ok=True)
    open(os.path.join(a.map, 'flags_ram.bin'), 'wb').write(ram_all)
    open(os.path.join(a.map, 'flags_rom.bin'), 'wb').write(rom_all)
    with open(os.path.join(a.map, 'indirect.txt'), 'w') as f:
        for (k, s, d), c in sorted(edges.items()):
            f.write('%s %s %s %d\n' % (k, s, d, c))
