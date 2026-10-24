# Personal diagnostic build 4

Device capture (9) contains 630 records: 6 candidates and 624 stores. It shows
P2 guest 0x10A7D6 stores 47->22 at frame 6773 and 22->0 at 6858, both 16-bit
writes at PC 0x104BC with manual live labels. P2 display field 0x10C408 approaches
those targets over 18 later stores at PC 0x08BB8. P1 capture (6) independently
identifies 0x1087D6 and the same normal writer; display smoothing is at 0x10A408.
The extra animation/counter values are not hits. Raw CSVs are not committed.
Publication of addresses and analysis conclusions was explicitly authorized.

Build4 adds Watch identified health to arm both discrete fields with one button,
removing repeated searches. Automatic combat haptics remain disabled: these
samples establish neither a zero-health contact writer nor a reliable
live/replay/outside gate. Manual live labels are not verification.
The preset clears the old volatile recording only when deliberately selected.
Save refuses a truly empty export before opening the output file, preserving an
earlier useful CSV. Success reports actual store/watch counts. Export schema 2
adds a comment with watch/store counts while retaining existing data columns.

New synthetic tests use real interpreter halfword stores for both observed
fields, reject smoothed display writes, check byte order and player labels,
check bounds and ensure empty exports do not truncate an earlier capture.
Apple compilation and simulator import checks must pass before delivery.
No physical device gameplay or vibration test is implied. Interpreter-only,
no JIT, no ROM/game files; rendering and lifecycle behavior are preserved.
