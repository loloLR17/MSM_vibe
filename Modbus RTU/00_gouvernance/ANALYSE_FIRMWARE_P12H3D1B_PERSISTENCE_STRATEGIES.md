# ANALYSE — Firmware P12-H3d1-B — Physical persistence strategies

## Context

H3d0 established an active logical payload of 51,818 bytes.
H3d1-A established that PersistentMedia is not merely a byte store: current portable stores depend on arbitrary byte-range writes plus a commit durability boundary, with high-churn B5 and campaign-data regions and several fixed-address hotspots.

This tranche compares physical strategies. It does not yet freeze a component or implementation.

## A — STM32U575 internal Flash

### Strengths
- no additional component, bus, BOM or PCB routing;
- enough raw capacity in principle;
- STM32U5 dual-bank Flash supports read-while-write;
- 8-Kbyte erase pages;
- endurance is 10 kcycles generally, with up to 100 kcycles on at most 256 Kbytes per bank under ST's stated high-cycling condition.

### Mismatch with current PersistentMedia
The portable layer issues byte-packed overwrites of 16, 18, 20, 36, 70, 88, 140 and 252 bytes. Raw internal Flash has page erase constraints and cannot directly implement arbitrary rewrites or boot-intent zeroing.

A correct backend therefore needs a Flash translation layer providing:
- page allocation/logging;
- copy-on-write or journaled page reconstruction;
- commit markers;
- garbage collection;
- wear distribution;
- recovery after interrupted erase/program/GC;
- layout/version metadata.

The resulting physical footprint must exceed 51,818 bytes because spare pages are required for transactional replacement and garbage collection.

### Endurance risk
Even 100 kcycles is not directly comparable to logical-record write counts: one logical mutation can eventually cause an 8-Kbyte page erase. Hotspot records and B5 traffic therefore make lifetime dependent on translation-layer quality and workload.

Conclusion: technically possible, but highest firmware complexity and qualification burden.

## B — External serial EEPROM

A concrete reference class is ST M24512-A125:
- 512 Kbit / 64 Kbyte;
- I2C up to 1 MHz;
- 128-byte page;
- byte/page write within 4 ms;
- endurance specified by ST as 4 M cycles at 25 C, 1.2 M at 85 C and 600 k at 125 C.

64 Kbyte is only 13,718 bytes above the current 51,818-byte logical payload, before global layout metadata, transactional shadowing or spare area.

### Strengths
- byte-addressable semantics are substantially closer to current stores than raw MCU Flash;
- no explicit erase-page management exposed to firmware;
- substantially greater endurance than raw Flash;
- simple low-pin-count I2C or SPI parts exist.

### Limits
- writes have millisecond-scale internal programming time;
- endurance remains finite and fixed-address hotspots remain relevant;
- page-boundary handling is required;
- power loss during EEPROM internal programming still requires qualification;
- 64 Kbyte density leaves limited space for a robust media-level transactional layer.

Conclusion: viable for low/moderate churn, but 64 Kbyte is uncomfortably tight for the complete current PersistentMedia contract and high-churn regions.

## C — External serial F-RAM

A concrete reference class is Infineon FM25V05-G:
- 512 Kbit / 64 Kbyte;
- SPI;
- 2.0 to 3.6 V;
- up to 40 MHz;
- industrial -40 C to +85 C;
- 10^14 read/write endurance class;
- nonvolatile writes occur at bus speed without EEPROM-style write delay.

### Strengths
- semantics are very close to byte-addressable RAM;
- no erase cycle;
- no page erase/reconstruction;
- extremely high endurance removes the practical hotspot problem for this workload;
- fast writes greatly reduce the power-loss exposure window;
- 3.3-V-compatible variants fit the MCU electrical domain.

### Important semantic point
F-RAM does not automatically implement the current candidate-write/commit transaction model. A physical byte write is already nonvolatile.

Therefore a direct mapping:
    PersistentMedia.write() -> F-RAM write
    PersistentMedia.commit() -> no-op
would violate the campaign checkpoint abstraction and the host fault model.

A small transactional media protocol is still required, for example:
- versioned physical layout;
- committed-generation/root metadata;
- copy-on-write/staging for writes belonging to one commit epoch;
- atomic publication of the committed epoch/root;
- recovery selecting only the last committed epoch.

F-RAM makes this layer much simpler than Flash because no erase or wear-leveling machinery is required.

### Capacity
A single 64-Kbyte F-RAM can physically contain the 51,818-byte logical image, but only 13,718 bytes remain. Whether that is enough for transactional metadata/staging depends on the chosen media protocol. A full shadow copy of 51,818 bytes would not fit.

Conclusion: strongest technology match to write frequency and endurance, but capacity must be selected together with the transactional protocol; 64 Kbyte should not be frozen prematurely.

## D — Hybrid architecture

Possible split:
- low-churn configuration/metadata in internal Flash or EEPROM;
- high-churn B5/campaign/hotspot records in F-RAM.

### Strengths
- can reduce F-RAM capacity;
- can reserve internal high-endurance Flash for low-frequency state.

### Costs
- two physical persistence authorities/backends;
- more routing/components or more Flash translation logic;
- more complex boot/recovery and layout-version rules;
- SystemRuntime currently receives one PersistentMedia authority, so composition would need explicit partitioning or a composite media layer.

Conclusion: possible, but complexity is not currently justified unless BOM/capacity constraints rule out one sufficiently large external NVM.

## Comparative result

| Criterion | Internal U5 Flash | Serial EEPROM | Serial F-RAM | Hybrid |
|---|---|---|---|---|
| Capacity for 51,818 B | yes with reserved Flash | 64 KB class barely | 64 KB class barely; larger preferred | configurable |
| Arbitrary overwrite fit | poor | good | excellent | mixed |
| Erase management | required | internal to device | none | mixed |
| Hotspot endurance | translation dependent | finite, M-cycle class | extremely high | mixed |
| Write latency | erase/program dependent | ms write cycle | bus-speed | mixed |
| Wear leveling | required for high churn | may still be desirable | unnecessary for endurance | mixed |
| Transaction layer for commit semantics | substantial | required | required but simpler | substantial |
| Extra component | no | yes | yes | likely |
| Firmware complexity | highest | medium | lowest physical-media complexity | highest system complexity |

## H3d1-B orientation

For the TR2 workload, external serial F-RAM is the leading architecture candidate because it directly removes the two hardest physical problems exposed by H3d1-A: erase granularity and write endurance.

This is an orientation, not a freeze.

Before selecting a specific part, H3d1-C must:
1. define the exact PersistentMedia commit semantics required by the portable stores;
2. design a minimal power-loss-safe physical transaction protocol;
3. calculate the physical capacity required by that protocol;
4. only then select the F-RAM density/interface/component;
5. compare the resulting design once more against EEPROM/internal Flash before final arbitration.

## Rejected shortcut

Do not implement F-RAM write() as immediate physical write with commit() as a no-op.

Although F-RAM itself is nonvolatile, that shortcut would make uncommitted campaign chunks visible after reboot and would change the already-qualified portable persistence semantics.
