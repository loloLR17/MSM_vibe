# CONCEPTION — Firmware P12-H3d1-C — PersistentMedia commit semantics and physical transaction protocol

## Status

Design tranche only. No firmware behavior or physical component is frozen by this document.

The purpose is to make the existing PersistentMedia semantics explicit and derive a power-loss-safe physical protocol suitable for byte-addressable nonvolatile media such as serial F-RAM.

## 1. Semantics already implemented by the host reference

Current host_platform.c is the strongest executable statement of the PersistentMedia contract:

- read() reads only persistent_committed;
- write() modifies only persistent_candidate;
- commit() copies the complete candidate image to persistent_committed.

Therefore the logical contract is image transactional, not merely "flush pending bus writes".

Let C be the last committed logical image and W the candidate image.

After initialization/recovery:
    W = C

After write(offset, bytes):
    W[offset..] changes
    C does not change
    read() still returns C

After commit() success:
    C := W

If power is lost before commit succeeds:
    recovery must expose the previous C.

If commit reports failure:
    physical outcome may be ambiguous; after reboot recovery must select one complete valid committed image, never a mixture.

A successful commit is the publication point.

## 2. Important consequence for campaign data

Campaign append writes chunk records before checkpoint.

Those chunk writes must not become visible through PersistentMedia.read() until a successful commit.

publish_checkpoint() performs:
1. commit pending chunks;
2. write descriptor;
3. commit descriptor.

This intentionally creates two publication points:
- durable chunk bytes first;
- descriptor publication second.

After a power loss between those commits, chunks may be physically retained but the older descriptor remains authoritative and therefore they are not part of the durable campaign prefix.

Any physical protocol must preserve this behavior.

## 3. Rejected direct-FRAM mapping

Rejected:
    read()   -> raw FRAM read
    write()  -> raw FRAM write
    commit() -> no-op

Reason:
- write() would become immediately visible;
- uncommitted candidate state would survive reboot as authoritative;
- campaign checkpoint semantics would change;
- the host fault model would no longer represent production.

F-RAM endurance and byte addressing solve physical wear/erase problems, but not transactional publication.

## 4. Candidate physical protocol: dual full-image generations

Given the prototype hardware already procured has 256 KiB F-RAM, the simplest protocol worth qualifying is deliberately conservative: two complete logical images plus redundant superblocks.

Logical payload:
    L = 51,818 bytes

Physical image A:
    image header + 51,818-byte payload + integrity metadata

Physical image B:
    image header + 51,818-byte payload + integrity metadata

Superblock copies:
    small redundant records identifying format/layout and the last published generation/image.

At 256 KiB, two raw payload images consume:
    2 x 51,818 = 103,636 bytes

Remaining before headers/superblocks:
    262,144 - 103,636 = 158,508 bytes

Therefore full-image A/B is comfortably feasible on the procured prototype F-RAM.

## 5. Runtime semantics

Maintain in RAM:
- active physical image identifier;
- committed generation;
- candidate buffer or candidate reconstruction state;
- dirty-range metadata.

### read()
Returns the committed logical view only.

### write()
Updates the candidate logical view only.
It must not modify the currently authoritative committed image.

For the first implementation, the simplest reference behavior is to keep a 51,818-byte candidate image in RAM, mirroring host semantics exactly.

This RAM cost is significant but fits within the STM32U575 768-Kbyte SRAM budget in principle. It must nevertheless be included in final linker/stack/RAM budgeting before freeze.

A later optimization may replace the full candidate RAM image with dirty ranges or a staging image, but only if semantics remain identical.

### commit()
Proposed publication sequence:

1. choose inactive physical image;
2. write a new image header in non-published state with next generation/layout version;
3. write the complete candidate payload to the inactive image;
4. compute/write payload integrity metadata;
5. verify/read-back as required by the qualification policy;
6. write a valid image-finalization marker/integrity record;
7. publish the new generation through redundant superblock metadata;
8. only after publication succeeds, switch runtime committed image/generation.

The previously committed physical image is never modified during this transaction.

## 6. Recovery

At boot:

1. read and validate both superblock copies;
2. validate referenced image header, layout version, generation and payload integrity;
3. if newest published image is invalid, fall back only according to explicitly defined generation/superblock rules;
4. never merge A and B;
5. expose exactly one complete committed logical image;
6. initialize candidate state from that committed image.

If neither image/superblock combination is valid:
- classify media EMPTY, UNSUPPORTED, CORRUPTED or UNAVAILABLE according to the later media-format contract;
- do not silently format or reinterpret legacy V2 layout.

## 7. Power-loss cases to qualify

Fault injection must cover at least:

A. loss before any inactive-image write
    -> old image remains authoritative.

B. loss during image header
    -> old image remains authoritative.

C. loss at arbitrary payload cut
    -> old image remains authoritative.

D. loss during integrity metadata/finalization
    -> old image remains authoritative.

E. loss after complete inactive image but before publication
    -> old image remains authoritative.

F. loss/torn write during publication superblock
    -> recovery selects a complete valid published generation; outcome may be old or new depending on whether publication became durable, never mixed.

G. loss immediately after successful publication
    -> new image authoritative.

H. repeated reboot/recovery without writes
    -> stable, no mutation.

I. generation exhaustion
    -> fail closed before wrap.

J. unsupported layout version
    -> explicit UNSUPPORTED, no reinterpretation.

## 8. Superblock requirements

Exact byte format is deferred to H3d1-D, but it must include at least:
- magic;
- physical-media format version;
- logical persistent-layout version;
- generation;
- active image identifier or image base;
- logical payload size;
- integrity protection (CRC or stronger accidental-corruption detector as later selected).

Two superblock copies are required so publication itself is not a single point of failure.

Publication records must use generation ordering and canonical validation, following the same design discipline already used for B5 A/B records.

## 9. Layout-version boundary

The physical-media format version and logical TR2 persistent-layout version are distinct concepts.

Physical-media format:
- describes the A/B image and publication protocol.

Logical persistent-layout version:
- describes offsets/sizes of configuration, time history, campaigns, B5 V3, diagnostics and boot intent inside the 51,818-byte logical image.

Both must be explicit.

No V2 dense-journal media may be silently accepted as the current bounded-V3 logical layout.

## 10. Why full-image A/B is preferred for the prototype

This is intentionally not the most storage-efficient protocol.

It is preferred for first qualification because:
- semantics match the existing host model directly;
- no erase/wear-leveling layer is needed on F-RAM;
- recovery has one clear authority;
- fault injection is finite and auditable;
- 256 KiB prototype capacity makes the space cost irrelevant;
- it avoids introducing a second complex journal beneath the already journaled portable stores.

The cost is write amplification: every commit can write roughly 51.8 KiB even for a 16-byte logical change.

F-RAM endurance makes wear acceptable in principle, but bus time, power budget and command/campaign commit rate must be measured before production freeze.

Therefore this protocol is a qualification baseline, not yet the final optimized production implementation.

## 11. Capacity conclusion

For the procured 256-KiB F-RAM class:

    physical capacity        262,144 B
    two logical payloads    103,636 B
    remaining               158,508 B

This leaves ample room for:
- image headers;
- redundant superblocks;
- alignment/reserved space;
- future format metadata;
- qualification instrumentation if desired.

Capacity no longer forces an in-place or delta-journal design for the prototype.

## 12. H3d1-C decision boundary

H3d1-C establishes the reference semantic model:

- PersistentMedia is image-transactional.
- write() changes candidate state only.
- read() exposes committed state only.
- commit() atomically publishes a complete logical image.
- power loss exposes either the previous or the new complete committed image, never a mixture.
- direct raw-FRAM mapping is forbidden.
- dual full-image generations are the preferred prototype protocol to qualify next.

Not frozen yet:
- exact physical record formats;
- exact superblock sizes/offsets;
- CRC choice;
- SPI instance/pins/clock;
- exact F-RAM component;
- RAM-buffer implementation;
- optimization of write amplification.

Next tranche: H3d1-D — exact physical A/B layout and publication record format, followed by host fault-injection qualification before STM32/F-RAM driver integration.
