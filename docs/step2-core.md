# Step 2: a Model 3 core and an interpreter, checked frame by frame

Status (2026-10-14): the core runs VF3 Revision D headless. It is identical to the reference build (Supermodel,
`tools/ref`) on every frame of every scripted run: all CPU registers and a hash of all 8 MB of RAM.

## Lockstep result

| run | frames | identical |
|---|---:|---|
| fighter00-11 (each fighter picked, then random play against the CPU, with continues) | 155,412 | all |
| versus (two players, both random) | 12,801 | all |
| test menu | 5,021 | all |
| **total** | **173,234** | **all** |

Each run covers boot, attract, coins and the select screen first. `vf3run` plays the same input scripts as the
reference, and `tools/statecmp.py` compares the per-frame dumps. The core runs about 80 frames a second on this
container's CPU, against the arcade's 57.5.

## What is ours, what is the reference's

All the code is ours (MIT). Supermodel was read as documentation of the board; none of its code is in the
repo. MAME's PowerPC core can't be lifted out as VF2's i960 was, because it only runs through MAME's
recompiler framework. So `src/ppc` is a new interpreter, which the recompiler can share decoding with later.

- `src/ppc`: the 603e interpreter. It matches the reference's timing (one instruction a cycle, time base and
  decrementer at a quarter of that) and checks for interrupts at the same points. Where the reference differs
  from a real 603e, the core follows the reference, and each such place is marked `Reference:`.
- `src/machine/model3.*`: the board, limited to what VF3 touches according to the reference's I/O log. Frame
  timing is the reference's.
- `src/machine/romset.*`: reads `vf3.zip` by CRC.
- `tools/vf3run.cpp`, `tools/statecmp.py`: the headless runner and the comparison.

## Found on the way

- **NaN signs depend on the reference's compiler.** The only difference found was at frame 1121 of attract: the
  sign of a NaN in a degenerate matrix that goes into a display list. The reference uses the host's FPU. On
  x86-64, an invalid operation gives −NaN (0xFFF8…), and with two NaN operands the first one in the machine
  code wins. GCC compiled the reference's `fmul` as c×a and `fadd` as b+a in round-to-nearest. Our core now
  makes every NaN result in software by those rules, so it gives the reference's answer on x86 and on ARM.
- **The reference differs between hosts.** A Supermodel built with clang for Apple silicon would use ARM's NaN
  rules (+NaN default, other operand order) and fuse multiply-adds. Its frames are not comparable bit for bit
  with this Linux x86-64 GCC build. The reference for lockstep is this build.
- **Where the reference differs from a real 603e:**
  - `fres` and `frsqrte` give exact results, not the 603e's estimates.
  - Multiply-adds are not fused.
  - Invalid operations give −NaN, where a 603e gives +NaN.
  - `mtfsf` can't clear most FPSCR bits.
  - `dcbz` does nothing.

  VF3 runs every one of these instructions except `dcbz`. Matching the hardware instead would be a deliberate
  departure, needing evidence (as VF2 did with MAME), and would end lockstep from that point.
- **The Real3D's line-of-sight registers** (0x84000014-0x84000020) come from the reference's renderer. They read
  0 in every run here. On a machine whose GPU computes them they could differ, which is worth checking before
  comparing runs made elsewhere.

## Not done yet

- The sound board (68000 and two SCSPs, from VF2): MIDI bytes are queued for it.
- The Real3D and tile generator renderers: their memories and registers are kept for them.
- The SDL host (window, menus, input) from VF1/VF2. There is nothing to show until a renderer exists.
