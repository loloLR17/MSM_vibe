#ifndef TR2_LIVE_COMMAND_STORE_H
#define TR2_LIVE_COMMAND_STORE_H
#include "tr2/application/live_configuration.h"
#include "tr2/domain/command/command.h"
#define TR2_LIVE_JOURNAL_SLOTS 128u
#define TR2_LIVE_JOURNAL_OFFSET 15922u

typedef struct { uint32_t epoch; CommandRequest command; } LiveCommandRequest;
typedef struct {
    uint64_t generation;
    LiveCommandRequest request;
    uint16_t phase, status, result, detail;
    CommandTerminalTimestamp timestamp;
    uint64_t run_id;
    uint32_t config_id, config_generation;
    uint16_t effect_state;
} LiveCommandRecord;
typedef struct {
    uint64_t generation;
    uint32_t current, previous;
    uint16_t renew_txid, flags;
    uint64_t transition;
} LiveEpochRecord;
typedef struct {
    uint64_t generation;
    uint32_t epoch;
    uint16_t txid, code;
    uint64_t run_id;
    uint32_t config_id, config_generation;
    uint16_t phase;
} LiveEffectRecord;
typedef struct {
    LiveConfiguration *configuration;
    LiveCommandRecord entries[TR2_LIVE_JOURNAL_SLOTS];
    bool present[TR2_LIVE_JOURNAL_SLOTS];
    uint8_t copy[TR2_LIVE_JOURNAL_SLOTS];
    LiveCommandRecord terminal;
    bool has_terminal;
    LiveEpochRecord epoch;
    LiveEffectRecord effect;
    uint8_t epoch_copy, effect_copy;
    uint64_t generation;
    bool has_effect, mounted;
} LiveCommandStore;

Tr2Result live_command_record_encode(const LiveCommandRecord *r,uint8_t bytes[112]);
Tr2Result live_command_record_decode(const uint8_t bytes[112],LiveCommandRecord *r);
Tr2Result live_command_store_init(LiveCommandStore *store,LiveConfiguration *configuration);
Tr2Result live_command_store_recover(LiveCommandStore *store);
/* One outer transaction; all pointers borrowed synchronously. No internal retry. */
Tr2Result live_command_store_commit(LiveCommandStore *store,unsigned slot,
    const LiveCommandRecord *record,const LiveEpochRecord *epoch,const LiveEffectRecord *effect);
int live_command_store_lookup(const LiveCommandStore *store,uint32_t epoch,uint16_t txid);
int live_command_store_allocate(const LiveCommandStore *store,uint16_t code);
#endif
