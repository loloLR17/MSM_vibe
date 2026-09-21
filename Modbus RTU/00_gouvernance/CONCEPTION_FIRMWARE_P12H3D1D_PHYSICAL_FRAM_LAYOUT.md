# CONCEPTION — Firmware P12-H3d1-D — Exact physical F-RAM A/B layout

## Status

Design tranche. No STM32 SPI driver or production component is frozen here.

Input baseline:
- logical PersistentMedia payload: 51,818 bytes;
- PersistentMedia image-transactional semantics frozen as H3d1-C design basis;
- prototype F-RAM class available: 256 KiB;
- direct raw-FRAM mapping is forbidden.

All multi-byte integers in this physical format are big-endian, consistent with the existing portable persistence encoders.

## 1. Physical address space

Reference capacity:
    262,144 bytes = 0x40000

The layout deliberately uses round 4-KiB boundaries even though F-RAM has no erase-page requirement. This provides simple inspection, future metadata growth and clean separation.

| Region | Base | Size | End exclusive |
|---|---:|---:|---:|
| Superblock A area | 0x00000 | 0x01000 (4,096) | 0x01000 |
| Superblock B area | 0x01000 | 0x01000 (4,096) | 0x02000 |
| Image A area | 0x02000 | 0x0E000 (57,344) | 0x10000 |
| Image B area | 0x10000 | 0x0E000 (57,344) | 0x1E000 |
| Reserved | 0x1E000 | 0x22000 (139,264) | 0x40000 |

Each image area is larger than the 51,818-byte logical payload and leaves 5,526 bytes for image header/trailer and future growth.

No current-format record may write into Reserved.

## 2. Constants

Physical media magic:
    "TR2M" = 0x5452324D

Image magic:
    "TR2I" = 0x54523249

Physical media format version:
    1

Logical persistent-layout version:
    1

The logical-layout version 1 identifies the post-C4-E bounded-V3 layout whose exact logical span is 51,818 bytes. It must not be used for legacy dense-V2 media.

Generation:
    uint64, values 1..UINT64_MAX-1
    0 = invalid/unpublished
    UINT64_MAX is not admitted as a next generation.
No wrap is permitted.

## 3. Image record

Image area begins with a 64-byte header.

### Image header — 64 bytes

| Offset | Size | Field |
|---:|---:|---|
| 0 | 4 | image_magic = TR2I |
| 4 | 2 | physical_format_version |
| 6 | 2 | logical_layout_version |
| 8 | 8 | generation |
| 16 | 4 | logical_payload_size = 51,818 |
| 20 | 4 | header_size = 64 |
| 24 | 4 | payload_crc32 |
| 28 | 4 | reserved = 0 |
| 32 | 28 | reserved = 0 |
| 60 | 4 | header_crc32 over bytes 0..59 |

Payload begins at image-area offset 64.

Payload:
    51,818 bytes

Payload end within image area:
    64 + 51,818 = 51,882 bytes

The remaining bytes of the 57,344-byte image area are reserved and ignored by format v1.

### Image validity

An image is VALID only if:
- magic matches;
- physical format version supported;
- logical layout version supported;
- generation is valid;
- payload size equals the supported logical span;
- header size equals 64;
- reserved header fields are zero;
- header CRC32 is valid;
- payload CRC32 over exactly 51,818 bytes is valid.

A partially written image is never authoritative merely because some fields decode.

## 4. Superblock publication record

Only the first 64 bytes of each 4-KiB superblock area are used in v1.

### Superblock — 64 bytes

| Offset | Size | Field |
|---:|---:|---|
| 0 | 4 | media_magic = TR2M |
| 4 | 2 | physical_format_version |
| 6 | 2 | logical_layout_version |
| 8 | 8 | generation |
| 16 | 1 | active_image (0=A, 1=B) |
| 17 | 3 | reserved = 0 |
| 20 | 4 | logical_payload_size = 51,818 |
| 24 | 4 | image_header_size = 64 |
| 28 | 4 | reserved = 0 |
| 32 | 28 | reserved = 0 |
| 60 | 4 | record_crc32 over bytes 0..59 |

A superblock is VALID only if all fixed fields and CRC are valid and it references an image that is itself VALID with the same generation and versions.

## 5. CRC32

For design consistency with existing persistence code, reference CRC32 is the same reflected CRC-32 algorithm already used in TR2 records:
- initial value 0xFFFFFFFF;
- polynomial 0xEDB88320;
- final XOR 0xFFFFFFFF.

This choice is for accidental corruption/torn-write detection, not cryptographic authenticity.

Before implementation, the algorithm should be factored or reused without creating divergent CRC implementations if practical.

## 6. Publication algorithm

Assume image A / generation G is currently authoritative.

Candidate state is held separately from the committed view.

commit():

1. reject if current generation cannot be incremented safely;
2. choose inactive image B;
3. encode/write complete B header+payload for generation G+1;
4. read back and validate B completely;
5. choose the older/invalid superblock copy as publication target;
6. encode/write one complete superblock referencing B/G+1;
7. read back and validate that superblock against B;
8. only then report commit success and switch runtime authority to B/G+1.

The old authoritative image and at least one old valid publication record are never modified before the new image has been completely validated.

The next commit reverses A/B.

## 7. Superblock copy selection

At recovery, both superblocks are independently validated against their referenced images.

Cases:
- neither valid: EMPTY/CORRUPTED/UNSUPPORTED according to classification rules below;
- one valid: use it;
- two valid, different generations: use highest valid generation;
- two valid, same generation and same canonical content: use either;
- two valid, same generation but different canonical content: CORRUPTED.

For publication, overwrite the invalid copy if one exists; otherwise overwrite the copy with the lower generation.

If both valid copies have equal generation/content, either physical copy may be selected deterministically; v1 selects B when A is current publication source and A otherwise.

No superblock is erased.

## 8. Recovery classification

UNAVAILABLE:
- physical read/transport failure prevents classification.

EMPTY:
- both superblock records and both image headers are entirely 0x00 or entirely 0xFF, with no contradictory non-empty metadata.

UNSUPPORTED:
- structurally recognizable TR2 media/image magic with unsupported physical or logical layout version, unless a higher-priority physical read failure exists.

CORRUPTED:
- non-empty recognized/current-format metadata exists but no valid authoritative superblock-image pair can be selected;
- duplicate same-generation conflicting publication records;
- CRC/field mismatch prevents authority.

VALID:
- one authoritative superblock-image pair selected.

No automatic format/repair occurs during recovery.

## 9. First format

Formatting is an explicit operation outside ordinary recovery.

Reference first-format sequence:
1. construct a zero-initialized logical payload matching current empty-store semantics;
2. write/validate Image A generation 1;
3. write/validate Superblock A generation 1 -> Image A;
4. leave B areas non-authoritative.

The exact initial logical bytes must be checked against all current store empty-image rules before implementation. Existing stores generally accept all-0x00 or all-0xFF records as empty, but the global initialization image must be qualified by SystemRuntime tests rather than assumed.

## 10. Candidate RAM

Reference prototype implementation keeps:
- committed logical read view sourced from active F-RAM image;
- a 51,818-byte candidate RAM buffer initialized from the committed image after recovery;
- dirty flag/range bookkeeping optional.

write():
- bounds-check;
- modify candidate RAM only.

read():
- read committed bytes from active physical image payload, or from a separate committed RAM cache if later justified;
- never return candidate bytes before commit success.

commit success:
- candidate becomes committed by publication.

commit failure:
- runtime must enter recovery-required/ambiguous media state before accepting further persistence mutations unless the backend can prove old authority remains intact.
- reboot/recovery determines old-vs-new authority.

## 11. Power-loss cut points

Host qualification must inject failure/torn persistence at:
- every byte cut of image header;
- representative and boundary cuts through payload, including first byte, region boundaries and final byte;
- image header CRC;
- superblock bytes 0..63;
- read-back validation;
- transport errors before/during/after publication.

For exhaustive byte-cut qualification of the entire 51,818-byte payload, test runtime cost may be high. A deterministic representative matrix plus boundary sweep is acceptable initially only if complemented by a lower-level generic torn-write proof that is independent of offset.

Mandatory invariant:
    recovery returns old complete image or new complete image, never mixed candidate state.

## 12. Capacity

Image A area: 57,344 B
Image B area: 57,344 B
Superblock areas: 8,192 B
Reserved: 139,264 B

Used/reserved by format map before global Reserved:
    122,880 B

Free Reserved:
    139,264 B

Logical payload occupancy of total device:
    51,818 / 262,144 ~= 19.8%

Two-image payload occupancy:
    103,636 / 262,144 ~= 39.5%

The format therefore leaves substantial room for future logical-layout growth without redesigning physical addresses.

## 13. Prototype readiness boundary

Before hardware integration:
- implement this media format on host/fault media;
- qualify format/recovery/publication;
- validate SystemRuntime on the transactional backend;
- define explicit format command/API;
- budget 51,818-byte candidate RAM;
- verify actual procured F-RAM command/address/protection behavior from manufacturer documentation.

Hardware-specific SPI instance, pins, chip select, clock, write-enable protocol, status-register handling and board wiring remain outside H3d1-D.

## 14. H3d1-D output

This document freezes no hardware yet.

It defines the exact reference format to implement and falsify in H3d2-host:
- two 57,344-byte image areas;
- two redundant 64-byte publication records in separate 4-KiB areas;
- explicit physical and logical versions;
- uint64 monotonic generation;
- CRC32-protected image and publication metadata;
- complete payload CRC;
- no in-place mutation of active image during commit;
- no silent format, repair or legacy reinterpretation.
