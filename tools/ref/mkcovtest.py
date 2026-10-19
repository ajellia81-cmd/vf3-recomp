#!/usr/bin/env python3
# Builds covtest.zip: a tiny PowerPC program (our own, no Sega code) that exercises the coverage hooks.
# Boot at 0xFFF00100 copies 128 words from 0xFFF01000 to RAM 0x100, then bctr there. The RAM code calls
# 0x200 (blr), flips the instruction at 0x140 (self-modifying), runs it, and loops.
import struct, zlib, zipfile, sys

def lis(d, imm): return (15 << 26) | (d << 21) | (imm & 0xffff)
def ori(a, s, imm): return (24 << 26) | (s << 21) | (a << 16) | (imm & 0xffff)
def li(d, imm): return (14 << 26) | (d << 21) | (imm & 0xffff)
def addi(d, a, imm): return (14 << 26) | (d << 21) | (a << 16) | (imm & 0xffff)
def lwz(d, off, a): return (32 << 26) | (d << 21) | (a << 16) | (off & 0xffff)
def stw(s, off, a): return (36 << 26) | (s << 21) | (a << 16) | (off & 0xffff)
def mtctr(s): return 0x7C0903A6 | (s << 21)
BCTR, BLR = 0x4E800420, 0x4E800020
def b(frm, to, lk=0): return (18 << 26) | ((to - frm) & 0x3fffffc) | lk
def bdnz(frm, to): return (16 << 26) | (16 << 21) | ((to - frm) & 0xfffc)

rom = bytearray(0x100000)  # mapped at 0xFFF00000
def put(off, words):
    for i, w in enumerate(words):
        rom[off + 4 * i:off + 4 * i + 4] = struct.pack('>I', w)

boot = [lis(3, 0xFFF0), ori(3, 3, 0x1000), li(4, 0x100), li(5, 128), mtctr(5),
        lwz(6, 0, 3), stw(6, 0, 4), addi(3, 3, 4), addi(4, 4, 4), bdnz(0xFFF00124, 0xFFF00114),
        li(4, 0x100), mtctr(4), BCTR]
put(0x100, boot)
def xori(a, s, imm): return (26 << 26) | (s << 21) | (a << 16) | (imm & 0xffff)
# 0x104-0x10C flip the instruction at 0x140 between li r8,1 and li r8,2 on every pass
pay = {0x100: b(0x100, 0x200, 1), 0x104: lwz(7, 0x140, 0), 0x108: xori(7, 7, 3),
       0x10C: stw(7, 0x140, 0), 0x110: b(0x110, 0x140), 0x140: li(8, 1), 0x144: b(0x144, 0x100), 0x200: BLR}
for a, w in pay.items():
    put(0x1000 + a - 0x100, [w])

# Usage: mkcovtest.py <out-folder> <Supermodel's Config/Games.xml>
# Writes covtest.zip and a Games.xml with the covtest game added (for runcov.sh: SM_GAMES_XML=<that file>).
out, games = sys.argv[1], sys.argv[2]
with zipfile.ZipFile(out + '/covtest.zip', 'w') as z:
    z.writestr('covtest.bin', bytes(rom))
game = '''  <game name="covtest">
    <identity><title>Coverage test</title><version>1</version><manufacturer>none</manufacturer><year>2026</year></identity>
    <hardware><platform>Sega Model 3</platform><stepping>1.0</stepping>
      <inputs><input type="common" /><input type="joystick1" /><input type="joystick2" /><input type="fighting" /></inputs>
    </hardware>
    <roms>
      <region name="crom" stride="4" chunk_size="4">
        <file offset="0" name="covtest.bin" crc32="0x%08X" />
      </region>
    </roms>
  </game>
''' % zlib.crc32(rom)
xml = open(games).read()
i = xml.rfind('</games>')
open(out + '/Games.xml', 'w').write(xml[:i] + game + xml[i:])
