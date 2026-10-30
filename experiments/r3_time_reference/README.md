# R3-1H — time qualification, same mission, TR2-002 only

Status: offline audit complete; short idle probe SOFTWARE PREPARED. A first idle physical calibration is now preserved in the report:160.962..161.174MHz relative to QPC, current setup only. The independent hour gate remains unqualified. Existing E4 integrity/150coretests/science/buses are not replayed.
No change to MISSION_EN_COURS, production, AC1/Modbus/FRAM or persistent storage.

## Existing evidence cannot retrospectively qualify the hour

`tools/audit_previous_hour.py EVIDENCE_DIRECTORY` reads the original logs without
modifying them. Decimal calculations: UTC outer interval3584.726090s,
nominal MCU3600.000743s, difference15.274653s. The three recorded Windows steps
sum to-1.913166300s; correcting only these yields3586.639256300s, still
13.361486700s short, even before subtracting connection/setup/drain/dumps.
That conditional model assumes WSL follows precisely those steps and no other
slew/virtualization effects: it is NOT a proven physical bound. In that model
only, the minimum core ratio would exceed1.00372535 (~160.596MHz). This is
not a measurement of oscillator error and does not attribute the whole gap.
Original Windows console bytes are preserved/hashed. Only ASCII dates are
parsed from messages; non-ASCII locale text is replaced and never interpreted.

Order actually implemented: host datetime before GDB server launch/sleep2s,
connect/guards/RAM trigger, prior short-file verification and FAT preallocation,
FIFOstart, begin_us, polling until nominal duration, duration_us read BEFORE
FIFOstop, STOP, frame flush/drain/footer, terminal breakpoint, GDBprints/dumps,
detach/server exit, host datetime. Neither datetime brackets the exact source
window. No recorded QueryPerformanceCounter/Stopwatch/monotonic start-stop pair
was found in these sources or logs. Timeout internals and sleeps do not create
such a published pair. GDB/server logs have no independent capture-event clock.
DWT and SysTick use the same MSI/default trim -> PLL nominal160MHz; HAL frequency
reads configuration, not an SI measurement. Source DEC8 calibration is itself
against that MCU and is not an external time reference. Integrity remains valid.

## Short, independent request/response calibration

Only `src/main.c` plus the existing ST GPIO clock function, HAL/startup and
small existing CRC/clock helpers. No FIR/SD/FIFO/FRAM/RTU sources linked, no
sensor GPIO or SD initialization and no NVM writes by the application. UID guard
002 before GPIO. USART1 PA9/PA10/AF7,1152008N1, no IRQ and no periodic TX.
On a singleASCII `Q`, return32bytes: QPC1, UID3u32, sequenceu32,
accumulatedDWTcyclesu64, CRC32(first28), allLE. BootWAITstage2; no automatic test.
Any UART error/unknown command/timeout latches terminal99. DWT extendedu64 sampled
on1ms SysTick wakes, including transmit waits. No debugger during measurement.
This idle probe has different peripheral load from capture, and cannot calibrate
an earlier run retroactively or guarantee future temperature/drift behaviour.

On Windows `measure_reference.ps1` defaults to HOST_QPC_ONLY_NO_PORT_OPEN. It
uses built-in Stopwatch, no dynamic compilation, policy change or security
bypass. -Calibrate requires an explicit COM other thanCOM4 and a PnP ancestor
matching TR2-002 STLINK0032001F3235510C37333439. Port exclusive, no forced handle
release/reconnect or discarded input. Two queries separated by10s; complete
frames and send/receiveQPC ticks saved. Python independently checks CRC/UID,
consecutive sequence, increasing cycles and nonoverlapping monotone brackets.

For each query the MCU snapshot lies between PCrequest-send and response-receive.
Thus delta_ref is in [sendB-receiveA, receiveB-sendA]/QPCfrequency. Divide the
cycle delta by those endpoints to bound actual cycles per QPCsecond. Transport
and scheduling delays widen the interval; no arbitrary latency bound is assumed.
Windows QPC is independent of the STM32 and system UTC adjustments. Its100ns
resolution on this host is not SI traceability or an absolute drift certificate.

## Next hour methodology (QPC2 implementation prepared, not yet physically qualified)

Use a versioned diagnostic snapshot on the same qualified VCP, nonblocking in
the FIFO polling budget, with UID, session, sequence, active flag, emitted source
count, MCUtime and CRC. SnapshotA must be generated only after first accepted
XYZ while acquisition is active. Record QPCreceiveA AFTER the full response.
After at least3600*QPCfrequency+2ticks on the same host, record QPCsendB BEFORE
sending queryB. SnapshotB must also report active, same session and increasing
source count. Consequently START < snapshotA <= receiveA < sendB <= snapshotB
< STOP: the independently measured INNER interval is contained in uninterrupted
capture. A late USB/OS delivery cannot lengthen this claimed lower bound.
Both samples plus a successful continuous file/quality/footer are required.

The opt-in R3_QPC_WITNESS=ON capture guard of4000nominal MCU seconds with72MiB keeps the
existingu32 duration and unchanged filev2 semantics; the active-snapshot test
fails if actual capture ends too soon, regardless of the MCU oscillator. At
conservative2666.7outputs/s,4000s produce64MB payload/~70.02MB framed, fitting
72MiB; check actual free/extent and rates before any execution. New Q1N names,
no reuse of H1N. This longer guard is proposed only to prove at least3600QPC
seconds, not to relabel4000nominal as exactly3600physical. Keep all pipeline and
time guards unchanged; qualify snapshot overhead with one justified shortgate.
No new hour before reference path and short gate actually work.

This design makes PC observation optional for firmware progression: MCU still
stops and drains on its own if the PC disappears. Future8h requires64bit duration,
explicit file format changes and larger extent; no Codex/GDB/PC dependency for
acquisition. QPC witness requires a PC for measurement only and is not an8h
control dependency. Independent review remains necessary, no8h launched here.

## References and physical preconditions

ST UM2861 Rev10 §7.9/table10 and table18 note1: default STLINK VCP is USART1
PA9/PA10 (SB32/SB34), whereas PG7/PG8 LPUART1 defaults to D0/D1. Actual bridge
routing is not inferred from the board model; if no correct frames, stop and
inspect, never change solder bridges automatically. Same board manual identifies
an onboard32.768kHz crystal; presence/routing/accuracy and any LSE implementation
remain unverified alternatives, not a qualified clock.
https://www.st.com/resource/en/user_manual/dm00789949.pdf

Microsoft documents QPC and Stopwatch for intervals independent of UTC, with
resolution/accuracy distinct. No UTC, SI calibration or ppm guarantee inferred.
https://learn.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps

First finish documentary audit, then identify board/image, preserve verified
A7 image/backups, program probe normally only under existing qualified chain.
Keep SD in PC and nativeUSB off. User alone connects power. After calibration,
preserve result and determine next candidate; no automatic hour or8h start.

Implementation: enable both R3_ENDURANCE_1H and R3_QPC_WITNESS. Q1N0003/5
names, header/footer v2 still express requested/actual MCU nominal duration.
QPC2 is a distinct48byte witness packet: magic4, UID12, seq4, MCUus8,
flags4 (active1/error2/complete4 plus run<<8), session8, accepted4, CRC4.
USART service sends at most one byte per poll and never waits during FIFO;
its cost is included in the unchanged service/gap limits. It also works in
WAIT/after terminal. Host records every query, waits for the first active
packet, then3601QPCseconds (1s explicit observation margin over3600), takes
second active packet and observes a successful terminal. It cannot stop or
start acquisition; GDB only triggers RAM and detaches before capture, then
is permitted again only after successful terminal packet (SD already quiet).
Short gate uses1QPCsecond within2nominal MCU seconds, with newfile; no replay
of H1N. Inactive/error/changed session/too-short interval fail verification.
No changes to DSP, sample units, archive DC, queues, SPI clocks or guards.
