# R3-DURABLE — experimental FAT32 campaign, TR2-002 only

Mission `TR2-20261029-R3-DURABLE-FAT32-10MIN`. This is a candidate test format,
not V1-05/2000Hz adoption, production, FRAM/RTU/AC1 or endurance qualification.
TR2-001, its storage, COM4 and Windows service are out of scope.

## D0 decisions and upstream

No FatFs backend was present in the current acquisition/RAW bench. CubeU5 v1.9.0
on T1000 has no populated FatFs middleware. The historical community SDMMC1 PoC
uses a separate R0.15 and its own disk driver: do not reuse that SDMMC/USB stack.
Vendored official ST FatFs **R0.16 / ST4.1.0**, commit
`dd9126e957ae841a13ccbe2115d39a1fb7414126`, includes ST security patches; hashes and
licenses in `vendor/fatfs/PROVENANCE.json`. Sources unchanged. Original ChaN license
and ST BSD3 license retained. Official source:
https://github.com/STMicroelectronics/stm32-mw-fatfs/tree/dd9126e957ae841a13ccbe2115d39a1fb7414126
Only local ffconf: no LFN/dynamic allocation/RTOS/exFAT, 512-byte sectors, create,
format, contiguous expansion, short ASCII8.3 names. FAT dates are fixed placeholders
(2025-01-01), **not measured UTC**. Archive UTC is explicitly unknown.

FAT32, MBR/partition made by f_mkfs, two FATs, 32KiB clusters. Formatting is an
explicit RAM command, exact test CID/capacity plus old R2 marker only, never boot.
This overwrites prior RAW metadata/zones as necessary on the sacrificial32Go card;
R3 positive/negative files and initial Flash/images are already preserved on GitHub.
SD transport/init/CRC remain R2; cooperative128B SdWriter remains R3 with **opt-in**
whole-test-card bounds. Without `R3_DURABLE_CARD_PROFILE`, its preprocessed code is
byte-identical to R3. CaptureQueue64KiB, singleFIFO owner64words, SPI3=8MHz,
SPI2=4MHz, real DEC8 and unchanged107tap Q15/10,250us pacing and1000/2500/5000us
references retained. Source rails, time/source faults, queue overflow and SD errors
still fail closed. No generic production disk writer is enabled.

## File lifecycle and safeguards

All FatFs operations run **with FIFO stopped**. A new file is created using
FA_CREATE_NEW (existing names refused), f_expand allocates a contiguous extent,
f_sync persists its size/allocation, every cluster is checked via f_lseek, f_close
finishes metadata, a CRC header and invalid footer are written, then capture starts.
No FatFs/file metadata operation occurs during capture. The closed file's reserved
LBA extent alone is written through the cooperative writer; out-of-extent writes
are refused before the fixed footer. STOP/drain completes all source records,
then a success footer is written only if all data was acknowledged. File size stays
preallocated; **file size is never a valid-data count or completion claim**.
A source/SD/queue failure stops FIFO; only the already-in-flight sector is finished,
uncertain writes are never retried. No success footer is generated for failed runs.

Boot WAIT; RAM magic `0x44335200`:

- `|1`: WHO7B, exact CID/capacity; mount or recognize original R2 RAW marker,
  then2s clock calibration, run1 terminal. No filesystem formatting implicitly.
- `|0x20`: only run1 + RAW marker + no filesystem: format/create `TR2INFO.TXT`.
  Confirm quiescence before physical removal. The file must actually be read
  on a standard PC reader **before** proceeding to captures.
- `|2`:2s no-writer baseline, run2.
- `|3`:2s real file `RUN0003.TR2`,128KiB preallocated.
- `|4`:60s `RUN0004.TR2`,2MiB preallocated; only after previous success footer.
- `|5`:600s `RUN0005.TR2`,12MiB preallocated; only after previous success footer.

After power cycle/reinsertion initialize with `|1` again. Fresh files and stages
prevent an implicit repeat. Before60s and600s, the previous closed file is freshly read via FatFs and its
record CRC/sequence/count/full payload SHA compared to the upstream producer,
with FIFO stopped. The final independent PC importer remains mandatory; RAM guards
do not replace PC extraction. Planned workflow: PC read of TR2INFO first; safe
reinsertion and initialization;2s→file verify→60s→file verify→600s; final removal
and full PC validation of all three files. No RAM-stage fabrication after reboot.
The physical qualification workflow is not yet claimed executed.

## Candidate format v1 (little endian)

Fixed512B header; 512B records; fixed512B footer at **last sector** of preallocated
file. Padding beyond valid records may contain old data and is ignored; record
session/sequence/CRC and footer count/hash define valid content.

Header:0 magic `TR2DUR01`;8 version1;12 header512;16 run;20 requested durationus;
24 UID3xu32;36 CID16B;52 sourceGitSHA40ASCII;92 SHA256 of107signedLEQ15 coefficients;
124 FSg8;128 sensitivity244ug/count;132 stride10;136/144 sourceHz numerator/denominator
u64 from actual calibration;152 sessionu64;160 recordcapacity78;164 UTCvalid0;
508 CRC32 of bytes0..507. DC retained, raw signed int16 counts; SI/amplitude and
clock metrology calibration not asserted. FIR causal delay53native samples, same
unity-DC107tap/10 candidate profile as R1/R3, no high-pass or RTU coupling.

Record:0 `D3BK`;4 sequenceu32;8 sessionu64;16 first exact output timeus u64;
24 last exact output timeus u64;32 first output indexu32;36 countu16 (1..78);
38 aggregate FIR flagsu16;40 CRC32 of whole512B with these4bytes zero;
44 packed XYZ3xint16×count, remainderzero. Index i maps source10*i, segment/run
from header. Flags1WARMUP/2source rail/4FIRclip/8timeunknown/16counter saturated;
warmup individually reconstructible for indices0..10, footer count also explicit.
Within-block times can be interpolated between actual endpoints; **interpolation
is approximate, not an assertion of every actual per-sample timestamp**. Dynamic
DEC8 tracking/guard100us remains active. The native waveform is not fully archived.

Footer:0 `TR2END01`;8 version1;12 complete1;16 sessionu64;24 run;28 outputcount;
32 sourceaccepted;36 recordcount;40 payloadbytes;44 observed durationus;
48 STOPreason1(controlled);52 error0;56/64 first/last output timeus u64;
72 warmupcount;76 railcount;80 clipcount;84 pendingnativeXYZ not extrapolated;
88/92 first/last source timeus u32;96 SHA256 of every output's six bytes **upstream
before record/queue/TX**;508 CRC32 bytes0..507. No complete footer on a failed run;
its cause remains unknown from the file alone and must not be invented. Firmware
observations preserve the precise fault separately. Future failure-footer/UTC
extensions require a versioned definition.

## Recovery claims, explicitly bounded

The importer scans matching-session, sequential CRC-valid records. Without a valid
complete footer, it reports INCOMPLETE and a verified prefix; a torn/corrupt tail
record without commit stops recovery at the preceding valid record. A claimed
complete file with any bad CRC/count/time/hash is CORRUPT, never accepted by merely
truncating it. Unknown versions refused. Header absent/torn prevents interpretation.
No data capture occurs during allocation/FAT updates; a preparation power loss can
leave no usable file or orphaned allocation. FAT is not a journaled filesystem.

An acknowledged record, CMD13/CTRL_SYNC and a CRC-valid prefix do **not** prove
physical survival after power disappears: controller cache/FTL/FAT integrity are
not qualified, so maximum physical data loss is **unknown**, potentially all data
if the media is damaged. Software volatile backlog is bounded to64KiB plus up to77
pending outputs in the framing buffer, plus native pending/unread sensor data;
these bounds are not a physical power-loss guarantee. No hot removal or deliberate
physical power cut while writing is part of this mission.

## Host and target validation

Host `cmake ... -DR3_HOST_TESTS=ON`, build and ctest. A sparse32Go **host file model**
(not a PC physical disk) runs the real SD writer and FatFs format/mount/expand/
sync/close/remount/read. It produces a C fixture consumed independently by Python,
with known XYZ checked separately and CRC/version/index/time/truncation/final-digest
negative cases. Six STOP writer states, uncertainCRC refusal,150/500ms and19×151054us
framed producer models check all bytes;19×500ms overflows explicitly. These are
simulations, not measurements of a card or validation of physical power loss.
Build ARM and canonical cross-build; do not rerun old scientific tests without code
change. Import on PC with `python3 tools/read_campaign.py <mounted-path>/RUN0003.TR2`.
Exit0 COMPLETE,2 INCOMPLETE,3 CORRUPT_OR_UNSUPPORTED. This tool only reads files.

D3 requires actual PC mount/read `TR2INFO.TXT`, then physical2s/60s/600s captures,
full extraction from a removed card and upstream digest comparison, timing/memory/
quiescence proofs. None is implied by host tests or successful flash. If a physical
move is necessary, only the user performs it after confirmed stopped/idle status.

The reader also exposes `samples(path)` and optional `--csv <new-PC-path>` for
XYZ counts, nominal g, output/source indices, explicit interpolated timestamps,
block endpoints and quality. CSV is created exclusively (no overwrite). Warmup is
derived from known causal phase; block-level quality is not invented per sample.
No CSV is written to the qualification card; retain the original binary.
