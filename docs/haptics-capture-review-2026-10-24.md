# Device capture review (2026-10-24)

The user explicitly authorized publishing addresses and analysis conclusions from the private diagnostic CSVs to this public personal ios-port repository. Raw CSV files are not committed.

Remote-only review of Library text extraction of haptics-hit-diagnostics(4).csv, plus empty (5).csv. The extraction exposes 1000 store rows around frames 3190-3365; this is a review of the exposed sample, not every original file byte. No ROM, program or RAM dump was obtained.

## Findings

- (5).csv contains the diagnostic comment and column header only. This establishes no recorded watches/events at export; it does not prove interpreter store interception failed.
- All exposed (4).csv events have phase unknown and player label 1. Labels are search ownership, not verified attacker/victim identity.
- 0x00119E3C and 0x0011A09C change nearly every frame through PC 0x00023E78, predominantly minus one with refills. Treat as timer/counter-like candidates, not confirmed health.
- 0x0011A47C includes frequent increments and values above the initial health-search range. Not confirmed health.
- 0x000A13EC, 0x000A3280 and 0x000A3F90 have frequent large fluctuations and paired wide/byte writes. Not confirmed health.
- 0x001087D6 has sparse writes including zero and a refill to 181. This requires on-screen correlation before promotion to Revision D health.
- 0x0010A408 appears once in this sample as a refill from 0 to 181. Insufficient evidence for current-health semantics or fighter pairing.

## Completion criteria

Establish both fighter health fields against on-screen bars, the hit/contact writer, and live-play versus replay/outside gating before enabling automatic feedback. Post-KO receiving was reported not reproducible; do not make that scenario a prerequisite. Check a direct post-KO attack only when reproducible.

Next device capture: in a fresh round start one fighter search, refine after damage only to that fighter, then keep unchanged values while the game advances without health loss. At <=8 candidates, label live and record isolated hits. Label replay/outside for controls, save before resetting, and repeat for the other side. A header-only export is insufficient.

VF2 build15 is being validated separately: contact-store events remain mandatory and matched effects estimate clipped lethal/post-KO intensity, with conservative unknown/ambiguous fallbacks. Sound-to-damage is approximate, not one-to-one. CI cannot establish physical vibration.

## New complete capture (6)

With the user's explicit permission, only the newly attached CSV was read locally;
source changes and builds remain remote. All 14,554 bytes were parsed: 317 records,
comprising 3 candidate snapshots and 314 stores. All labels are player 1 and live.
Raw CSV is not published.

- Guest 0x001087D6: three 16-bit stores, all PC 0x000104BC: frame 3191
  72->32 (40), frame 3354 32->12 (20), frame 3580 12->0 (12 remaining HP).
  This is strong candidate evidence for the discrete current-health field and
  its normal writer. The final 12 is clipped remaining HP, not full move damage.
- Guest 0x0010A408: 27 stores at PC 0x00008B6C. After each discrete decrease,
  it approaches the same target over later frames: 72->62->54...->32,
  32->27->23...->12, then 12->9->6...->0. This strongly suggests a
  display-bar smoothing field. Do not count each intermediate store as a hit.
- Guest 0x001A186A: 284 frequently varying wide stores at PC 0x00037C74,
  including wrapped/high values. No health mapping is established.
- Export frame 3611 shows both first candidates at zero. There are no zero-to-zero
  contact records in this capture, no player-2 labels and no replay/outside/post_ko
  labels. This capture cannot establish the other fighter address, post-KO writer
  or an automatic live-play gate. Manual labels alone are never a production gate.

Next useful capture: search Player 2 while attacking only that opponent, label
live for normal hits and post_ko before reproducible extra attacks after knocking
that opponent out. Save before a new search/reset. Replay/outside control samples
are still needed to assess whether the same writer is shared. Never request that
the user's knocked-out fighter receive further attacks. Do not infer the second
fighter by adding an unverified address offset.
