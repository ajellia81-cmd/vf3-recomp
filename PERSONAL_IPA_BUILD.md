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
