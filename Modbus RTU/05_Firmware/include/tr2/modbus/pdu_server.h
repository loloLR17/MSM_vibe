#ifndef TR2_MODBUS_PDU_SERVER_H
#define TR2_MODBUS_PDU_SERVER_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/application/campaign_inventory_service.h"
#include "tr2/application/command_request_mailbox.h"
#include "tr2/domain/configuration/configuration_staging.h"
#include "tr2/domain/time/time_service.h"
#include "tr2/modbus/read_adapter.h"

#define MODBUS_PDU_FC_READ_HOLDING_REGISTERS UINT8_C(0x03)
#define MODBUS_PDU_FC_WRITE_MULTIPLE_REGISTERS UINT8_C(0x10)
#define MODBUS_PDU_EXCEPTION_ILLEGAL_FUNCTION UINT8_C(0x01)
#define MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_ADDRESS UINT8_C(0x02)
#define MODBUS_PDU_EXCEPTION_ILLEGAL_DATA_VALUE UINT8_C(0x03)
#define MODBUS_PDU_EXCEPTION_SLAVE_DEVICE_FAILURE UINT8_C(0x04)
#define MODBUS_PDU_MAX_SIZE ((size_t)253u)

typedef struct {
    ModbusReadSources read_sources;
    TimeService *time_service;
    ConfigurationStagingService *configuration_staging;
    CommandRequestMailbox *command_mailbox;
    CampaignInventoryService *campaign_inventory;
    ModbusBlock6Image *b6_image;
} ModbusPduServerContext;

typedef struct {
    Tr2Result operation_result;
    size_t response_length;
} ModbusPduServerOutcome;

ModbusPduServerOutcome modbus_pdu_server_process(const ModbusPduServerContext *context,
                                                 const uint8_t *request_pdu,
                                                 size_t request_length,
                                                 uint8_t *response_pdu,
                                                 size_t response_capacity);

#endif
