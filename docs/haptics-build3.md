# Personal diagnostic iOS build 3

Source: `533deadeb3e93d7de8ddc676a094b0735dd13215`, personal ios-port fork.
Run https://github.com/ajellia81-cmd/vf3-recomp/actions/runs/39989991071
(job 120538975913, macOS 15) succeeded. The previous macOS 26 attempt
expired without an assigned runner; switching the runner resolved that delay.

This build adds manual vibration testing and opt-in scalar health-field discovery
and store diagnostics. Automatic combat haptics remain disabled pending verified
Revision D health/contact/scene mapping. See ios/HAPTICS.md for capture steps.
Manual scene labels are observations, not an automatic live-fight/replay gate.

Apple checks passed: interpreter/no-executable-memory enforcement, strict FP
runtime, texture snapshot deduplication, controls, autosave, native Metal/display,
scalar discovery and actual interpreter 8/16/32/64-bit store tests (ASan/UBSan),
shared damage tracker, queued controller warmup, source/JIT exclusion and touch
Menu checks. Device Release compilation and both simulator document import
XCTests passed. No ROM/gameplay or physical haptic test was performed.

File: VirtuaFighter3-build3-unsigned.ipa, 7,853,040 bytes.
SHA-256: e6dfee2a75afc5b012eb2ae903342364a80718a2b3a19af9d8cea71677091af1.
Actions artifact 12518567873 wrapper SHA-256:
8917399e9603955a4e34f3039f99a5552ff73ee849b19b799bee48174f0b2356.
Both ZIPs were checked. Inner hash matched the runner log. Bundle version/display
name, source, interpreter-only/no-ROM metadata, disabled automatic combat haptics,
and diagnostic menu/native haptic markers were inspected. No game archives or
ROM files were present. Personal unsigned use only, no public release.
