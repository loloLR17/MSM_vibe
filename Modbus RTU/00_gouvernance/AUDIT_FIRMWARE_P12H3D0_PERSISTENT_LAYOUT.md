# AUDIT — Firmware P12-H3d0 — Persistent layout after C4-E

## Scope

This audit derives the active persistent-media layout from current main after the C4-E bounded V3 command-journal switch.

No physical STM32 non-volatile technology is selected here. This tranche is descriptive only: exact logical regions, offsets, sizes, current media contract, and remaining physical constraints.

## Active logical layout

SystemRuntime lays out the regions contiguously, without explicit padding or alignment gaps.

| Region | Base offset | Size (bytes) | End exclusive |
|---|---:|---:|---:|
| Active configuration store | 0 | 280 | 280 |
| Time history | 280 | 18 | 298 |
| Campaign repository | 298 | 4,072 | 4,370 |
| Campaign data | 4,370 | 11,552 | 15,922 |
| B5 bounded command journal V3 | 15,922 | 35,840 | 51,762 |
| Diagnostic + self-test history | 51,762 | 40 | 51,802 |
| Boot intent | 51,802 | 16 | 51,818 |

Minimum logical media span required by the current SystemRuntime layout:

    51,818 bytes

No spare/reserved area, layout header, magic, global format version, migration marker, erase-block alignment, or wear-leveling metadata is included in this number.

## Derivation

### Configuration

- TR2_CONFIGURATION_RECORD_SIZE = 140
- 2 slots
- total = 280 bytes

### Time history

- TR2_TIME_HISTORY_RECORD_SIZE = 18 bytes

### Campaign repository

- allocator: 2 x 20 = 40 bytes
- campaign metadata: 8 campaigns x 2 copies x 252 = 4,032 bytes
- total = 4,072 bytes

### Campaign data

Per slot:

- descriptors: 2 x 36 = 72 bytes
- chunks: 32 x 88 = 2,816 bytes
- slot = 2,888 bytes

Four slots:

- total = 11,552 bytes

### B5 bounded command journal V3

- 256 logical slots
- 2 copies per slot
- 70 bytes per record
- total = 256 x 2 x 70 = 35,840 bytes

### Diagnostic/self-test

- diagnostic history = 20 bytes
- self-test history = 20 bytes
- total = 40 bytes

### Boot intent

- TR2_BOOT_INTENT_RECORD_SIZE = 16 bytes

## Current logical media contract

PersistentMedia exposes only:

- read(offset, buffer, size)
- write(offset, buffer, size)
- commit()

PersistentMediaRegion adds bounds checking and maps a child region onto a parent media offset.

The current contract does not express:

- total physical capacity;
- erase operation;
- erase/program granularity;
- required write alignment;
- erased value;
- one-way programming constraints;
- endurance/write-cycle limits;
- atomic write granularity;
- commit durability guarantees;
- page/sector geometry;
- bad-block handling;
- wear leveling;
- media identity;
- global persistent-layout version.

Therefore the current logical persistence layer must not yet be assumed directly implementable by arbitrary STM32 internal Flash or any specific external NVM.

## Alignment observation

The current layout is byte-packed. Region bases include:

- 280
- 298
- 4,370
- 15,922
- 51,762
- 51,802

No general power-of-two alignment invariant is currently enforced. This is valid for the host byte-array model, but compatibility with the selected physical NVM must be established explicitly.

## Endurance observation

The audit establishes sizes and write structure, but current source constants alone do not establish acceptable physical endurance.

Write frequency differs materially by region. In particular, command-journal mutations, campaign data chunks, time synchronization history, diagnostics, and boot intent have different update patterns.

H3d1 must therefore evaluate endurance and atomicity against actual expected write rates rather than selecting media from capacity alone.

## Persistent-layout compatibility

C4-E moved every region following the command journal because the active journal changed from the legacy dense V2 geometry to bounded V3.

There is currently no global persistent-layout version at offset 0 and no defined V2-to-V3 on-media migration.

An existing V2-formatted medium must not be silently interpreted using this V3 layout.

H3d must define an explicit clean-format/version/migration policy before production persistent media is accepted.

## H3d0 conclusion

The active runtime needs at least 51,818 logical persistent bytes.

This value is a logical payload requirement, not yet a physical NVM capacity requirement.

H3d1 must choose/qualify a physical persistence strategy against:

1. capacity including metadata/reserve;
2. write and erase granularity;
3. atomicity and commit semantics;
4. endurance;
5. power-loss behavior;
6. byte-packed current layout or an explicitly versioned replacement layout;
7. STM32U575 hardware resources and board constraints.

No physical persistence technology is frozen by H3d0.
