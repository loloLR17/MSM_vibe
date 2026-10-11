#ifndef TR2_APPLICATION_PRODUCTION_COMMISSIONING_H
#define TR2_APPLICATION_PRODUCTION_COMMISSIONING_H
#include "tr2/application/persistent_identity.h"
#include "tr2/domain/configuration/configuration_validator.h"
typedef struct {
    ConfigurationValidationEnvironment environment;
    const PersistentIdentityAssignment *assignments;
    size_t assignment_count;
} ProductionCommissioningAuthorities;
/* Pure inspection of a complete historical prefix. No boot or repair writes.
 * Requires a valid configuration and a characterized real storage environment. */
Tr2Result production_commissioning_inspect_prefix(void *context,
    const uint8_t *image, size_t size);
Tr2Result production_commissioning_check_attribution(void *context,
    const uint32_t uid[3], uint32_t device_id);
typedef struct {
    ProductionCommissioningAuthorities *authorities;
    uint32_t observed_uid[3];
    uint32_t target_uid[3];
    uint32_t device_id;
    uint64_t expected_generation;
    bool exclusive_ownership;
    const ActiveConfigurationSnapshot *configuration;
} ProductionCommissioningConfigurationRequest;
/* Explicit off-boot prerequisite: installs only into a genuinely EMPTY config
 * slot, preserving non-empty peers and every other historical byte. */
Tr2Result production_commissioning_install_configuration(TransactionalImageMedia *media,
    const ProductionCommissioningConfigurationRequest *request, bool write_authorized,
    ProductionProvisioningResult *result);
#endif
