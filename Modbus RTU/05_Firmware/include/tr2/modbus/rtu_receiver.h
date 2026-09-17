#ifndef TR2_MODBUS_RTU_RECEIVER_H
#define TR2_MODBUS_RTU_RECEIVER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/modbus/codec.h"

typedef enum {
    MODBUS_RTU_RECEIVER_IDLE = 0,
    MODBUS_RTU_RECEIVER_RECEIVING,
    MODBUS_RTU_RECEIVER_WAITING_T3_5,
    MODBUS_RTU_RECEIVER_INVALID
} ModbusRtuReceiverState;

typedef struct {
    ModbusRtuReceiverState state;
    uint8_t buffer[MODBUS_RTU_ADU_MAX_SIZE];
    size_t length;
} ModbusRtuReceiver;

typedef struct {
    bool frame_available;
    const uint8_t *frame;
    size_t frame_length;
} ModbusRtuReceiverEvent;

void modbus_rtu_receiver_init(ModbusRtuReceiver *receiver);
ModbusRtuReceiverEvent modbus_rtu_receiver_push_byte(ModbusRtuReceiver *receiver,
                                                     uint8_t byte);
ModbusRtuReceiverEvent modbus_rtu_receiver_on_silence_t1_5(ModbusRtuReceiver *receiver);
ModbusRtuReceiverEvent modbus_rtu_receiver_on_silence_t3_5(ModbusRtuReceiver *receiver);

#endif
