# Step 1: what VF3 runs, and does it change its own code?

Measured 2026-10-14 on Supermodel (trzy/Supermodel c65edb1 plus `tools/ref/supermodel-cov.patch`), Revision D,
hidden runs under Xvfb on Linux, each in its own data folder. 15 runs, 236,000 frames in all (about 66 minutes of
game time).

## Answer

**VF3 never changes its code. The whole program can be recompiled ahead of time from the ROM, as VF2's was.**
Keep the interpreter as a safety net (VF1 and VF2 both did), but nothing in these runs needs it.

- **One loader.** Every RAM instruction that ran was written by a single store, at 0xFFF001B4 in frame 58: the boot
  copy from CROM to RAM. No code is copied, unpacked or patched after that.
- **The RAM program is the ROM image.** Every executed RAM word, in all 15 runs, equals the fixed CROM at offset
  0x710000 + its address, so the boot copies from 0xFFF10000 to RAM 0 (0xA0000 bytes, per the brief). The
  code that ran sits in 0x100-0x6E2AC.
- **No stores onto code.** Zero stores, from any PC or DMA, landed on a word that had already run. No code ran
  twice with different contents.
- **No cache maintenance.** `icbi`, `dcbst`, `dcbf`, `dcbi` and `dcbz` never ran, nor `sc` or traps. A 603e,
  with its separate instruction cache, couldn't see changed code without them. `isync` and `sync` do run (26
  places, mostly in the low-level code at 0x100-0x20BC), but only for ordering: without the cache-block
  instructions they can't make changed code visible.
- **Statically discoverable.** PowerPC instructions are a fixed 4 bytes. Walking the ROM image from the entry
  points and the indirect targets the runs found (`tools/ref/staticwalk.py`) reaches 78,177 instructions. That's
  all but 56 of the 65,664 that ran, plus 12,569 that no run reached (other moves, the ending, error paths). The
  56 are in 9 places that are only entered by returning from an interrupt (`rfi`) or through `blr` used as a jump.
  The map has them.

## Coverage

| run | frames | RAM instructions | CROM instructions | new RAM | new CROM |
|---|---:|---:|---:|---:|---:|
| attract | 37201 | 47960 | 50 | 47960 | 50 |
| fighter00 Akira | 12771 | 50993 | 50 | 11098 | 0 |
| fighter01 Pai | 12831 | 52291 | 50 | 1294 | 0 |
| fighter02 Lau | 12891 | 51470 | 50 | 154 | 0 |
| fighter03 Wolf | 12951 | 52137 | 50 | 734 | 0 |
| fighter04 Jeffry | 13011 | 52606 | 50 | 653 | 0 |
| fighter05 Kage | 13071 | 52621 | 50 | 113 | 0 |
| fighter06 Sarah | 12831 | 53069 | 50 | 61 | 0 |
| fighter07 Jacky | 12891 | 52924 | 50 | 143 | 0 |
| fighter08 Shun | 12951 | 52683 | 50 | 191 | 0 |
| fighter09 Lion | 13011 | 51987 | 50 | 24 | 0 |
| fighter10 Aoi | 13071 | 52210 | 50 | 122 | 0 |
| fighter11 Taka-Arashi | 13131 | 51264 | 50 | 74 | 0 |
| versus | 12801 | 52788 | 50 | 2036 | 0 |
| testmenu | 5021 | 32160 | 50 | 1007 | 0 |
| **all** | | **65664** | **50** | | |

The screenshots confirm each fighter run picked its fighter. The runs went through rounds, K.O.s, Continue,
stage 2 and "Next match". In the fighter runs, player 1 played randomly against the CPU and was given a fresh
credit every two minutes. The later fighter runs added under 200 instructions each. The versus and test menu runs
added 2,036 and 1,007, so other modes still have unseen code, which the static walk covers. The 50 CROM
instructions are the boot code at 0xFFF00100-0xFFF001C4.

## Indirect flow (the map's job)

| kind | edges | targets | times |
|---|---:|---:|---:|
| bcctr (jump tables) | 251 | 249 | 3,219,467 |
| bcctrl (calls through pointers) | 235 | 209 | 27,157,542 |
| bclrl (calls through LR) | 152 | 149 | 10,857,082 |
| blr that isn't a return | 27 | 26 | 276,357 |
| rfi | 8222 | 7968 | 8,374,693 |
| interrupt entry | 8214 | 2 | 630,250 |
| exception after an op | 2 | 1 | 430,909 |

- Interrupts enter at 0x500 (external) and 0x900 (decrementer) only. They return with `rfi` to nearly 8,000
  different places, so native code must be able to stop and resume at any instruction boundary. VF1 and VF2
  already did this, ending slices where the interpreter would and taking interrupts there.
- Two `mtmsr` instructions (0x61CD0, 0x61D50) re-enable interrupts and take a pending external interrupt
  straight away. Native code has to check for interrupts right after `mtmsr`, not only at block ends.
- About 600 jump-table and pointer targets: a map, as with VF1's `vf_map.txt` and VF2's `vf2_map.cpp`
  (addresses only), plus the interpreter for anything a player reaches that the map lacks.

## Worth knowing for the recompiler

- **Idle loop.** In the calibration run, about 40% of everything executed is a 3-instruction poll at
  0x1E9FC-0x1EA04 (`lbz`, `cmpw`, `beq`), waiting for a byte the interrupt handler sets. VF2 skipped its idle
  rounds the same way.
- **Floating point.** 146 different instructions ran. Single-precision float work is heavy (`lfs`, `fmadds`,
  `fmuls`, `fsubs`, `stfs`): `fmadds` alone ran 64 million times in the first 3,600 frames. `fmadds` rounds
  once on hardware. Matching what the reference does (separate or fused) will need checking against lockstep,
  much as the GPU work did with fused multiply-adds.
- **Supermodel's gaps.** `icbi` and `dcbz` do nothing in Supermodel. VF3 never runs them, so that doesn't
  matter here.

## How it was measured

`tools/ref` (see its README). The patch hooks the PowerPC run loop and every Model 3 RAM write path, CPU stores
and DMA alike. It records the code that ran, who wrote each RAM word before it first ran, every store onto code
that had already run, the indirect flow, and cache instructions. It also compares the RAM code with the CROM
image. Scripted inputs drive the runs, which are hidden and each have a fresh data folder. A synthetic PowerPC
program of our own (`mkcovtest.py`) checks that the hooks catch a loader, an indirect jump and self-modifying
code.

Commands:

```sh
tools/ref/gen_scripts.py scripts
ls scripts/*.txt | xargs -P 4 -I{} sh -c 'tools/ref/runcov.sh $(basename {} .txt) {}'
tools/ref/covmerge.py $VF3REF_RUNS/*/out --map map
tools/ref/staticwalk.py vf3.zip map $VF3REF_RUNS
```

Under Xvfb, Mesa's software GL leaves most 3D surfaces white. The game logic doesn't depend on it, and the
screenshots are still readable enough to follow the game. Runs went at about 13 frames a second each, 4 at a
time.

## Not covered yet

- Dural and the ending (no run won the ladder), name entry, and Revisions A and C and Team Battle. The static
  walk covers code the runs didn't reach. A ladder win script, and runs on the other revisions when they're
  targeted, would confirm it.
