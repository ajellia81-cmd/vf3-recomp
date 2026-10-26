# Smooth-motion effect audit (2026-10-27)

Scope: code inspection only for VF1/VF2; synthetic actual-code regression for VF3.
No ROM/game-data access, no physical-device or all-stage gameplay claim.

VF3: the post-interpolation flipbook pass ignored the earlier swarm/snap exclusion.
A regression invoking the actual production pass fails before the patch because a
swarm-excluded model's mesh is replaced/blended. After the patch it stays untouched,
while stable water flipbook blending remains enabled. Flip history no longer crosses
scene cuts or non-interpolated partners. This is a verified logic defect, not proof
that every reported real-game effect is fixed.

VF1: reviewed slot pairing, changed-copy runs, camera repair, cut handling and lighting.
VF2: reviewed object pairing, camera repair, replay, changed lights/focal state.
Neither has VF3's post-pass flipbook mechanism. No speculative blanket disable or
same patch has been applied to those games. Their effects are not certified flicker-free.

Remaining: physical Wolf-stage comparison with Smooth motion on/off; all-stage,
all-character/effect visual coverage for all three games needs gameplay evidence.
Held VF2 brightness and VF3 intro triangle issues are unchanged. No IPA packaged here.
