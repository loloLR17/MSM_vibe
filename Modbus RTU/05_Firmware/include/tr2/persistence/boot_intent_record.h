#ifndef TR2_PERSISTENCE_BOOT_INTENT_RECORD_H
#define TR2_PERSISTENCE_BOOT_INTENT_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/boot/boot_intent.h"

#define TR2_BOOT_INTENT_RECORD_FORMAT_VERSION UINT16_C(1)
#define TR2_BOOT_INTENT_RECORD_SIZE 16u

Tr2Result tr2_boot_intent_record_encode(const BootIntent *intent,
                                        uint8_t *record,
                                        size_t record_size);
Tr2Result tr2_boot_intent_record_decode(const uint8_t *record,
                                        size_t record_size,
                                        BootIntent *intent);

#endif
