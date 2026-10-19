# Step 3, first part: the PowerPC recompiler

Status (2026-10-14): VF3's program is recompiled to native code (x86-64 or ARM64, with SLJIT) when the game
starts, from the player's ROM. It runs identical to the interpreter, and so to the reference build, on every frame
of every scripted run. The interpreter stays as the fallback and as the reference for checking.

## How it works (`src/rec`)

- **Finding the code.** The boot copies 0xA0000 bytes from the fixed CROM (offset 0x710000) to RAM 0, and VF3
  never changes that code (step 1). At start-up the recompiler walks that image from the address map,
  `src/rec/vf3_map.cpp`: 1,487 addresses (call targets, indirect jumps' targets, interrupt entries, and the
  places reached only by returning from an interrupt). The map is made by `tools/recomp/mkmap.py` from the step 1
  coverage, and contains addresses only. The walk finds 78,252 instructions, every one of the 65,664 seen running
  and about 12,600 more.
- **Native code.** All of it is one piece of SLJIT code with a label at every instruction, so it can be entered
  wherever an interrupt returns. The flow between instructions is native jumps; indirect branches go through a
  dispatcher. Made in about 160 ms.
- **The interpreter's order, exactly.**
  - Every instruction takes its cycle off the count and checks the end of the slice and the decrementer's trigger
    where the interpreter's loop does.
  - Anything that can take an interrupt or stop the CPU (device accesses, `mtmsr`, `rfi`, `sc`, traps, timer
    registers) runs the interpreter's own handler and then goes through the dispatcher.
  - Native: the integer instructions, loads and stores with a RAM fast path, branches, CR operations, FP loads,
    stores and moves, and FP arithmetic in round-to-nearest. FP results that would be NaNs go to the interpreter's
    handler, which has the reference's NaN rules.
  - Everything else (`fctiw`, `frsqrte`, `mftb`, `mtcrf`, ...) calls the interpreter's handler. Where the handler
    can't change the flow, the call costs no dispatch.
- **The idle loop.** The game waits for the vblank interrupt in 3-instruction loops that poll a byte. Within a
  slice nothing but the CPU changes RAM, and interrupts come only at the slice's start or the decrementer's trigger.
  So once a whole round has run, the rounds that can't reach the slice's end or the trigger are skipped, their
  cycles counted. If an interrupt arrived in the middle of a round, the loop is never skipped (the bug the lockstep
  checker found: a round started before the interrupt changed the byte).
- **Safety.** The native code runs only while every instruction it was made from is in RAM unchanged. Stores to
  compiled words are tracked, the CPU's and the 53C810's alike. Anything without code runs on the interpreter:
  before the boot copy, the boot code in CROM, and any address the map lacks (none in these runs).

## Checks

- `tools/vf3lock.cpp`: two boards, interpreter and recompiled code, compared after every frame (CPU state, RAM,
  backup RAM, the Real3D's and tile generator's memories, MIDI out). At the first difference it replays that frame
  and compares after every slice.
- Against the reference: `vf3run --rec --state` and `tools/statecmp.py` over the 14 scripted runs.

## Speed (this container, x86-64, 3,601 frames: boot, attract, select, a fight)

| | frames a second | vs. real time (57.5) |
|---|---:|---:|
| interpreter | 89 | 1.5x |
| recompiled, every instruction through the interpreter's handlers | 52 | 0.9x |
| recompiled, native integer, memory and branches | 233 | 4.1x |
| + idle loop skipped | 322 | 5.6x |
| + native FP arithmetic | 456-478 | 8x |

## Next

- The rest of the frame's time is now the board, not the CPU: profile it once a renderer is in.
- An ARM64 run of `vf3lock` (Apple silicon) to check the native code there too.
- The sound board, then the renderer (with reference frames from the Mac).
