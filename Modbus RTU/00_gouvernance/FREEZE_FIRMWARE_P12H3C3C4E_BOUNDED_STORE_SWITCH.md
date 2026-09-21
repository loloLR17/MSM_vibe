# FREEZE — Firmware P12-H3c3-C4-E — Atomic bounded journal store switch

## Status

Frozen after local validation reported by the operator:

- 85/85 tests green
- HOST VALIDATED
- CROSS-BUILD VALIDATED
- HARDWARE PENDING

This freeze closes C4-E: the active SystemRuntime command journal composition has been switched atomically from the legacy dense V2 store to the bounded V3 store.

## E1 — Boot recovery decoupling

CommandBootRecovery now depends on the portable CommandJournal contract, not on CommandJournalStore V2.

The caller is responsible for recovering/qualifying the concrete store before passing its CommandJournal interface.

This allows the same boot-recovery logic to operate independently of V2/V3 persistence geometry.

## E2 — Active SystemRuntime switch

SystemRuntime now owns:

- CommandJournalBoundedStore
- CommandJournalBoundedRecoveryResult
- a command PersistentMediaRegion sized by TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE

The active command journal persistent geometry is:

- 256 logical slots
- 2 redundant copies per slot
- 70 bytes per V3 record
- total: 35,840 bytes

SystemRuntime recovery uses command_journal_bounded_store_init(), command_journal_bounded_store_recover(), and command_journal_bounded_store_journal().

Diagnostic-history and boot-intent offsets are consequently computed after the bounded V3 command-journal region rather than after the legacy dense V2 region.

## E3 — Positive runtime proof

Integration test:

    tests/integration/test_p12h3c3c4e3_runtime_bounded_journal.c

proves that the active SystemRuntime:

- exposes a command journal media region of exactly TR2_COMMAND_JOURNAL_BOUNDED_STORAGE_SIZE;
- binds CommandEngine to the CommandJournal exported by CommandJournalBoundedStore;
- persists transaction 65000 into a bounded V3 slot independently of transaction-ID physical addressing;
- recovers that RESERVED transaction after reboot;
- reconstructs next_admission_order;
- restores boot-recovery evidence without replay;
- classifies same-ID/same-identity as RETRY;
- classifies same-ID/different-identity as COLLISION.

The existing power-loss boot-recovery integration tests also remain green after the switch.

## E4 — Active-composition review

Review of the current main composition confirms:

- include/tr2/application/system_runtime.h references CommandJournalBoundedStore, not CommandJournalStore V2;
- src/application/system_runtime.c sizes and recovers the active command region exclusively through bounded V3 symbols;
- CommandEngine has no V2/V3 concrete-store dependency;
- CommandBootRecovery has no V2/V3 concrete-store dependency;
- host platform composition has no V2 concrete-store dependency;
- inspected acquisition-command, B5/PDU, host-main and STM32 composition paths contain no active CommandJournalStore V2 reference.

Legacy V2 source and unit tests remain in the repository intentionally. Their presence is not an active V2/V3 hybrid: they are retained historical/test code and are no longer SystemRuntime persistence authority.

## Persistent-media compatibility boundary

C4-E changes the persistent layout: the command journal shrinks from the legacy dense V2 region to the 35,840-byte bounded V3 region, and all following regions move accordingly.

No V2-to-V3 on-media migration is defined by C4-E. Existing media formatted with the legacy V2 SystemRuntime layout must not be silently reinterpreted as the V3 layout.

Before deployment on persistent production hardware, the project must define and enforce an explicit persistent-layout version / clean-format or migration policy. Hardware persistence remains outside this freeze.

## Invariants preserved

- B0-B7 Modbus mapping unchanged.
- B5 transactional semantics unchanged.
- transaction_id remains uint16, 0 invalid, 1..65535 valid.
- retained same-ID/same-request is retry without re-execution.
- retained same-ID/different-request is collision/refusal.
- no automatic B5 retry.
- at most one non-terminal transaction after valid recovery.
- bounded retention is 256 most recent distinct transactions subject to the frozen eviction rules.
- no direct business bypass was introduced.

## Validation boundary

Validated:

- host tests: 85/85
- host build
- STM32 cross-build

Not validated:

- physical STM32 execution
- real non-volatile persistent medium
- power-loss behavior on final physical medium
- physical RS-485 path

Therefore HARDWARE PENDING remains mandatory.

## C4-E conclusion

The active firmware runtime now has one command-journal persistence authority: bounded V3.

There is no active V2/V3 hybrid in SystemRuntime composition.

Legacy V2 code may remain until a later cleanup tranche, but must not be reintroduced into active runtime composition.
