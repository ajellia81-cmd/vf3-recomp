# Scene transitions and startup

Build 2 addresses repository-side work that can delay both pictures and sound
at a character transition. It does not claim a measured fix on a physical phone.

- Release uses -O3 instead of -Os, retaining -ffp-contract=off and the PowerPC
  core's strict floating-point environment. No executable-memory compiler is added.
- Texture snapshots coalesce dirty rows before copying. Previously up to 256
  overlapping uploads could copy the same rows repeatedly; 256 requests covering
  1024 rows copied 1 GiB instead of 4 MiB. The synthetic coverage test verifies
  identical final rows and no duplicate copies, including clipped sheet edges.
- The GPU uses the same row planner; completed sound is submitted before scene
  and texture publication, so that copy cannot hold back already available audio.

PC play style deliberately executes 960 hidden game frames before the SEGA logo.
These are real interpreter frames, including sound-board state, not a fixed sleep.
They are retained for correctness. Boot now avoids tile drawing during first-run
coin initialization too, and services UIKit without the previous per-frame 1 ms
run-loop wait. Archive/board loading and hidden boot are timed separately in log.txt.
ROM CRC validation is retained. No state cache or game data is shipped.

The Apple runner's hand-authored 5,000,015-instruction loop took 24.11 ms at -Os
and 22.31 ms at -O3 in one run, with identical architectural state and timer counts.
Both builds passed floating-point rounding checks. This narrow synthetic result
is not a gameplay, boot-time or iPhone benchmark. Scene-specific CPU load, high
render scales, antialiasing and the one-time GPU/CPU validation may still cost time.
The actual opening and audio continuity must be checked with the player's archive
on their device; no ROM was used for CI.
