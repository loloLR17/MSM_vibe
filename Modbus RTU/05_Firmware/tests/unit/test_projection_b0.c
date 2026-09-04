#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "tr2/domain/identity/identity.h"
#include "tr2/modbus/projection.h"

int main(void)
{
    IdentitySnapshot source = { 0 };
    IdentitySnapshot copy = { 0 };
    IdentityService service = { 0 };
    ModbusBlock0Image image = { { 0u }, 0u };
    const char serial[TR2_IDENTITY_SERIAL_LENGTH] = {
        'T','R','2','T','E','S','T','0','0','0','0','0','0','0','0','1'
    };
    const char manufacturer[TR2_IDENTITY_MANUFACTURER_LENGTH] = {
        'M','S','M','T','E','S','T','\0'
    };

    source.generation = UINT32_C(42);
    source.device_id = UINT32_C(0x12345678);
    source.hardware_version = UINT16_C(0x0102);
    source.firmware_version_major = 1u;
    source.firmware_version_minor = 2u;
    source.firmware_version_patch = 3u;
    source.protocol_version = UINT16_C(1);
    source.device_capabilities = UINT16_C(0xFFFF);
    memcpy(source.serial_number, serial, sizeof(serial));
    memcpy(source.manufacturer, manufacturer, sizeof(manufacturer));

    assert(identity_service_get_snapshot(&service, &copy) == TR2_ERROR_INVALID_STATE);
    assert(identity_service_init(&service, &source) == TR2_OK);
    assert(identity_service_get_snapshot(&service, &copy) == TR2_OK);
    assert(memcmp(&source, &copy, sizeof(source)) == 0);

    assert(modbus_project_b0(&copy, &image) == TR2_OK);
    assert(image.source_generation == UINT32_C(42));
    assert(image.registers[0] == UINT16_C(0x1234));
    assert(image.registers[1] == UINT16_C(0x5678));
    assert(image.registers[2] == UINT16_C(0x0102));
    assert(image.registers[3] == 1u);
    assert(image.registers[4] == 2u);
    assert(image.registers[5] == 3u);
    assert(image.registers[6] == 1u);
    assert(image.registers[7] == UINT16_C(0x000F));

    assert(image.registers[8] == UINT16_C(0x5452));
    assert(image.registers[9] == UINT16_C(0x3254));
    assert(image.registers[10] == UINT16_C(0x4553));
    assert(image.registers[11] == UINT16_C(0x5430));
    assert(image.registers[12] == UINT16_C(0x3030));
    assert(image.registers[13] == UINT16_C(0x3030));
    assert(image.registers[14] == UINT16_C(0x3030));
    assert(image.registers[15] == UINT16_C(0x3031));

    assert(image.registers[16] == UINT16_C(0x4D53));
    assert(image.registers[17] == UINT16_C(0x4D54));
    assert(image.registers[18] == UINT16_C(0x4553));
    assert(image.registers[19] == UINT16_C(0x5400));
    assert(image.registers[20] == 0u);

    copy.serial_number[0] = (char)0x80;
    assert(modbus_project_b0(&copy, &image) == TR2_ERROR_INVALID_ARGUMENT);
    assert(image.source_generation == UINT32_C(42));
    assert(image.registers[0] == UINT16_C(0x1234));

    assert(modbus_project_b0(NULL, &image) == TR2_ERROR_INVALID_ARGUMENT);
    assert(modbus_project_b0(&source, NULL) == TR2_ERROR_INVALID_ARGUMENT);

    return 0;
}
