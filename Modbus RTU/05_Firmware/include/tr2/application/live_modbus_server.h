#ifndef TR2_LIVE_MODBUS_SERVER_H
#define TR2_LIVE_MODBUS_SERVER_H
#include "tr2/application/live_command_engine.h"
#include "tr2/application/live_metrology.h"
#include "tr2/modbus/rtu_server_runtime.h"

#define TR2_LIVE_INGRESS_SLOTS 4u /* three normal captures, one STOP reserve */
typedef struct {
    ModbusBlock3Image b3;
    LiveMetrology metric; /* immutable RAM state; callbacks stripped at publication */
    ModbusBlock4Image b4;
    uint16_t b5[29];
    uint64_t run_id;
    uint32_t config_id, config_generation;
} LiveModbusImage;
typedef struct {
    uint16_t mailbox[8];
    uint32_t epoch;
    PreparedConfiguration prepared;
    uint64_t order;
    bool used, prepared_present;
} LiveModbusCapture;
/* All methods are called by one main-loop owner, never ISR/threads. During a
 * cooperative R2 checkpoint only poll_once/publish are allowed, NEVER owner_step.
 * No media, sensor, engine admission or metrology callbacks in PDU handling.
 * FC10 acknowledgement means RAM capture, NOT durable admission or success.
 * Unsupported blocks/effects remain unavailable; not a production LIVE server. */
typedef struct {
    LiveCommandEngine *engine;
    LiveMetrology *metrology;
    ModbusRtuServerRuntime rtu;
    ModbusBlock0Image identity;
    LiveModbusImage images[2];
    ConfigurationStagingService staging;
    uint16_t mailbox[8];
    uint32_t mailbox_epoch;
    LiveModbusCapture queue[TR2_LIVE_INGRESS_SLOTS];
    void *time_context;
    uint64_t (*now_ms)(void *); /* bounded observed main-loop clock, no media */
    uint64_t capture_order, value_run, value_ms, last_read_ms;
    bool value_seen, read_seen, time_fault;
    uint32_t captured, queue_full, owner_calls, transport_errors;
    unsigned published;
    bool image_ready, responding, owner_busy, step_due;
    bool read_only;
    bool unavailable_inventory;uint16_t selected_inventory_index;
} LiveModbusServer;
Tr2Result live_modbus_server_init(LiveModbusServer *, LiveCommandEngine *,
    LiveMetrology *, SerialTransport *, uint8_t unit_id, const ModbusBlock0Image *,
    uint64_t (*now_ms)(void *), void *time_context);
/* Must be refreshed by the owner after FIFO/invalidation and at cooperative
 * service boundaries. now_ms is observed monotone, not a fabricated sample time. */
Tr2Result live_modbus_server_publish(LiveModbusServer *, uint64_t now_ms);
Tr2Result live_modbus_server_poll_once(LiveModbusServer *);
/* Explicit R0-INV absent authority, not an empty/erased historical inventory.
 * No media access or schema change. Default remains unsupported (R4Q). */
Tr2Result live_modbus_server_enable_unavailable_inventory(LiveModbusServer *);
/* At most one capture/admission OR one durable R2 phase. Refuses reentrancy and
 * entry during an existing cooperative scope. STOP reserve cannot be starved by
 * ordinary ingress; it never preempts an already admitted durable operation. */
Tr2Result live_modbus_server_owner_step(LiveModbusServer *);
ModbusPduServerOutcome live_modbus_server_process_pdu(void *,const uint8_t *,size_t,
                                                     uint8_t *,size_t);
#endif
