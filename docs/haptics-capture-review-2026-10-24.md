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

VF2 already has contact-store-based feedback and a fixed post-KO fallback. Sound analysis does not justify a general sound-to-strength mapping. Physical checks for normal hits, KO and reproducible direct post-KO contacts remain necessary; CI cannot establish physical vibration.
