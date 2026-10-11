#ifndef TR2_APPLICATION_PERSISTENT_IDENTITY_H
#define TR2_APPLICATION_PERSISTENT_IDENTITY_H
#include "tr2/application/production_provisioning.h"
#define TR2_PERSISTENT_IDENTITY_OFFSET ((size_t)51818u)
#define TR2_PERSISTENT_IDENTITY_SIZE ((size_t)96u)
typedef struct {
    uint64_t generation;
    ProductionProvisioningIdentity identity;
} PersistentIdentityRecord;
Tr2Result persistent_identity_encode(const PersistentIdentityRecord *record,
    uint8_t bytes[TR2_PERSISTENT_IDENTITY_SIZE]);
Tr2Result persistent_identity_decode(const uint8_t *bytes, size_t size,
    PersistentIdentityRecord *record);
/* Validates the appended identity; historical payload is validated separately
 * by existing services / the explicit prefix authority below. */
Tr2Result persistent_identity_from_image(const uint8_t *image, size_t size,
    const uint32_t uid[3], uint16_t protocol, PersistentIdentityRecord *record,
    ModbusBlock0Image *b0);
/* Entries are supplied by a reviewed project attribution registry. This checks
 * local consistency only; it does not allocate IDs or prove fleet authority. */
typedef struct {
    uint32_t hardware_uid[3];
    uint32_t device_id;
} PersistentIdentityAssignment;
Tr2Result persistent_identity_attribution_check(
    const PersistentIdentityAssignment *entries, size_t count,
    const uint32_t uid[3], uint32_t device_id);
typedef struct {
    void *context;
    Tr2Result (*attribution)(void *context, const uint32_t uid[3], uint32_t device_id);
    Tr2Result (*validate_prefix)(void *context, const uint8_t *prefix, size_t size);
    uint32_t target_uid[3];
    uint16_t supported_protocol_version;
    uint64_t expected_generation;
    bool exclusive_ownership;
    const uint8_t *image;
    size_t image_size;
} PersistentIdentityProvisioningRequest;
/* Explicit, off-boot only; no default registry or prefix authority. */
Tr2Result persistent_identity_provision(TransactionalImageMedia *media,
    const PersistentIdentityProvisioningRequest *request, bool write_authorized,
    ProductionProvisioningResult *result);
#endif
