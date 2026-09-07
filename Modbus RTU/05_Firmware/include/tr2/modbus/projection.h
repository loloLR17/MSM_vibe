#ifndef TR2_MODBUS_PROJECTION_H
#define TR2_MODBUS_PROJECTION_H

#include <stdint.h>

#include "tr2/application/campaign_inventory_service.h"
#include "tr2/application/command_request_mailbox.h"
#include "tr2/common/result.h"
#include "tr2/domain/command/command.h"
#include "tr2/domain/configuration/configuration.h"
#include "tr2/domain/identity/identity.h"
#include "tr2/domain/supervision/supervision.h"
#include "tr2/domain/system_state/system_state.h"
#include "tr2/domain/time/time_service.h"

#define TR2_B0_REGISTER_COUNT 21u
#define TR2_B1_REGISTER_COUNT 20u
#define TR2_B2_REGISTER_COUNT 16u
#define TR2_B3_REGISTER_COUNT 48u
#define TR2_B4_REGISTER_COUNT 176u
#define TR2_B5_REGISTER_COUNT 20u
#define TR2_B6_REGISTER_COUNT 64u

typedef struct {
    uint16_t registers[TR2_B0_REGISTER_COUNT];
    uint32_t source_generation;
} ModbusBlock0Image;

typedef struct {
    uint16_t registers[TR2_B1_REGISTER_COUNT];
    uint32_t source_generation;
} ModbusBlock1Image;

typedef struct {
    const SystemStateSnapshot *system_state;
    const TimeSnapshot *time;
} ModbusBlock1ProjectionSource;

typedef struct {
    uint16_t registers[TR2_B2_REGISTER_COUNT];
    uint32_t source_generation;
} ModbusBlock2Image;

typedef struct {
    uint16_t registers[TR2_B3_REGISTER_COUNT];
    uint32_t source_calculation_sequence;
} ModbusBlock3Image;

typedef struct {
    uint16_t config_structure_version;
    uint16_t config_capabilities_mask;
    uint16_t config_state;
    uint16_t config_error_code;
    const PreparedConfiguration *prepared;
    const ActiveConfigurationSnapshot *active;
} ModbusBlock4ProjectionSource;

typedef struct {
    uint16_t registers[TR2_B4_REGISTER_COUNT];
} ModbusBlock4Image;

typedef struct {
    const CommandRequestMailbox *mailbox;
    const CommandSnapshot *snapshot;
} ModbusBlock5ProjectionSource;

typedef struct {
    uint16_t registers[TR2_B5_REGISTER_COUNT];
    uint32_t source_generation;
} ModbusBlock5Image;

typedef struct {
    const CampaignInventoryViewSnapshot *inventory_snapshot;
    uint16_t inventory_structure_version;
    uint32_t storage_used_mb;
    uint32_t storage_free_mb;
    uint16_t storage_health_status;
} ModbusBlock6ProjectionSource;

typedef struct {
    uint16_t registers[TR2_B6_REGISTER_COUNT];
    uint32_t source_generation;
} ModbusBlock6Image;

Tr2Result modbus_project_b0(const IdentitySnapshot *snapshot, ModbusBlock0Image *output);
Tr2Result modbus_project_b1(const ModbusBlock1ProjectionSource *source,
                            ModbusBlock1Image *output);
Tr2Result modbus_project_b2(const TimeSnapshot *snapshot, ModbusBlock2Image *output);
Tr2Result modbus_project_b3(const SupervisionSnapshot *snapshot, ModbusBlock3Image *output);
Tr2Result modbus_project_b4(const ModbusBlock4ProjectionSource *source,
                            ModbusBlock4Image *output);
Tr2Result modbus_project_b5(const ModbusBlock5ProjectionSource *source,
                            ModbusBlock5Image *output);
Tr2Result modbus_project_b6(const ModbusBlock6ProjectionSource *source,
                            ModbusBlock6Image *output);

#endif
