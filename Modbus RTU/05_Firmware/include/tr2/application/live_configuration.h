#ifndef TR2_APPLICATION_LIVE_CONFIGURATION_H
#define TR2_APPLICATION_LIVE_CONFIGURATION_H
#include "tr2/application/persistent_identity.h"
#include "tr2/application/configuration_workflow.h"
#include "tr2/persistence/transactional_image_media.h"

#define TR2_LIVE_EPOCH_OFFSET 51914u
#define TR2_LIVE_EFFECT_OFFSET 52042u
#define TR2_LIVE_PROVENANCE_OFFSET 52170u
/* R1 configuration authority only. No command dispatch, sensor or SD ports.
 * This is not a selectable production protocol2 server. R2 must supply the
 * genuine epoch/command engine before protocol2 can be exposed on a wire. */
typedef struct {
    TransactionalImageMedia *historical;
    TransactionalImageMedia *live;
    const PersistentIdentityAssignment *assignments;
    size_t assignment_count;
    uint32_t uid[3];
    uint8_t historical_sha[32];
    uint8_t historical_slots[280];
    uint64_t historical_generation;
    PersistentIdentityRecord identity;
    ActiveConfigurationSnapshot active;
    uint32_t last_generation;
    uint32_t last_revision;
    unsigned target_slot;
    uint16_t state;
    uint16_t error;
    void *control_context;
    Tr2Result (*validate_control)(void *,const uint8_t *,size_t);
    bool initialized;
    bool mounted;
    bool has_active;
} LiveConfiguration;

Tr2Result live_configuration_init(LiveConfiguration *service,
    TransactionalImageMedia *historical, TransactionalImageMedia *live,
    const uint32_t uid[3], const PersistentIdentityAssignment *assignments,
    size_t assignment_count);
/* Explicit successor engine reader; bind once before mounting, no permissive
 * default. The validator must check the complete control namespace. */
Tr2Result live_configuration_bind_control(LiveConfiguration *service,void *context,
    Tr2Result (*validate)(void *,const uint8_t *,size_t));
/* Read-only, deterministic, revokes authority on any failure. */
Tr2Result live_configuration_recover(LiveConfiguration *service);
/* Explicit off-boot creation, unknown ownership or nonempty roots are refused.
 * Original historical bytes never change; journal/BootIntent are NOT replayed. */
Tr2Result live_configuration_migrate(LiveConfiguration *service);
/* In-process configuration application for R1 tests/integration. No B5 engine
 * or simulated B5 success is provided. Persistence precedes publication. */
Tr2Result live_configuration_apply(LiveConfiguration *service,
    const PreparedConfiguration *prepared, bool acquisition_running);
bool live_configuration_active(const LiveConfiguration *service,
    ActiveConfigurationSnapshot *snapshot);
Tr2Result live_configuration_project_b4(const LiveConfiguration *service,
    const PreparedConfiguration *prepared, ModbusBlock4Image *image);
/* Audit descriptor only: cannot be used to claim an operational LIVE server. */
Tr2Result live_configuration_identity(const LiveConfiguration *service,
    PersistentIdentityRecord *identity);
#endif
