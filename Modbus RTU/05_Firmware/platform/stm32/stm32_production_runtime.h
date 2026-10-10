#ifndef TR2_STM32_PRODUCTION_RUNTIME_H
#define TR2_STM32_PRODUCTION_RUNTIME_H

#include "stm32_fram_storage.h"
#include "stm32_iis3dwb_vibration_source.h"
#include "tr2/application/production_application.h"

/* Independent of the qualification harness. GPIO/SPI setup is the caller's
 * responsibility; no default pinout/geometry, no destructive formatting. */
typedef struct {
    Stm32FramStorage fram;
    TransactionalImageMedia transactional_media;
    TransactionalImageRecoveryResult recovery;
    uint8_t candidate[TR2_TRANSACTIONAL_MEDIA_LOGICAL_SIZE];
    Stm32Iis3dwbVibrationSource vibration;
    ProductionApplication application;
    bool prepare_attempted;
} Stm32ProductionRuntime;

typedef struct {
    SPI_HandleTypeDef *fram_spi;
    GPIO_TypeDef *fram_cs_port;
    uint16_t fram_cs_pin;
    SPI_HandleTypeDef *vibration_spi;
    GPIO_TypeDef *vibration_cs_port;
    uint16_t vibration_cs_pin;
    TransactionalImageGeometry geometry;
    CampaignDataStore *campaign_data_store;
    ConfigurationValidationEnvironment environment;
    void *configuration_metadata_context;
    ConfigurationActivationMetadataAcquire configuration_metadata_acquire;
    const SelfTestExecutor *selftest_executor;
    const PlatformResetTrigger *reset_trigger;
} Stm32ProductionDependencies;

Tr2Result stm32_production_runtime_prepare(Stm32ProductionRuntime *runtime,
    const Stm32ProductionDependencies *dependencies);

/* Must return only explicitly provisioned, qualified, long-lived authorities.
 * Default NOT_AVAILABLE prevents all preparation and keeps RX diagnostics. */
Tr2Result stm32_production_binding_acquire(SerialTransport *uart,
    Stm32ProductionDependencies *dependencies, SerialTransport **qualified_transport,
    uint8_t *unit_id, const ModbusBlock0Image **identity);

#endif
