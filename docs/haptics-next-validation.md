# VF3 haptics validation

Automatic combat feedback stays disabled until Revision D fighter-health fields and a reliable live-fight gate are verified.

## Regression checklist

- Narrow both fighter health searches independently, checking eight watches per side.
- Record normal hits, KO, post-KO contacts, and scene changes.
- Verify that guest memory writes are logged without altering emulated state.
- Check that manual scene labels never enable production vibration.
- Preserve existing interpreter-only iOS builds and ROM-free artifacts.

No memory address or sound ID is assumed to represent a verified hit.
