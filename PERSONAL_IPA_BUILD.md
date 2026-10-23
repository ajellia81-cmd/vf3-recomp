# Virtua Fighter 3 personal unsigned IPA — build 1

- Personal fork: ajellia81-cmd/vf3-recomp, branch ios-port; mr-finster upstream unchanged.
- Source: 38a3d5ea23852da0f8ea09fe04a69ab85d3fe99a.
- Successful Apple build: https://github.com/ajellia81-cmd/vf3-recomp/actions/runs/39815154472
- Output: VirtuaFighter3-build1-unsigned.ipa (7,681,762 bytes).
- SHA-256: 1c1778fb33f47b366ab7e1eac5eafe58863ad60e435d4280e23d6d911f6cd16d.
- Bundle: com.ajellia81.vf3recomp; display name Virtua Fighter 3; build 1.
- arm64 iPhoneOS Release; minimum iOS 18.0; unsigned for personal re-signing.

The supplied artwork is compiled into AppIcon. Native touch controls, including
Escape, appear only without a connected controller. Pending Menu taps survive
repeated menu updates; gameplay buttons are released when entering the menu.
Original 1x–6x raster choices remain in order, followed by 2868 x 1320 (iPhone).
All resolutions follow Picture shape. The iPhone option is a fixed composition
output from the original 4x raster, not a fractional native 3D raster.
Brightness supports 50–200%; the original SDL_GPU/Metal Real3D renderer remains.

The Apple runner passed interpreter/JIT boundary and invalid-ROM rejection checks,
control settings persistence, autosave drain/resume, synthetic Metal geometry
comparison against the CPU renderer, composition/aspect/brightness checks,
the actual touch Menu transition helper, Release iPhone and Debug simulator
compilation, and two asynchronous import tests (cancellation and invalid file).
The downloaded artifact and IPA ZIP integrity, build/source metadata, icon,
ROM exclusion and SHA-256 were verified.

CI does not establish gameplay compatibility, physical iPhone performance,
120 fps, or LiveContainer behavior. The user must import their own complete
vf3 Revision D ZIP on-device. No ROM, extracted game data, generated game program,
JIT or signing secret was downloaded or included. No public release was published.

## Build 2: character transitions and startup overhead

- Source: 4995d6fffe1b5d5bc5ffab263d7aff0101752b8d.
- Successful Apple build: https://github.com/ajellia81-cmd/vf3-recomp/actions/runs/39899764245
- Output: VirtuaFighter3-build2-unsigned.ipa (7,832,658 bytes).
- SHA-256: fb356db29ad4131133344e1580660d48f35f39199f29babaeadfdf5e2770f112.

Release prioritizes speed while preserving strict floating-point behavior.
Character texture snapshots coalesce repeated dirty rows, and completed audio is
submitted before scene publication. Hidden boot avoids unnecessary tile drawing
and per-frame UIKit waits; archive loading and boot times are logged separately.
The original 960-frame boot and ROM CRC validation remain. See ios/PERFORMANCE.md
for the measured synthetic result and the limits of repository-side verification.

Apple interpreter state/rounding checks passed at both -Os and -O3, as did the
sanitized texture coverage test, synthetic Metal/CPU geometry and composition
checks, controls/autosave/Menu tests, device/simulator builds and two import tests.
Downloaded archive/IPA integrity, build/source metadata and SHA-256 were verified.
Actual opening-transition smoothness, audio continuity and launch time on the
player's phone have not been measured. Personal unsigned use only, interpreter-only,
no ROM inclusion; upstream unchanged.
