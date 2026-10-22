# Personal iOS port
- Work only on ajellia81-cmd/vf3-recomp ios-port. mr-finster/vf3-recomp is read-only upstream.
- Small reviewable commits, no force push, no ROM downloads, data or derived program inclusion.
- VF3_IOS and VF3_DISABLE_JIT required; exclude ppc_rec.cpp and SLJIT, use ppc_rec_stub.cpp.
- User authorized personal unsigned IPA with the supplied icon. No public release or signing secrets.
- ios/project.yml is authoritative; never commit generated projects, dependencies or build products.
- Use Korean progress messages. Distinguish CI compilation/tests from physical iPhone gameplay/performance.
