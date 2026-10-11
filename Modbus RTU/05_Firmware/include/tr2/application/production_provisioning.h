#ifndef TR2_APPLICATION_PRODUCTION_PROVISIONING_H
#define TR2_APPLICATION_PRODUCTION_PROVISIONING_H

#include "tr2/modbus/projection.h"
#include "tr2/persistence/transactional_image_media.h"

/* Internal full hardware identity, never truncated into a Modbus register. */
typedef struct {
    uint32_t hardware_uid[3];
    IdentitySnapshot functional;
    uint8_t unit_id;
} ProductionProvisioningIdentity;

/* Board/layout authority must validate the COMPLETE logical image and extract
 * its persisted identity. No parser/default allocation is supplied by this
 * engine: the current V1 image has no approved identity reservation. */
typedef Tr2Result (*ProductionProvisioningImageInspect)(void *context,
    const uint8_t *image, size_t size, ProductionProvisioningIdentity *identity);

typedef struct {
    uint32_t target_uid[3];
    uint16_t supported_protocol_version;
    uint64_t expected_generation;
    bool exclusive_ownership;
    const uint8_t *image;
    size_t image_size;
    void *inspect_context;
    ProductionProvisioningImageInspect inspect_image;
} ProductionProvisioningRequest;

typedef struct {
    TransactionalImageRecoveryResult recovery;
    bool unchanged;
    bool identity_available;
    bool write_attempted;
    ModbusBlock0Image identity;
    uint8_t unit_id;
} ProductionProvisioningResult;

/* Read-only boot gate. VALID means recovered media, not READY or B1 available.
 * An inaccessible/absent interface is UNAVAILABLE; no hardware presence inferred. */
Tr2Result production_provisioning_probe(TransactionalImageMedia *media,
    TransactionalImageRecoveryResult *recovery);

/* Planning never writes. EMPTY/unsupported/corrupt/unknown ownership/parser fail
 * closed. Creation and identity migration need a separate approved procedure.
 * Requests/buffers must be immutable and media exclusively owned throughout. */
Tr2Result production_provisioning_plan(TransactionalImageMedia *media,
    const ProductionProvisioningRequest *request, ProductionProvisioningResult *result);

/* Only explicit provisioning calls this, NEVER normal boot. Replans against the
 * current committed image/generation. One transaction, readback/recovery, no retry.
 * Physical authorization is external; this function is not an authorization. */
Tr2Result production_provisioning_apply(TransactionalImageMedia *media,
    const ProductionProvisioningRequest *request, bool write_authorized,
    ProductionProvisioningResult *result);

#endif
