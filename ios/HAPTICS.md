# Combat haptics: Revision D mapping prerequisite

Personal build 3 adds opt-in scalar discovery and manual vibration tests. Automatic
combat vibration is still disabled: verified Revision D health fields, a contact
writer and a live-fight/replay gate are not established yet. Build 2 remains the
previous user-tested gameplay baseline.

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


## Build 3 diagnostic workflow

Controls > Combat diagnostics provides the same queued phone backend and patched
SDL controller initialization used by VF1 build 6 / VF2 build 12. Test vibration
plays one short pulse at the saved test strength/player choice. Automatic combat
feedback is deliberately absent; no candidate is promoted to a confirmed event.
Pause, menu entry, reset, hotplug, fast forward, background and exit stop feedback.
Foreground prepares silently; the phone and SDL engines start off the render thread.

To identify current-health fields without copying Revision C offsets:

1. Enter a fresh round. Open Controls > Combat diagnostics, choose Search target
   Player 1 (or 2), and Start health search before that player takes damage.
2. Resume. Let ONLY that player lose health, reopen the menu and choose Keep
   decreased values. Do this again after another hit if more than 8 remain.
3. Resume briefly without damage to that player, reopen and Keep unchanged values
   to discard timers and other moving state. Do not cross a round refill while
   narrowing. If no candidates remain, restart the search in a fresh round.
4. At 8 or fewer candidates, the interpreter records stores to those fields.
   Repeat for the other player to distinguish attacker/victim sides. Set Scene
   label to live before further normal hits; use post_ko before further hits
   after KO. Label replay and outside separately when practical. Labels describe
   the user's observation and are NOT an automatic scene detector.
5. Save hit diagnostics promptly. The newest 4096 scalar writes are retained.
   The file haptics-hit-diagnostics.csv is in Library/Application Support/
   Virtua Fighter 3 Recomp inside the app's data container. In LiveContainer use
   that installed app's data folder. Export the CSV only, not game files/dumps.

Searches inspect aligned 16-bit guest values from 1 to 1000. A larger integer
field whose upper half is zero can also leave a low-half candidate. Floating-point,
negative, larger or indirectly encoded health is outside this initial search.
A search keeps at most 262144 candidates and fails visibly on overflow; it never
silently keeps only the earliest addresses. Search/refinement and disk export run
only with the emulation worker joined, while paused in the menu. Nothing is saved
until Save hit diagnostics is selected. No whole-memory snapshots are exported.

The store hook covers the interpreter's DIRECT RAM writes, which bypass Model3's
bus functions. Guest big-endian byte order is reconstructed before mutation;
8/16/32/64-bit and partial stores can be observed. With no narrowed candidates it
returns immediately. At most 8 candidates per side are watched; only those scalar
before/after values, guest addresses, guest PC, frame and manual labels are saved.
Unchanged positive values are omitted; zero-to-zero stores are preserved for the
post-KO question. Reset clears candidates and records. No ROM/opcodes, program
bytes, full RAM dump, sound-amplitude or button-based hit inference is exported.

The CSV can establish whether candidate fields really track the bars and whether
the normal writer also runs after KO. Matching replay/outside samples help assess
false triggers. If the same writer is shared across scenes, an independently
verified play-state discriminator will still be required; manual labels alone
cannot supply a production gate. Synthetic tests prove byte handling and bounds,
not Revision D semantics or physical feedback.
