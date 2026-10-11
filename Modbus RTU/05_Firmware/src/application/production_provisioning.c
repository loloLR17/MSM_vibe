#include "tr2/application/production_provisioning.h"
#include "tr2/modbus/rtu_server_runtime.h"
#include "tr2/persistence/persistent_storage_core.h"
#include <string.h>

Tr2Result production_provisioning_probe(TransactionalImageMedia *media,
    TransactionalImageRecoveryResult *recovery)
{
    Tr2Result status;
    if (recovery == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    memset(recovery, 0, sizeof(*recovery));
    recovery->status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
    if (media == NULL || !media->initialized) return TR2_ERROR_NOT_AVAILABLE;
    status = transactional_image_media_recover(media, recovery);
    if (status != TR2_OK) return status;
    switch (recovery->status) {
    case TRANSACTIONAL_IMAGE_RECOVERY_VALID: return TR2_OK;
    case TRANSACTIONAL_IMAGE_RECOVERY_EMPTY: return TR2_ERROR_NOT_AVAILABLE;
    case TRANSACTIONAL_IMAGE_RECOVERY_UNSUPPORTED: return TR2_ERROR_UNSUPPORTED;
    case TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED: return TR2_ERROR_CORRUPTED;
    default: return TR2_ERROR_UNAVAILABLE;
    }
}

static Tr2Result identity_validate(const ProductionProvisioningRequest *request,
    const ProductionProvisioningIdentity *identity, ModbusBlock0Image *b0)
{
    if (memcmp(request->target_uid, identity->hardware_uid, sizeof(request->target_uid)) != 0)
        return TR2_ERROR_INVALID_STATE;
    if (identity->functional.protocol_version != request->supported_protocol_version)
        return TR2_ERROR_UNSUPPORTED;
    if (!modbus_rtu_server_runtime_unit_id_is_valid(identity->unit_id) ||
        (identity->functional.device_capabilities & UINT16_C(0xFFF0)) != 0u)
        return TR2_ERROR_INVALID_ARGUMENT;
    return modbus_project_b0(&identity->functional, b0);
}

Tr2Result production_provisioning_plan(TransactionalImageMedia *media,
    const ProductionProvisioningRequest *request, ProductionProvisioningResult *result)
{
    ProductionProvisioningIdentity current = {0}, desired = {0};
    ModbusBlock0Image old_b0, new_b0;
    Tr2Result status;
    if (result == NULL) return TR2_ERROR_INVALID_ARGUMENT;
    memset(result, 0, sizeof(*result));
    result->recovery.status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
    if (request == NULL || request->image == NULL ||
        request->image_size != TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE)
        return TR2_ERROR_INVALID_ARGUMENT;
    if (!request->exclusive_ownership || request->inspect_image == NULL)
        return TR2_ERROR_NOT_AVAILABLE;
    if (media != NULL && media->initialized) {
        uintptr_t image = (uintptr_t)request->image;
        uintptr_t candidate = (uintptr_t)media->candidate;
        size_t size = TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE;
        if ((image >= candidate && image-candidate < size) ||
            (candidate >= image && candidate-image < request->image_size))
            return TR2_ERROR_INVALID_ARGUMENT;
    }
    status = production_provisioning_probe(media, &result->recovery);
    if (status != TR2_OK) return status;
    if (result->recovery.generation != request->expected_generation)
        return TR2_ERROR_INVALID_STATE;
    /* Do not allow a borrowed candidate to masquerade as an immutable request:
     * recovery overwrites it, and write validation must cover independent bytes. */
    status = request->inspect_image(request->inspect_context, media->candidate,
                                    TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE, &current);
    if (status != TR2_OK) return status;
    status = identity_validate(request, &current, &old_b0);
    if (status != TR2_OK) return status;
    status = request->inspect_image(request->inspect_context, request->image,
                                    request->image_size, &desired);
    if (status != TR2_OK) return status;
    status = identity_validate(request, &desired, &new_b0);
    if (status != TR2_OK) return status;
    /* Existing provisioning identity/address is immutable in this operation.
     * A future migration requires its own contract, not a silent replacement. */
    if (current.unit_id != desired.unit_id ||
        current.functional.generation != desired.functional.generation ||
        memcmp(old_b0.registers, new_b0.registers, sizeof(old_b0.registers)) != 0)
        return TR2_ERROR_INVALID_STATE;
    result->identity = old_b0;
    result->unit_id = current.unit_id;
    result->identity_available = true;
    result->unchanged = memcmp(media->candidate, request->image, request->image_size) == 0;
    return TR2_OK;
}

Tr2Result production_provisioning_apply(TransactionalImageMedia *media,
    const ProductionProvisioningRequest *request, bool write_authorized,
    ProductionProvisioningResult *result)
{
    PersistentStorageCore storage = {0};
    Tr2Result status;
    if (!write_authorized) {
        if (result != NULL) {
            memset(result, 0, sizeof(*result));
            result->recovery.status = TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE;
        }
        return TR2_ERROR_NOT_AVAILABLE;
    }
    status = production_provisioning_plan(media, request, result);
    if (status != TR2_OK || result->unchanged) return status;
    status = persistent_storage_core_init(&storage, transactional_image_media_interface(media));
    if (status != TR2_OK) return status;
    result->write_attempted = true;
    result->identity_available = false;
    status = persistent_storage_core_write(&storage, 0u, request->image, request->image_size);
    if (status == TR2_OK) status = persistent_storage_core_commit(&storage);
    if (status != TR2_OK) return status;
    status = production_provisioning_probe(media, &result->recovery);
    if (status != TR2_OK) return status;
    if (memcmp(media->candidate, request->image, request->image_size) != 0)
        return TR2_ERROR_STORAGE;
    result->identity_available = true;
    return TR2_OK;
}
