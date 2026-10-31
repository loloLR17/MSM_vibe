# R3-8H — experimental autonomous profile

Mission `TR2-20261030-R3-8H-FAT32-AUTONOME`. Baseline source24aaf06a and filesQ1N remain historical, never overwritten. This profile is opt-in:
`R3_ENDURANCE_8H=ON R3_QPC_WITNESS=ON R3_ENDURANCE_1H=ON`.
The latter retains the established no-format/calibration→short→long command gates;
it does not select a one-hour campaign when8H is enabled. Prefix **E8N**, new
`E8N0003.TR2` / `E8N0005.TR2` required absent. No boot autostart/retry.

## Capacity and time

Long guard **30,600,000,000 MCU nominal microseconds** (8.5h nominal), not SI.
QPC must prove **28,801s inside active capture**, including1s observation margin.
Measured1h clock ratio1.00674..1.00677 motivates margin; even an **assumed**1.05
ratio leaves30600/1.05=29142.857QPCseconds,342s beyond the witness threshold.
This assumption is sizing only: a failed/absent active second witness is NOT PASS.
A rate assumption28000nativeXYZ/MCUnominalsecond yields856800000native,
85680000outputs,514080000usefulbytes,1098462records512B,562413568B including
header/footer. **640MiB=671088640B** gives108675072B headroom; no blind queue
increase (64KiB unchanged). The real extent boundary still fails closed if a rate
assumption proves insufficient. FAT32 remains below4GiB, required20480clusters
at32KiB. Every allocated cluster checked contiguous, file synced/closed before FIFO.

Retainedu32 bounds: accepted856.8M, output85.68M, bytes514.08M, framed562.41M,
polls<=122.4M from minimum250us pacing, SysTick30.6M, DEC8groups<=107.1M;
all<2^32. FIFO native/index and clock sums alreadyu64. DWT accumulator4.896e12
cycles fitsu64. Clock sampler runs eachpoll and everyidleSysTickwake; newE8-only
crosscheck against SysTick refuses>=25s observationgaps and >4ms phase discrepancy
(the latter does NOT relax work/service/gap guards). IRQs remain enabled; both
clocks share oscillator, so this check detects missed DWT wraps, **not** SI drift.
A physical MCU halt/IRQ blackout is not magically recoverable and cannot qualify
continuous acquisition. Hardware cadence/identity/failures remain gate criteria.
Raw sensor32 counter uses modulo differences at eachDEC8 anchor/100ms track;
80kHz reference wraps~14.9h, so24h needs modulo tracking already provided and tested
in `tests/platform/test_fifo_event_time.c`; scientific mapper is unchanged.
24h inventory: duration86.4e9 requiresu64; native<=2.4192B, bytes<=1.45152B,
framed~1.589GB fitu32/FAT32 but **not this640MiB allocation**. No24h run authorised.

## Version3 archive — explicit new meaning

Little endian, header/record/footer512B unchanged; IEEECRC32 rules unchanged.
Header8 version3; legacyduration20 **reserved zero**; requested durationu64 at168;
header176..507 zero. Footer8 version3; legacyduration44 **reserved zero**;
actual durationu64 at144; footer152..507 zero. Sourceu64 endpoints remain128/136
and low32words88/92 crosschecked as in v2. All other v2 fields retain their meaning,
including unknownUTC, DCretained,±8g logical,/10 candidate, coefficient/source SHA.
Reader accepts v1/v2 with exactly their previous duration meanings; unknownversions,
v3reserved/truncated/incoherent fields fail. COMPLETE is integrity, not elapsed
physical time; combine same-session raw QPC evidence with actual PC file/digest.
The Result debug layout is version4 only in8H (duration_us u64), old profilesv3.

No FIR/DEC8/scientific-core change. SPI3=8MHz/SPI2=4MHz, quanta64words/128B,
CMD25groups1..8, queue64KiB and1000/2500/5000us limits retained. Flush has30s global
budget in8H plus existing per-command timeouts; FIFO stops before drain. Fault
retains INCOMPLETE/no successfooter; no automatic recovery or replay. Error UART
is emitted only after normal FIFOstop/SDcleanup, never as COMPLETE; unsafe cleanup
still latches requiring an explicit later intervention. Durable fault reporting
under arbitrary powerloss is NOT qualified.

## Native passive Windows observer, no Codex during8h

`experiments/r3_time_reference/tools/observe_endurance.ps1`, instrumentversion3,
uses the existing QPC2 format and exactUID/COM5 ancestor guard; COM4 forbidden.
E8 advertises ACTIVE only once the first SDgroup has completed, so the initial
witness also proves data were actually acknowledged on the extent. OnebyteUART
perpoll, no periodicMCUTX, observer only sendsQ; all journal files use native local
Windows storage. No network, START, STOP, debugger or execution-policy changes.
`launch_observer.py` uses local `Win32_Process.Create` through CIM, then exits. Initial Start-Process
trial left a WSL standard-handle wait despite a surviving native child; no capture
was triggered. Native CIM creation returns before the child finishes, verified
by a host-only delay/file smoke and the single physical short gate. No elevation,
new service, scheduled task or Windows policy change.
This is a passive instrument explicitly allowed by the mission, not a background
Codex/WSL monitor. It uses the readable command transport already accepted for1h;
no ParseInput/fixture/EncodedCommand/Add-Type or retry of a blocked diagnostic.
If Windows refuses it, stop rather than re-encode/rewrite to evade that refusal.

Short gate is also detached and uses minimum1s before the2s MCUstop. For long,
use minimum28801 and terminalgrace3600s. Ready/journal/summary remain after parent
and Codex exit. Observer failure never stops/restarts MCU. Its first/second
raw QPC brackets must be independently analysed on resume, never inferred from UTC.
Power must remain until post-run RAM producerdigest is captured; **do not remove
SD/power merely because8h have passed**. Resume Codex under the SAME mission,
confirm terminal/SDquiet, dumpRAM/histograms/producerhash, detach, then request
physical poweroff/SD→PC. Compare every usefulbyte against that separate producer
SHA, not only the footer. Preserve historical10files+Info, noformat.

Windows source: [Win32_Process.Create](https://learn.microsoft.com/en-us/windows/win32/cimwin32prov/create-method-in-class-win32-process).
Native detached lifetime is also observed on the short gate; documentation alone
is not physical proof. Read power
state/ACsleep/hibernate/USB state without modifying protections; user retains
physical power and must confirm sector power/no manual sleep/reboot duringrun.
No software power-policy or security exceptions implicit in this profile.

## FASTSTART differential SPI transport, 31/10/2026

After relocation and connector reseat, the unchanged split command/payload
transport failed FIFO tag checks during calibration, before any SD capture.
User reports VDD=VDDIO=3.3V; this is a static measurement, not signal integrity.
The failed burst shows initially valid tags then an apparent one-bit shift.
This resembles the documented R4-U split failure; it does not establish its cause.

Opt-in **R3_SPI_DUPLEX=ON**, restricted to the explicit 8h profile, selects the
existing `platform.c` BENCH_DUPLEX path: one HAL_SPI_TransmitReceive frame,
command then payload with CS low, command RX excluded. Default remains OFF,
so previous profiles retain their existing transport. No new SPI implementation.
SPI3 stays8MHz/mode3, SPI2 stays4MHz; quantum64, RXpreclose/OVR/tag/counter/
timestamp/configuration and1000/2500/5000us guards are unchanged. FIR/gain/FS/
archivev3/int16/duration/footer quality gate/importer unchanged.

Build and physical calibration/short gate are required for this new combination;
a previous R4-U success is not R3/FIFO+SD/endurance evidence. Fresh N8D filenames
preserve N8A/B/C artifacts. Never repair shifted data or relax parity to start.
No long START without the explicit user signal and a successful current gate.

Differential results (31/10): duplex8MHz passed FIFO timestamp continuity during
the calibration interval but direct timestamp reads were incoherent, so the
unchanged100us calibration guard rejected it. SCKMEDIUM8MHz also rejected;
duplex4MHz rejected configuration before calibration. No8h/gate capture.
This opt-in remains experimental/unqualified; defaultOFF is retained.
