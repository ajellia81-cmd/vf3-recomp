#!/usr/bin/env python3
# Static walk of VF3 Revision D's RAM program image, read from the player's ROM, seeded with the entry points
# and indirect targets the coverage runs found. Prints counts only.
#   tools/ref/staticwalk.py <vf3.zip> <covmerge --map folder> <runs folder>
import zipfile, struct, sys, re, os
rom, mapdir, runs = sys.argv[1:4]
z = zipfile.ZipFile(rom)
names = ['epr-19230d.20', 'epr-19229d.19', 'epr-19228d.18', 'epr-19227d.17']  # offsets 0,2,4,6, stride 8, chunk 2, byte_swap
files = [z.read(n) for n in names]
size = len(files[0]) * 4
crom = bytearray(size)
for k, f in enumerate(files):
    for i in range(0, len(f), 2):
        o = (i // 2) * 8 + k * 2
        crom[o] = f[i + 1]; crom[o + 1] = f[i]          # byte_swap
crom = bytes(crom)
# Fixed CROM is 8 MB with this region at the top; the boot copies from fixed offset 0x710000 to RAM 0
base = 0x800000 - size
img_off = 0x710000 - base
IMG = 0xA0000
def word(a):
    o = img_off + a
    return struct.unpack('>I', crom[o:o + 4])[0]
def sx(v, bits): return v - (1 << bits) if v & (1 << (bits - 1)) else v
# Check the layout: the idle loop at 0x1E9FC is lbz, cmp, bc
assert [word(a) >> 26 for a in (0x1E9FC, 0x1EA00, 0x1EA04)] == [34, 31, 16], 'CROM layout or offset wrong'
cov = open(os.path.join(mapdir, 'flags_ram.bin'), 'rb').read()
seeds = {0x100} | {v for v in range(0x100, 0x1500, 0x100)}
for line in open(os.path.join(mapdir, 'indirect.txt')):
    k, s, d, c = line.split()
    d = int(d, 16)
    if k != 'rfi' and d < IMG: seeds.add(d)
for r in os.listdir(runs):
    p = os.path.join(runs, r, 'out', 'calls.txt')
    if os.path.exists(p):
        for line in open(p):
            a = int(line.split()[0], 16)
            if a < IMG: seeds.add(a)
seen = set(); work = list(seeds); bad = 0
while work:
    a = work.pop()
    while a < IMG and a not in seen:
        w = word(a); p = w >> 26
        if p == 0 or w == 0: bad += 1; break
        seen.add(a)
        if p == 18:
            t = (sx(w & 0x3fffffc, 26)) + (0 if w & 2 else a)
            work.append(t & 0xffffffff)
            if w & 1: a += 4; continue
            break
        if p == 16:
            t = sx(w & 0xfffc, 16) + (0 if w & 2 else a)
            work.append(t & 0xffffffff)
            bo = (w >> 21) & 31
            if (bo & 0x14) == 0x14 and not (w & 1): break
            a += 4; continue
        if p == 19:
            x = (w >> 1) & 0x3ff; bo = (w >> 21) & 31
            if x in (16, 528) and (bo & 0x14) == 0x14 and not (w & 1): break
            if x == 50: break
        a += 4
covset = {i * 4 for i in range(IMG // 4) if cov[i] & 1}
print('static walk: %d words reached, run coverage %d, coverage inside walk %d, walk beyond coverage %d, hit zero/invalid %d' %
      (len(seen), len(covset), len(covset & seen), len(seen - covset), bad))
print('covered but not reached by walk: %d' % len(covset - seen))
