# R3 — FIFO SPI3 + causal FIR/10 + cooperative SD SPI2

Experimental NON-PRODUCTION, mission TR2-20261029-R3-COEX-FIFO-FIR-SPI2, TR2-002 only.
UID001B0046/38425018/20333157, explicit STLINK0032001F3235510C37333439.
Real U575 startup/vector/linker, CubeU5v1.9.0/O2/160MHz, SPI3mode3/**8MHz** and
SPI2mode0/250kHzinit→4MHz. No FRAM, UART/RTU, Windows or TR2-001 access.

## Reused qualified paths and board

One FifoLive owner,64words/quantum, existing full-burst validations/parser and exact
DEC8 microsecond mapper. Same unchanged107tap Q15/10, causal endpoints and WARMUP,
source rails still rejected by the validated parser. Optional SD hook forwards the
qualified HAL preclose observer to SPI2 only when handle differs; default platform
preprocessor output is byte-identical without the R3 opt-in.

Sensor: PC9CS/PC10SCK/PC11MISO/PC12MOSI as R1. SD Adafruit4682 wiring **from actual R2**:
PB13 CN7-5 SCK, PD3 CN9-10 MISO, PD4 CN9-8 MOSI, PF12 CN7-20 CS;3V3 CN8-7/GND CN8-11.
No PB14/PB4 candidate, no old SDMMC lines. Boot prepares hardware/CS HIGH but sends
no card/sensor command before RAM trigger. GPIOs preserve SWD, old DAT lines input,
trace/UCPDdeadbattery off. Shared HSI16 clock, separate peripheral instances/CS.

## Bounded writer

R2 `sd_spi_write` is **not called** during capture. New explicit SdWriter states:
ready+CMD24/R1 → token → four128B chunks → CRC/dataresponse + eight clocks → one-byte
busy observation per turn → separate CMD13/R2 → complete. SD CS stays low across
command/payload/busy, SCK parked between quanta while foreground services SPI3;
busy internal programming continues without SPI2 clocks. No concurrent bus owner.
Clock/CS semantics checked against SanDisk SD Product Manual§5.9/SD SPI protocol
and existing STAdafruit reference; eight post-response clocks provided. References:
https://www.brokentoaster.com/arduinomp3/files/Sandiskmanual-SecureDigital2.2.pdf
https://github.com/STMicroelectronics/STM32CubeG4/blob/1d9f03564da407b0a70bfa53e54681c8dc923bc2/Drivers/BSP/Adafruit_Shield/adafruit_802_sd.c
No external BSP/SD implementation copied; old initialization/CRC/read transport reused.

A turn drains/checks FIFO, then executes at most one bounded SD step, then250µs
post-drain interval (same baseline/with-writer loop). Service includes both; work
consumer and SD quantum each compared to1000µs, totalservice2500/gap5000µs. Fail
closed on exceeding references, malformed source, timestamp/clock residual>100µs,
queue overflow or SD error; STOP FIFO before any debugger terminal point. Source
fault does not create a successful archive header. In-flight uncertain SD write is
never retried and must quiesce before a terminal halt; no new source after STOP.

Borrowed sector remains charged to a true32KiB queue until status-acknowledged;
push refuses atomically before overwrite and counts overflow. Source is real checked
XYZ, not a nominal16kB/s simulator. Every output is packed signed int16XYZ/6bytes.
A separate producer SHA256 is updated **before queue and transmit buffers**; the
whole returned media payload must match this independent upstream digest. Exact
numeric FIR replay uses actual first1024raw inputs and first128outputs (103fully
replayable). Later source waveform is not claimed to be archived in native26kHz.

## RAM gates, isolated RAW test layout v1

52334301: WHO7B, init SDHC/CID/CSD/capacity, read original R2 sector0marker+CID,
strict exact test-card authorization, then2s actual sensor/MCU clock calibration.
No writes during calibration; no debugger interactions through this window.
52334302:2s FIFO+FIR baseline, no SD writer (same capture instrumentation).
52334303:2s real coexistence. HeaderLBA24576, payload24577onward (preserve failed4MHz zone8192).
52334304:10s coexistence. Header12288, payload12289onward.
52334305:60s coexistence. Header16384, payload16385onward.
52334310:readback64KiB windows after STOP with contiguous r3_return_offset.
Later capture gates require full return of the previous run; host orchestration
must additionally verify all bytes/hash/indices before proceeding. No autorun/retry.

Only exact CID03534453/44333247/858BF092/3C01927B/62333952sectors; writes bounded
[8192,40960), preserving R2 marker0 and all R2 data2048..3922. Existing sacrificial
card RAW state checked first, no FAT/Windows volume. Zone assignments are **test**
layout, not final V1 file design; runs do not overwrite each other. No mass erase/OB.
Header512B after STOP/drain only: magicTR2R3V1, version/run, byte/output/source counts,
stride10/origin0, source endpointtimesµs, WARMUP/rails/clips, STOPreason and pendingXYZ,
producer SHA256, CID and CRC32. Data contiguous, final partialsector paddedFF.
Actual media header read separately and compared byte-for-byte before the first
return; all payload bytes recovered and checked on PC, not just a supplied SHA.

## Statistics / reproducibility / limits

Combined BSS from actual MAP, not sum of former bench buffers. All poll cycle sums,
counts/exact maxima, full nearest-rank histograms with ceil1µs bins; overflow clamps
only after violating the5ms guard, which fails. First256/last256 paired records
for worst-case composition, not falsely complete rawpoll list. SD all successful
write/busy rawpairs up to2048, then bounded first/last trace plus full ms histogram;
trace capacity never becomes a sensor fault. First2048sensorwords/1024timedinputs/
128FIRoutputs and source SHA preserved. Stack4KiB marker excludes startup/256B live
guard. Ordinary SysTick included, no claim about UARTIRQ/FRAM/coexistence product.

R3_HOST_TESTS: real parser/DEC8/FIR with modeled producer and card, injected150ms/
500msbusy, exact31998bytes, sourcefault→invalidation, real queue full/borrow/refusal,
CID/region/SD faults/timeout, incremental SHA known vector +one-shot agreement.
Simulation is not DWT or a card observation. Production/shared core unchanged;
new ARM +canonical and targeted tests required. Only after physical C3 success,
60sC4; optional10min not required and not silently attempted from fixed trace RAM.

## Measured4MHz failure and bounded correction

First imageebd433e3, bothbuses4MHz, FIFO64words and250µs pacing: baseline2s healthy,
then coupled2s stopped on actualFIFOoverrun after1.067492s. Per-turn maxservice2367µs,
work838µs,gap2624µs stayed below references, but sustained drainage did not keep up.
Negative image/captures/zone8193..8224 preserved. SPI3now8MHz, within IIS3DWB10MHz
maximum (ST DS12569Rev8table5), sameSPI2/FIR/quantum/limits. This requires fresh
baseline and short coexistence before longer tests; not a revalidation of old R1.
https://www.st.com/resource/en/datasheet/iis3dwb.pdf
Cleanup now commits only the already-in-flight acknowledged sector to diagnostics,
without pulling new queue blocks on source failure, and freezes producer digest
on fault. Old firstfailure had31counted/32hardwareacknowledged, no valid successheader.
