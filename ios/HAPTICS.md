# Combat haptics: Revision D mapping prerequisite

The working iOS build remains build 2. Automatic combat vibration is not enabled:
no verified Revision D health fields or live-fight/replay gate exist in this repository.

The reusable damage tracker, SDL controller routing and Core Haptics phone backend
are implemented and synthetically tested in the personal VF1 and VF2 ios-port forks:
- VF1 implementation: 4d115f7d1ff4dc28bd230e18548c26e206aeb1a7
- VF2 implementation: 239320966a1562d41c334c75c537a8c3390520fc

## Repository and metadata audit

- src/machine/romset.h and romset.cpp accept Revision D, not Revision C.
- Model3's backup RAM exposes configurable starting energy, not current fighter health.
  The menu reads energy settings at backup offsets 0x1E008 and 0x1E00C.
- RAM is stored in host-order 32-bit words. Model3::read16 applies address XOR 2;
  read8 applies XOR 3. A host memory offset must not be treated as a guest address.
- ArtMoney's author-published Model 3 table is explicitly titled Revision C
  (Supermodel 0.2a x86). Its 16-bit health candidates are host offsets
  0x1087D4 and 0x10A7D4. Neither version compatibility nor byte interpretation
  has been established for this app.
- Pugsy's author-published MAME cheat 0.279 metadata for vf3 provides a timer at
  guest 0x106014. A timer alone cannot distinguish human play from attract/replays.
- Full repository searches found no supported current-health/live-fight interface.
  No ROMs, extracted program, RAM dumps or game-generated source were obtained.

Metadata sources (addresses only; no cheat patches applied):
- https://www.artmoney.ru/et_model3_1.htm
- https://www.mamecheat.co.uk/

## Required evidence before enabling the adapter

On the already working Revision D app, establish two current health values that
track the on-screen bars, the configured maximum, and an active-play flag that is
false during attract, selection, intros, replays, results and service mode.
Check fresh rounds, KO, ring-out, time-out, continue, reset and both player sides.
Record only scalar diagnostic values and scene labels; do not export game memory
or ROM/program data. Synthetic snapshots can test the adapter after these meanings
are established, but cannot establish those meanings themselves.

Once verified, connect the shared tracker after completed original emulation frames,
on the main thread, and reuse the VF1/VF2 stop/reset/controller lifecycle handling.
Do not trigger events from input presses, sound peaks, texture uploads or unverified
Revision C offsets. No interpreter/JIT, rendering or startup policy change is needed.
