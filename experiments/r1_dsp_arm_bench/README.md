# R1-ARM — isolated NON-PRODUCTION microbenchmark, TR2-002 only

Mission `TR2-20261029-R1-ARM-DSP-FIFO-BENCH`. This project reuses the qualified ST
CubeU5 v1.9.0 U575 startup/vector table and the existing GPIO reference **clock
function only** (same vendor license in `../gpio_dat123_st_reference/vendor/`).
It never initializes GPIO, SPI, UART, FRAM or SD. Do not use TR2-001.

The real firmware checks the second MCU UID `001B0046/38425018/20333157`, configures
160MHz, DWT and ordinary unmasked SysTick, and stays at stage2/WAIT. A fresh, guarded
RAM command `0x52314131` runs the eight synthetic cases once, then stage4/DONE or
stage99/error. There is no boot autorun, storage or persistent command identity.

Pairs have 256 blocks of 1/8/60/128 source XYZ. Order alternates baseline/FIR vs
FIR/baseline between pairs, in **one identical image**, selected by RAM boolean.
Synthetic preparation is outside block timing. Baseline is unfiltered one-in-ten;
FIR uses the unchanged portable R1 Q15 component. Both share input validation,
dispatch, count/time checks and publication hashing. Difference includes the real
FIR API/history/quality overhead, not just MACs. Timestamps are synthetic fixture
endpoints, **not observed sensor time**. Raw block/emission cycles, read overhead,
input/output hashes, WARMUP/clipping/rails, interrupt counts and nearest-rank
statistics are retained. DWT read barriers/instrumentation and IRQs are included;
no automatic overhead subtraction or CPU/WCET claim from software tests.

Real table/vector ELF and BIN are built using the existing ARM toolchain and the
existing board linker. The 2KiB stack marker paints only unused RAM below a 256B
live-frame guard, checks BSS separation, and scans after timing/statistics. It is
an empirical marker from WAIT through completion/IRQs, **not startup high-water**.
An exhausted marker floor fails closed. It does not establish an industrial bound.

`R1_HOST_TESTS=ON` builds a test of pairing/counts/statistics/wrap/deadline/state
only with a fake cycle source. Such output is **never physical DWT data**. A failed
second call clears the previous completion marker (red/green test preserved).
No core FIR source or production profile is changed by this project.

## Safe physical sequence

1. Confirm bare second board, identity and qualified ST-LINK
   `0032001F3235510C37333439`. Preserve TR2-001. Read UID before programming;
   back up/reuse a verified appropriate initial image as required by the mission.
2. Hash the new build. Standard qualified programming is to `0x08000000`, scoped
   to SN002, normal necessary pages only, `-v -rst`; no mass erase/OB/RDP.
   This is **not** the previous `tests/dsp/arm` link-only ELF.
3. Wait until firmware boot has finished. A read-only WAIT check may halt only
   at stage2, with actual image readback and UID guards; never during calibration.
4. `tools/prepare_gdb.py` generates scripts only. It does not connect or program.
   Port61236 is reserved for the second-board server bound to its explicit SN.
   The run script sets a temporary **hardware** breakpoint at `r1_bench_finished`,
   sets the RAM trigger, continues without debugger commands through the window,
   and captures immutable RAM only after the terminal breakpoint. No `printf`.
5. `tools/analyze_capture.py` checks UID/clock/DONE, exact ABI/array lengths,
   raw-statistic consistency, paired input hashes, counts/quality/IRQs and derives
   cycles and instrumented CPU load at nominal source rate/configured MCU clock.
   A result requires the real guarded invocation and image provenance; bytes alone
   do not certify their physical origin. Do not relaunch a successful test blindly.

The microbenchmark is **not FIFO coexistence**. Only after it passes physically
may a separate sensor stage use one qualified FIFO owner and actual unwrapped
microsecond anchors, then short paired sessions and a longer session if healthy.
FRAM/RTU are optional only when present and safe, readonly, no B5/SD mutation.
Absent hardware or user action is reported as BLOCKED, never replaced by TR2-001.
