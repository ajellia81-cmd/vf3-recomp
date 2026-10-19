# Reference-build measurement tools

Step 1 of the brief: run VF3 on Supermodel and record which PowerPC code it runs and whether it ever writes to
code it has already run. These tools are for the reference build only; nothing here goes into the port.

`supermodel-cov.patch` patches Supermodel (made against trzy/Supermodel c65edb1). Supermodel is GPL, and so is
this patch. The port's own code must not copy from it.

## What the patch adds

- `Src/CPU/PowerPC/PPCCoverage.*` (on with `SM_COV_DIR=<folder>`): hooks in the PowerPC run loop and in the
  Model 3's RAM write paths (every CPU store and DMA goes through them). For each RAM and CROM word it counts
  how often it ran, and for RAM it keeps the word seen at the first run, the last store's PC and frame.
  It records:
  - the code that ran (`cov_ram.txt`, `cov_rom.txt`, `flags_*.bin` for merging), the hottest addresses
    (`hot.txt`) and an instruction mix (`opstats.txt`);
  - indirect control flow (`indirect.txt`): `bcctr`, `bcctrl`, `bclrl`, `blr` that doesn't return to just after
    a branch-and-link, `rfi`, exceptions and interrupt entries; and every call target (`calls.txt`);
  - who wrote the RAM code before it first ran (the "loaders": the boot copy and anything else);
  - every store onto code that had already run, whether it changed the word, and any code that then ran with
    a different word (`smc_events.txt`, summary);
  - whether the RAM code matches the CROM image, and where in CROM it came from;
  - cache and sync instructions run (`icbi`, `isync`, `dcbst`, ...): code that changes code on a real 603e
    needs them. Supermodel doesn't model the caches (`icbi` and `dcbz` do nothing), so the counts are the
    tell.
  Everything is written at exit, `summary.txt` first.
- `Src/OSD/SDL/InputScript.*` (`SM_SCRIPT=<file>`): scripted inputs per frame (press, hold, random play for
  either player, phase marks, screenshots, quit). Format at the top of `InputScript.cpp`.
- `SM_HIDDEN=1`: the window is created hidden, and on a Mac with no Dock icon.
- `SM_STATE=<file>` (with `SM_COV_DIR`): after every frame, the CPU's state and a hash of RAM, 128 words a frame,
  in the format `vf3run --state` writes; `tools/statecmp.py` compares the two. `SM_RAM_AT=<frame>` also writes
  RAM after that frame (`ram_<frame>.bin`), to find what differs.
- `io.txt` in the output: every access outside RAM and CROM, by address, size and direction, with how often, in
  which frames and the first few values. It's how the board's devices were chosen for `src/machine/model3.cpp`.

## Running

```sh
cd ~/sites/vf3-ref/Supermodel && git apply ~/sites/vf3-recomp/tools/ref/supermodel-cov.patch
make -f Makefiles/Makefile.OSX          # Makefile.UNIX on Linux
cd ~/sites/vf3-recomp
tools/ref/gen_scripts.py /tmp/vf3scripts
tools/ref/runcov.sh attract /tmp/vf3scripts/attract.txt
tools/ref/runcov.sh fighter00 /tmp/vf3scripts/fighter00.txt   # ... fighter11, versus, testmenu
tools/ref/covmerge.py ~/sites/vf3-ref/runs/*/out --map ~/sites/vf3-ref/map
```

`runcov.sh` gives each run its own data folder (`$VF3REF_RUNS/<name>`, fresh Config, NVRAM and saves) and runs
hidden: a private Xvfb display on Linux, a hidden window with no Dock icon on a Mac. It never shows a window.

The run folders hold ROM-derived data (first-seen opcodes, screenshots) and stay out of git. `covmerge.py`
prints addresses and counts only, so its output can go in a commit message or a doc.

The scripts are first drafts: how long VF3 takes to boot and how its select screen moves are guesses until
checked against each run's screenshots.

## Self-check without the game

`mkcovtest.py` builds a 1 MB PowerPC program of our own (no Sega code) registered as a "covtest" game: its
boot code copies itself to RAM, jumps there with `bctr`, calls a function and flips one of its own
instructions on every pass. The summary must show the boot copy as the loader, one `bcctr`, one call target,
and one address that ran modified.

```sh
mkdir /tmp/ct && tools/ref/mkcovtest.py /tmp/ct ~/sites/vf3-ref/Supermodel/Config/Games.xml
printf '0 mark boot\n300 quit\n' > /tmp/ct/run.txt
SM_GAMES_XML=/tmp/ct/Games.xml tools/ref/runcov.sh covtest /tmp/ct/run.txt /tmp/ct/covtest.zip
```
