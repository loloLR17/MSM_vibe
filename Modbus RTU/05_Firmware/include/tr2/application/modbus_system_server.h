#ifndef TR2_APPLICATION_MODBUS_SYSTEM_SERVER_H
#define TR2_APPLICATION_MODBUS_SYSTEM_SERVER_H

#include "tr2/application/system_runtime.h"
#include "tr2/modbus/rtu_server_runtime.h"

/* All binding objects are borrowed and must outlive the server. Runtime boot,
 * identity provisioning, address and half-duplex qualification belong to the
 * caller. An initialized server must not be copied or moved. Missing write authorities are exposed as exception 04.
 */
typedef struct {
    SystemRuntime *runtime;
    SerialTransport *transport;
    uint8_t unit_id;
    const ModbusBlock0Image *identity;
    ConfigurationWorkflow *configuration_workflow;
    void *command_submit_context;
    ModbusCommandSubmit command_submit;
    Tr2Result (*command_snapshot)(void *context, CommandSnapshot *snapshot);
} ModbusSystemServerBinding;

typedef struct {
    ModbusSystemServerBinding binding;
    ModbusRtuServerRuntime rtu;
    TimeSnapshot time;
    ModbusBlock1Image b1;
    ModbusBlock3Image b3;
    ModbusBlock4Image b4;
    ModbusBlock5Image b5;
    ModbusBlock7Image b7;
    bool initialized;
    bool started;
} ModbusSystemServer;

Tr2Result modbus_system_server_init(ModbusSystemServer *server,
                                    const ModbusSystemServerBinding *binding);
Tr2Result modbus_system_server_start(ModbusSystemServer *server);
Tr2Result modbus_system_server_poll_once(ModbusSystemServer *server);

#endif
