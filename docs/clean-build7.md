# Clean build 7

- Automatic activation from the observed full-health refill pair at PC 0x09cf0:
  no manual action required for each round.
- Contact PC 0x104bc and the two verified fighter HP fields remain the only hit trigger.
- Replay restoration, unknown HP writers and setup stop feedback; refill re-enables
  the next round. A cold initial equal/full-health contact can seed the initial round,
  but first-snapshot baseline protection may suppress its first pulse.
- Normal damage-dependent strength unchanged. Zero-health post-KO contacts use the
  strongest ordinary impact, obeying overall strength/selected players, with no repeated KO tail.
- Controls exposes only Vibration, Vibration strength, Vibration players.
- Removed diagnostic menu, manual arming, scalar search/recording and CSV export
  from the shipped application. Historical source remains available in git history.
- Package checks reject diagnostic UI/export strings and recorder symbols.
- Human-vs-attract state is NOT verified: an attract demo using the same refill/contact
  writers may vibrate. This build is automatic by observed writer sequence, not a
  verified universal scene detector. Unverified throw writers may stop feedback.
- Physical iPhone validation is still required; CI tests do not substitute for gameplay.

Synthetic tests cover repeated automatic rounds, replay stopping, refill reactivation,
both victims, gains 0/25/75/100, selected players, multiple health references, normal
damage strength and maximum post-KO contacts, no repeated KO tails, duplicate frames,
unknown/invalid writes, incomplete refill, timeout/reset, and display-field exclusion.
No ROMs, game instructions/data or raw captures are included.
