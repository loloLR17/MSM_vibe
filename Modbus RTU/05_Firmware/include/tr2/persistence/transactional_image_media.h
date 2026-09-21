#ifndef TR2_PERSISTENCE_TRANSACTIONAL_IMAGE_MEDIA_H
#define TR2_PERSISTENCE_TRANSACTIONAL_IMAGE_MEDIA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/platform/persistent_media.h"

#define TR2_TRANSACTIONAL_MEDIA_PHYSICAL_SIZE ((size_t)262144u)
#define TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE ((size_t)51818u)
#define TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_SIZE ((size_t)64u)
#define TR2_TRANSACTIONAL_MEDIA_IMAGE_HEADER_SIZE ((size_t)64u)

#define TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_A_BASE UINT32_C(0x00000)
#define TR2_TRANSACTIONAL_MEDIA_SUPERBLOCK_B_BASE UINT32_C(0x01000)
#define TR2_TRANSACTIONAL_MEDIA_IMAGE_A_BASE UINT32_C(0x02000)
#define TR2_TRANSACTIONAL_MEDIA_IMAGE_B_BASE UINT32_C(0x10000)
#define TR2_TRANSACTIONAL_MEDIA_IMAGE_AREA_SIZE UINT32_C(0x0E000)

typedef struct {
    size_t physical_size;
    uint32_t superblock_a_base;
    uint32_t superblock_b_base;
    uint32_t image_a_base;
    uint32_t image_b_base;
    uint32_t image_area_size;
} TransactionalImageGeometry;

Tr2Result transactional_image_geometry_validate(
    const TransactionalImageGeometry *geometry);

TransactionalImageGeometry transactional_image_geometry_qualification_profile(void);

typedef enum {
    TRANSACTIONAL_IMAGE_RECOVERY_EMPTY = 0,
    TRANSACTIONAL_IMAGE_RECOVERY_VALID,
    TRANSACTIONAL_IMAGE_RECOVERY_UNSUPPORTED,
    TRANSACTIONAL_IMAGE_RECOVERY_CORRUPTED,
    TRANSACTIONAL_IMAGE_RECOVERY_UNAVAILABLE
} TransactionalImageRecoveryStatus;

typedef struct {
    TransactionalImageRecoveryStatus status;
    uint64_t generation;
    uint8_t active_image;
} TransactionalImageRecoveryResult;

typedef struct {
    void *context;
    Tr2Result (*read)(void *context, uint32_t offset, void *buffer, size_t size);
    Tr2Result (*write)(void *context, uint32_t offset, const void *buffer, size_t size);
} TransactionalImagePhysicalStorage;

typedef struct {
    TransactionalImagePhysicalStorage physical;
    TransactionalImageGeometry geometry;
    uint8_t *candidate;
    uint64_t generation;
    uint8_t active_image;
    uint8_t active_superblock;
    bool initialized;
    bool recovered;
    bool recovery_required;
    PersistentMedia interface;
} TransactionalImageMedia;

Tr2Result transactional_image_media_init(
    TransactionalImageMedia *media,
    const TransactionalImagePhysicalStorage *physical,
    const TransactionalImageGeometry *geometry,
    uint8_t *candidate,
    size_t candidate_size);

Tr2Result transactional_image_media_format_empty(TransactionalImageMedia *media);

Tr2Result transactional_image_media_recover(
    TransactionalImageMedia *media,
    TransactionalImageRecoveryResult *result);

PersistentMedia *transactional_image_media_interface(TransactionalImageMedia *media);

#endif
