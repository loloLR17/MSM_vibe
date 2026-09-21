# AUDIT — Firmware P12-H3d1-A — Persistent mutation profile

## Scope

This audit classifies persistent writes in current main after C4-E.

It does not select a physical NVM and does not modify firmware behavior. Its purpose is to determine which logical regions are low-, medium-, or high-churn and what atomicity/endurance characteristics a physical PersistentMedia implementation must support.

## Cross-cutting contract

PersistentStorageCore forwards byte-range read/write/commit directly to PersistentMedia.

A write is not, by itself, the durability boundary. Stores generally call commit() after publishing a record. Campaign data is the notable case where chunk writes may accumulate before a checkpoint commit publishes the durable prefix.

The physical backend must therefore preserve the semantic distinction between candidate writes and committed durable state already exercised by the host fault-injection model.

## Region mutation inventory

### Active configuration — 280 B

Record size: 140 B, two alternating slots.

Mutation:
- one 140-byte record write;
- one commit;
- target alternates according to recovered generation.

Trigger:
- accepted configuration activation/commit.

Profile:
- low expected churn;
- copy-on-write style redundancy;
- power-loss tolerance depends on preserving the previous valid slot until the new commit is durable.

### Time history — 18 B

Mutation:
- overwrite the same 18-byte logical location;
- one commit.

Trigger:
- durable last-time-synchronization history update.

Profile:
- potentially recurring;
- single logical record, no A/B copy in this store;
- physical backend must make overwrite+commit power-loss safe or provide transactional indirection below PersistentMedia.

### Campaign repository — 4,072 B

Allocator:
- two 20-byte allocator records alternate by generation;
- reserve campaign ID writes one 20-byte record + commit.

Campaign metadata:
- open writes copy 0 of one 252-byte slot + commit;
- close writes copy 1 of that slot + commit.

Profile:
- event-driven per campaign, not sample-rate driven;
- moderate/low churn;
- redundancy exists at allocator and OPEN/CLOSED metadata level.

### Campaign data — 11,552 B

Four slots, each containing:
- two 36-byte descriptors;
- 32 chunks of 88 bytes.

Begin:
- one 36-byte descriptor + commit.

Append:
- each up-to-64-byte payload becomes one 88-byte chunk write;
- chunk write itself does not call commit.

Checkpoint:
- commit pending chunk writes;
- write one 36-byte descriptor;
- commit descriptor.

Finish:
- same checkpoint publication sequence with FINISHED state.

Profile:
- highest burst-write region in the current campaign model;
- up to 32 chunk records per slot/campaign-data allocation;
- durability is explicitly checkpoint-based;
- physical media must support multiple candidate writes followed by a durability barrier without falsely reporting them durable earlier.

### B5 bounded command journal V3 — 35,840 B

Record size: 70 B.
256 logical slots, two copies per slot.

Every durable journal mutation:
- writes one 70-byte record to the inactive/opposite copy;
- calls commit immediately.

Mutations include:
- admission of a new transaction;
- recovery-context mutation where applicable;
- RESERVED -> STARTED;
- terminal completion;
- readmission of an evicted COMPLETED slot.

Profile:
- potentially high lifetime churn;
- several durable writes may occur for one business command;
- distribution is across 256 logical slots, but a physical erase-page implementation can still concentrate wear if logical updates force page erase/rewrite;
- this region is the dominant capacity consumer and a major endurance driver.

### Diagnostic history — 20 B

Mutation:
- overwrite same 20-byte logical location;
- one commit.

Trigger:
- durable last-fault update.

Profile:
- event-driven but potentially bursty under repeated faults;
- single logical record at store level.

### Self-test history — 20 B

Mutation:
- overwrite same 20-byte logical location;
- one commit.

Trigger:
- durable self-test facts update.

Profile:
- expected low/moderate churn depending on self-test policy;
- single logical record at store level.

### Boot intent — 16 B

Commit:
- overwrite same 16-byte logical location with encoded intent;
- one commit.

Clear:
- overwrite same 16-byte location with zeros;
- one commit.

Profile:
- each relevant reset/reboot workflow can cause at least a set/clear pair;
- small record but potentially important endurance hotspot;
- direct zero overwrite is incompatible with raw NOR-flash programming semantics unless the physical backend provides erase/translation.

## Churn classification

Approximate qualitative ranking from current code semantics:

1. Campaign data chunks/checkpoints — burstiest write traffic.
2. B5 command journal — repeated transactional durable mutations.
3. Boot intent / diagnostic history / time history — small fixed-address hotspot records whose risk depends strongly on event frequency.
4. Campaign repository — campaign lifecycle traffic.
5. Configuration — comparatively infrequent.

This is a semantic ranking, not a measured operational write-rate. Exact service-life calculations require workload assumptions that are not frozen in current main.

## Consequences for raw STM32 Flash

The current logical API permits arbitrary byte-range overwrites such as 16, 18, 20, 36, 70, 88, 140 and 252 bytes at byte-packed offsets.

A physical backend implemented as direct raw-Flash writes cannot be assumed correct merely because total capacity is sufficient.

The backend must address at least:

- erase-before-rewrite constraints;
- physical program granularity/alignment;
- preservation of unrelated logical records sharing an erase page;
- transactional commit semantics;
- power loss during page reconstruction;
- wear concentration at fixed logical addresses;
- candidate writes that must not become logically committed before commit();
- safe recovery after ambiguous commit.

## Architectural implication

The existing portable stores already contain record-level CRCs, A/B copies, generations, and recovery rules. These mechanisms should not be confused with physical Flash translation.

A physical PersistentMedia backend may still require its own lower-level transactional/wear-leveling scheme so that the portable stores continue to observe the simple byte-addressable read/write/commit contract they were designed against.

Changing the portable stores to match raw Flash geometry would be a larger architectural change and is not justified until H3d1 compares physical strategies.

## H3d1-A conclusion

Capacity is no longer the deciding criterion.

The selected physical persistence strategy must be qualified primarily against:

- transactional commit semantics;
- power-loss atomicity;
- fixed-address overwrite behavior;
- erase/program granularity;
- endurance and wear distribution;
- checkpointed multi-write campaign behavior.

H3d1-B should now compare concrete physical strategies against this mutation profile before any backend is implemented.
