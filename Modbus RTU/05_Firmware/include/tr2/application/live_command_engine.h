#ifndef TR2_LIVE_COMMAND_ENGINE_H
#define TR2_LIVE_COMMAND_ENGINE_H
#include "tr2/persistence/live_command_store.h"

typedef enum { LIVE_BOOT=0,LIVE_NO_CONFIG,LIVE_READY,LIVE_STARTING,LIVE_RUNNING,LIVE_STOPPING,LIVE_FAULT } LiveRunState;
typedef struct {
    bool stopped, running, configuration_confirmed;
    uint64_t run_id;
    uint32_t config_id, config_generation;
} LiveAcquisitionObservation;
/* The observation must come from the real acquisition owner after its actual
 * readbacks/handshake. No default stub declares an effect. R3 supplies the board
 * implementation. safe_stop never proves absence of a historical effect. */
typedef struct {
    void *context;
    Tr2Result (*start)(void *,uint64_t,const ActiveConfigurationSnapshot *);
    Tr2Result (*stop)(void *);
    Tr2Result (*observe)(void *,LiveAcquisitionObservation *);
    bool (*io_admissible)(void *,uint16_t);
    CommandTerminalTimestamp (*terminal_time)(void *);
} LiveAcquisitionPort;
typedef struct {
    unsigned configuration; /* 0 none, 1 LIVE, 2 campaign, 3 invalid */
    unsigned acquisition;   /* 0 stopped, 1 LIVE, 2 campaign */
    bool blocking_fault,maintenance,journal_budget,nonterminal;
} LiveCommandContext;
typedef struct { uint16_t status,result; bool admit; } LiveCommandDecision;
typedef struct {
    LiveCommandStore *store;
    LiveAcquisitionPort acquisition;
    LiveRunState state;
    LiveCommandRecord active,last;
    PreparedConfiguration prepared,captured_prepared;
    uint16_t mailbox[8];
    uint32_t mailbox_epoch;
    uint16_t status,result,detail,epoch_status;
    bool has_active,has_last,indeterminate,booted,prepared_present;
    bool blocking_fault,maintenance,campaign_active;
} LiveCommandEngine;
LiveCommandDecision live_command_context_decide(uint16_t code,const LiveCommandContext *context);
Tr2Result live_command_engine_init(LiveCommandEngine *,LiveCommandStore *,const LiveAcquisitionPort *);
Tr2Result live_command_engine_boot(LiveCommandEngine *);
/* Calls safety stop independently of command admission/journal success. */
Tr2Result live_command_engine_fault(LiveCommandEngine *);
Tr2Result live_command_engine_submit(LiveCommandEngine *,const LiveCommandRequest *);
/* One durable phase per step. No internally repeated hardware effects. */
Tr2Result live_command_engine_step(LiveCommandEngine *);
Tr2Result live_command_engine_prepare(LiveCommandEngine *,const PreparedConfiguration *);
Tr2Result live_command_engine_project_b5(const LiveCommandEngine *,uint16_t image[29]);
/* Dedicated B5 V1.1 access path, not the historical 20-register model. FC10
 * writes are staged atomically before immutable capture/submit. */
Tr2Result live_command_engine_write_b5(LiveCommandEngine *,uint16_t address,const uint16_t *,uint16_t count);
Tr2Result live_command_engine_read_b5(const LiveCommandEngine *,uint16_t address,uint16_t *,uint16_t count);
#endif
