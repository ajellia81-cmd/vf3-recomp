# Build 5: single-round contact preview

This is a diagnostic preview, not the final clean distribution build.

## Use
Start a real fight and open Controls > Combat diagnostics with both health bars full.
Select **Round contact vibration**, confirm it says **Armed for this round**, then resume.
Vibration strength and Vibration players apply to contact feedback as well as the test.
Re-arm at the start of each round. Never arm during an attract demo or replay.

The preview is off at launch, not saved, and disarms on reset/background, timeout
(7200 emulated frames), or any writer other than the verified 16-bit contact store
touching either actual fighter-health field. This includes observed replay restoration,
refill and ring-out/setup paths. Manual scene labels do not enable feedback.
A live-vs-demo game-state gate remains unverified; explicit user arming is intentional,
not evidence that the app can detect a human match automatically.

## Event policy
Only a full 16-bit write at contact PC 0x104bc to P1 0x1087d6 or P2 0x10a7d6 counts.
Positive health loss sets continuous damage-based strength, including the decisive KO.
A verified zero-to-zero contact after KO uses a fixed medium-light profile with no
second KO tail. No sound-only, display-smoothing or health-poll-only trigger.
Unverified throw writers fail closed and may stop the preview; not advertised as full
throw support. Initial equal full bars establish the intensity reference, not a guessed
fixed maximum or a decoded energy setting.

Snapshot consumption is once per completed original emulation frame after worker
join, with a baseline before starting the next frame. Interpolated render passes never
generate contacts. Existing controller routing and asynchronous iPhone fallback remain.

## Verification
Synthetic interpreter store instructions only, no game data: default-off, both victims,
selected player masks, damage-dependent intensity, KO/post-KO, duplicate suppression,
replay/refill/unknown-writer disarm, smoothing exclusion, no HP-only fallback, invalid
stores, timeout and reset. macOS CI additionally builds arm64 device and simulator,
runs document-import tests, and packages a ROM-free unsigned IPA.
Physical iPhone gameplay remains to be tested.
