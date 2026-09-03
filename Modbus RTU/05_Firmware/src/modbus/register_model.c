#include "tr2/modbus/register_model.h"

bool modbus_register_descriptor_is_valid(const ModbusRegisterDescriptor *descriptor)
{
    if (descriptor == NULL) {
        return false;
    }

    return descriptor->access == MODBUS_REGISTER_RO ||
           descriptor->access == MODBUS_REGISTER_RW ||
           descriptor->access == MODBUS_REGISTER_RESERVED;
}

bool modbus_register_is_writable(const ModbusRegisterDescriptor *descriptor)
{
    return modbus_register_descriptor_is_valid(descriptor) &&
           descriptor->access == MODBUS_REGISTER_RW;
}
