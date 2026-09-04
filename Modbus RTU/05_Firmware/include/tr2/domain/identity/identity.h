#ifndef TR2_DOMAIN_IDENTITY_H
#define TR2_DOMAIN_IDENTITY_H

#include <stdbool.h>
#include <stdint.h>

#include "tr2/common/result.h"

#define TR2_IDENTITY_SERIAL_LENGTH 16u
#define TR2_IDENTITY_MANUFACTURER_LENGTH 8u

typedef struct {
    uint32_t generation;
    uint32_t device_id;
    uint16_t hardware_version;
    uint16_t firmware_version_major;
    uint16_t firmware_version_minor;
    uint16_t firmware_version_patch;
    uint16_t protocol_version;
    uint16_t device_capabilities;
    char serial_number[TR2_IDENTITY_SERIAL_LENGTH];
    char manufacturer[TR2_IDENTITY_MANUFACTURER_LENGTH];
} IdentitySnapshot;

typedef struct {
    IdentitySnapshot snapshot;
    bool initialized;
} IdentityService;

Tr2Result identity_service_init(IdentityService *service, const IdentitySnapshot *snapshot);
Tr2Result identity_service_get_snapshot(const IdentityService *service, IdentitySnapshot *snapshot);

#endif
