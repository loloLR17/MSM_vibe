#ifndef TR2_PERSISTENCE_CONFIGURATION_RECORD_H
#define TR2_PERSISTENCE_CONFIGURATION_RECORD_H

#include <stddef.h>
#include <stdint.h>

#include "tr2/common/result.h"
#include "tr2/domain/configuration/configuration.h"

#define TR2_CONFIGURATION_RECORD_FORMAT_VERSION UINT16_C(1)
#define TR2_CONFIGURATION_RECORD_SIZE 140u

Tr2Result tr2_configuration_record_encode(const ActiveConfigurationSnapshot *snapshot,
                                          uint8_t *record,
                                          size_t record_size);

Tr2Result tr2_configuration_record_decode(const uint8_t *record,
                                          size_t record_size,
                                          ActiveConfigurationSnapshot *snapshot);

#endif
