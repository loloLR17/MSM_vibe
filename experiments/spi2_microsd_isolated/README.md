# R2 — isolated SDHC SPI2 bench, TR2-002 only

Mission TR2-20261029-R2-SPI2-MICROSD-ISOLATED. NON-PRODUCTION. No FIFO acquisition,
SPI1/FRAM, SPI3 transport, SDMMC, UART/RTU or Windows service. Source profiles and
protocols of TR2-001 are unchanged. Real CubeU5v1.9.0 startup/vector/linker and
qualified observed HAL SPI, Cortex-M33/O2/160MHz, ordinary SysTick enabled.

## Wiring without soldering, confirmed by user before testing

| Adafruit4682 | STM32U575 | Female Zio connector |
|---|---|---|
| CLK | PB13 /AF5 SPI2_SCK | CN7-5 /D18 |
| SO /D0 | PD3 /AF5 SPI2_MISO | CN9-10 /D55 |
| SI /CMD | PD4 /AF5 SPI2_MOSI | CN9-8 /D54 |
| CS /D3 | PF12 GPIO | CN7-20 /D8 |
| 3V | 3.3V only | CN8-7 |
| GND | GND | CN8-11 |

This replaces the original **candidate** Morpho routing from the study. Same SPI2;
PD3 provides its alternate MISO, PF12 is arbitrary software CS, avoiding PB4's Zio
solder bridge dependencies. It is a local bench wiring choice, not normative PCB
architecture. Disconnect ALL old SDMMC signal wires/D1/DAT2. Preserve sensor wires.
The firmware sets former PB3/PB4/PB14/PB15 INPUT, PD6/PD7 ANALOG, trace off, and
sensorCS PC9 HIGH without initializing SPI3. PA13/PA14 SWD untouched. No SD command
before a RAM trigger; CS is latched HIGH before GPIO output. USB native stays off.

References reviewed: ST DS13737Rev10 table27 pp133/135 and UM2861Rev10 tables15–19,
Adafruit4682 pinouts3.3V-only. See SOURCES.json. No exact U575+4682 ST SPI example
was found in installed CubeU5. Closest ST BSP is Adafruit802/STM32CubeG4 SPI SD,
commit1d9f03564da407b0a70bfa53e54681c8dc923bc2. Its hardware/shield/HAL differ.
Our minimal SDHC-only transport follows the command sequence, **not HAL_SD**; BSP
implementation is not copied. Its unbounded ACMD41 loops/dummy CRC behavior are not
imported. Reference files remain in local evidence; original headers refer to a
component LICENSE that was not found in that directory, so not redistributed here.
The reused clock/HAL/startup keep existing ST licenses from their qualified projects.

## Gates and exact write scope

Boot WAIT2. RAM command52325301 initializes250kHz, CMD0/CMD8/ACMD41(HCS)/CMD58,
requires SDHC OCR/CSDv2, enables CMD59 CRC, reads CID/CSD with dataCRC16, then4MHz.
Double-read LBA0/2048/lastsector with exact equality and CRC32. No writes in P1.

After P1 evidence is captured and CID/capacity checked, command52325302 enables
writes **only** for test CID03534453/44333247/858BF092/3C01927A (the native R2 low
end bit may be absent: compare first15bytes and CRC7 bits, SPI end bit separately),
and62333952sectors. Explicit **sacrificial raw layout**: sector0 is replaced by
`TR2 R2 SPI2 RAW TEST V1` marker + CID; old sector0 retained from P1, partition
signature intentionally invalidated. This is not an accidental FAT overwrite or
format of a Windows volume; only the physically confirmed, wholly disposable card
is addressed. Then LBA2048..2216 receives169deterministic blocks in groups1/8/32/128
(512B/4KiB/16KiB/64KiB), each written and immediately read/compared/CRC-checked.
No erase/CMD38, formatting of any other medium or final filesystem/archive design.

Commands52325303 and52325304 generate16,000B/s equivalent interleaved int16 XYZ
bytes for10s and60s respectively, using a real32KiB ring. Fixed stream version1
and distinct seeds; actual written512B blocks from the ring, final partial paddedFF.
Backlog includes time spent in writes/busy; overflow fails explicitly. All bytes
then read from media and compared to regenerated expectations, not the transmit
buffer. Single-block CMD24/17 only, no claim of CMD25 multi-block efficiency.
Every write checks dataresponse, busy≤500ms and CMD13R2 status. No data-write retry.

After each load, command52325310 reads **the media again** into64KiB return RAM,
with contiguous `r2_return_offset`. Capture at terminal breakpoint, independently
compare all returned bytes and SHA256 on PC. Long gate requires complete short
return first; orchestration must also assert the independent comparison succeeded.
SD/CS is idle during every debugger capture; no halt during init/data/busy/timing.
An uncertain CMD24 timeout is quiesced with only ready clocks, original fault kept;
if readiness cannot be established, stage98 with no terminal breakpoint, no new CMD.
Recovery is explicit initialization only after evidence preservation, never retry.

## Evidence / limits

Raw per-write kind/writeµs/busyµs/backlog/readµs distributions, counters, command
trace (first64 and last192), readbacks and return streams retained. Compute nearest
rank P95/P99/max offline; no mean-throughput-only PASS. SPI clock is configured,
not oscilloscope-calibrated. Stack marker4KiB excludes startup and live-frame256B.
Busy polling blocks this isolated foreground: even a favorable16kB/s run does NOT
qualify future FIFO coexistence. No0-loss/1h/8h/production claim from this bench.

`R2_HOST_TESTS=ON` tests a card model, CRC vectors, exact sector roundtrip, write CID
and area refusal, readCRC/token/response/I/O faults, busy before/after accepted data,
quiescence and fail-closed behavior. Such output is not a physical SD result.
