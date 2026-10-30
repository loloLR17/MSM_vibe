# R3-1H — bounded endurance profile (NON-PRODUCTION)

Mission TR2-20261029-R3-1H-FAT32-ENDURANCE. This note describes software,
not a claim of a completed physical hour. See the matching mission report.

Configure this same project with `-DR3_ENDURANCE_1H=ON`. Boot WAIT remains.
RAM commands: 1 identifies WHO/CID, mounts FAT32 and calibrates; 3 produces a
new 2-second H1N0003.TR2 (128KiB); 5 first reads/verifies that closed short
file against the producer SHA, then produces H1N0005.TR2 for 3600 seconds.
Commands 2 (old bypass), 4 (old60s), and 0x20 (format) are refused in this
profile. No old2s/60s/600s campaign is restarted. The short gate is justified
by changed clock ABI, file version and allocation check. An existing H1N name
is refused, never overwritten. All six RUN/R2N historical captures survive.
No automatic retries, no FAT calls while FIFO runs, unchanged64KiB queue,
SPI3 8MHz/SPI2 4MHz, FIFO64words/SD128B/groups1..8 and guards1000/2500/5000us.

## E0: ranges, allocation and timing

At nominal conservative8000/3 outputHz: 9,600,000 XYZ/h,57,600,000 payloadB,
ceil(9,600,000/78)=123077 records,63,016,448 framedB including header/footer.
72MiB=75,497,472B gives12,481,024B (19.81%) overhead reserve beyond that
calculation; 147456 sectors,2304 FAT clusters at32KiB. It holds up to
11,501,412 XYZ, so it covers3194.84 outputHz for3600s. Actual prior rate
was2642.868094Hz between endpoints, NOT exactly2666.7 or normative2000Hz.
Space is checked with f_getfree before FA_CREATE_NEW; f_expand and every
cluster check enforce contiguous allocation, sync and close before FIFO.
The independent host fixture now really allocates and parses72MiB.

Duration3600000000us fits uint32 with694967295us margin. Absolute boot
microseconds need not fit: old sink rejected us>UINT32_MAX and Bracket cast
both timestamps to32bits. Both now retain64bits; monotonic/index checks
remain. Diagnostic Result version3 has64-bit source first/last fields;
Bracket is24bytes instead of12 (AAPCS padding). Poll/group/busy histograms
have exact full counts/sum/max, bounded raw first/last traces; no hour trace
array expansion. Native samples~96M, outputs~9.6M, sector~123k, groups~15.4k,
polls<15M, SysTick3.6M, all under u32; cycle sums5.76e11 fitu64.
The sensor raw32 DEC8 counter period~29.8h, longer than this run. Its mapper
already extends modular deltas. DWT cycles wrap every26.8435456s; r3_now
accumulates modular32-bit deltas into64bits. It is sampled each poll/idle
1ms SysTick wake; no debugger halt during capture/calibration/allocation.
An actual shared accumulator test simulates5.4M ticks (5400s), crossing
multiple DWT and absolute-u32-us boundaries with exact expected64-bit time.
A debug halt is not elapsed wall-clock evidence and may discard whole DWT
wraps: all reported capture time is measured during uninterrupted execution.

File header/footer version2 (same magic) reserves footer128/136 for complete
u64 source first/last microseconds. Old88/92 contain explicit low32 words;
reader cross-checks them. Record layout/CRC and producer SHA are unchanged.
PC reader accepts v1 and v2, refuses unknown/mismatched versions. V2 also
requires the requested duration actually completed within the unchanged
5ms service bound. Timestamps are sensor event endpoints tied to nominal
MCU DWT, not UTC/SI. Within-record interpolation remains approximate.

## Before8h (not authorized here)

28,800,000,000us exceedsu32. Widen session duration, Result.duration_us,
header20/footer44 representation (new explicitly versioned format), host
readers and all command gates. Absolute sources/brackets are alreadyu64.
At the same candidate rate: ~456.7MB useful / ~499.7MB framed; conservative
nominal460.8MB useful/~504.2MB framed. Reserve an explicit margin, check
FAT32 contiguous capacity/free space before capture; no single file4GiB
limit yet at8h. Native~768M, outputs~76.8M and framed bytes<1GiB fitu32,
but verify every multiplication and histogram counter under actual rate,
future24h and maximum allowed boot duration; widen/limit before overflow.
Keep DWT sampling gap<one wrap. Handle sensor raw counter wrap for long
boot+run through existing modular mapper. Audit all scripts/GDB timeouts,
CPU/stack and extraction/copy time. More rare SD wait clusters can still
fill64KiB; neither a good mean rate nor one good hour proves arbitrary
pauses, power-loss tolerance, industrial8h, metrology, FRAM or RTU.
