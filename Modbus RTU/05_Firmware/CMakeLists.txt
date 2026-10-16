cmake_minimum_required(VERSION 3.20)
project(tr2_firmware_p0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_library(tr2_core STATIC
    src/common/cooperative_work.c
    src/domain/acquisition/window_statistics.c
    src/application/live_modbus_server.c
    src/application/live_metrology.c
    src/persistence/live_command_store.c
    src/application/live_command_engine.c
    src/common/sha256.c
    src/application/live_configuration.c
    src/application/acquisition_service.c
    src/application/campaign_inventory_service.c
    src/application/campaign_service.c
    src/application/command_apply_configuration.c
    src/application/command_boot_recovery.c
    src/application/command_engine.c
    src/application/command_policy.c
    src/application/command_request_mailbox.c
    src/application/command_selftest.c
    src/application/command_start_acquisition.c
    src/application/command_stop_acquisition.c
    src/application/command_synchronize_time.c
    src/application/configuration_activation_adapter.c
    src/application/configuration_service.c
    src/application/configuration_workflow.c
    src/application/diagnostic_service.c
    src/application/supervision_service.c
    src/application/system_command_dispatcher.c
    src/application/production_provisioning.c
    src/application/persistent_identity.c
    src/application/production_commissioning.c
    src/application/production_application.c
    src/application/modbus_system_server.c
    src/application/system_runtime.c
    src/application/system_runtime_acquisition_command.c
    src/application/system_state_aggregator.c
    src/domain/command/command.c
    src/domain/configuration/configuration_staging.c
    src/domain/configuration/configuration_validator.c
    src/domain/identity/identity.c
    src/domain/supervision/indicator_calculator.c
    src/domain/supervision/threshold_evaluator.c
    src/domain/system_state/system_state.c
    src/domain/time/time_recovery.c
    src/domain/time/time_service.c
    src/modbus/b4_configuration_codec.c
    src/modbus/b5_projection.c
    src/modbus/b5_write_adapter.c
    src/modbus/b6_projection.c
    src/modbus/b6_write_adapter.c
    src/modbus/codec.c
    src/modbus/pdu_server.c
    src/modbus/register_model.c
    src/modbus/projection.c
    src/modbus/read_adapter.c
    src/modbus/rtu_receiver.c
    src/modbus/rtu_server_runtime.c
    src/modbus/write_adapter.c
    src/persistence/campaign_bulk_aggregator.c
    src/persistence/campaign_bulk_block.c
    src/persistence/campaign_bulk_block_stream.c
    src/persistence/campaign_bulk_checkpoint.c
    src/persistence/campaign_bulk_media.c
    src/persistence/campaign_bulk_metadata.c
    src/persistence/campaign_data_store_bulk.c
    src/persistence/campaign_data_store_persistent.c
    src/persistence/campaign_record.c
    src/persistence/campaign_repository_store.c
    src/persistence/command_journal.c
    src/persistence/command_journal_bounded_record.c
    src/persistence/command_journal_bounded_recovery.c
    src/persistence/command_journal_bounded_reader.c
    src/persistence/command_journal_bounded_slot.c
    src/persistence/command_journal_bounded_writer.c
    src/persistence/command_journal_bounded_admission.c
    src/persistence/command_journal_bounded_store.c
    src/persistence/command_journal_context_store.c
    src/persistence/command_journal_record.c
    src/persistence/command_journal_store.c
    src/persistence/configuration_record.c
    src/persistence/configuration_store.c
    src/persistence/diagnostic_history_record.c
    src/persistence/diagnostic_history_store.c
    src/persistence/persistent_storage_core.c
    src/persistence/time_history_record.c
    src/persistence/time_history_store.c
    src/persistence/transactional_image_media.c
    src/platform/serial_transport.c
)

target_include_directories(tr2_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(tr2_platform_host STATIC platform/host/host_platform.c)
target_include_directories(tr2_platform_host PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/platform/host/include)
target_link_libraries(tr2_platform_host PUBLIC tr2_core)

add_executable(tr2_host platform/host/main.c)
target_link_libraries(tr2_host PRIVATE tr2_platform_host)

add_executable(tr2_rtu_request platform/host/rtu_request.c)
target_link_libraries(tr2_rtu_request PRIVATE tr2_core)

enable_testing()
add_executable(test_stm32_continuous tests/platform/test_stm32_continuous.c
    platform/stm32/stm32_iis3dwb_continuous.c)
target_include_directories(test_stm32_continuous PRIVATE
    tests/platform/stm32_hal_ctrl1_mock platform/stm32)
target_compile_definitions(test_stm32_continuous PRIVATE TR2_STM32_CONTINUOUS_FIFO=1)
target_link_libraries(test_stm32_continuous PRIVATE tr2_core)
add_test(NAME stm32_continuous COMMAND test_stm32_continuous)

add_executable(test_stm32_ctrl1_differential tests/platform/test_stm32_ctrl1_differential.c
    platform/stm32/stm32_iis3dwb_ctrl1_differential.c)
target_include_directories(test_stm32_ctrl1_differential PRIVATE
    tests/platform/stm32_hal_ctrl1_mock platform/stm32)
target_compile_definitions(test_stm32_ctrl1_differential PRIVATE TR2_STM32_CTRL1_DIFFERENTIAL=1)
target_link_libraries(test_stm32_ctrl1_differential PRIVATE tr2_core)
add_test(NAME stm32_ctrl1_differential COMMAND test_stm32_ctrl1_differential)

add_executable(test_stm32_burst_capture tests/platform/test_stm32_burst_capture.c
    platform/stm32/stm32_iis3dwb_burst_capture.c)
target_include_directories(test_stm32_burst_capture PRIVATE
    tests/platform/stm32_hal_register_mock platform/stm32)
target_compile_definitions(test_stm32_burst_capture PRIVATE TR2_STM32_BURST_CAPTURE=1)
target_link_libraries(test_stm32_burst_capture PRIVATE tr2_core)
add_test(NAME stm32_burst_capture COMMAND test_stm32_burst_capture)

add_executable(test_stm32_active_readback tests/platform/test_stm32_active_readback.c
    platform/stm32/stm32_iis3dwb_active_readback.c)
target_include_directories(test_stm32_active_readback PRIVATE
    tests/platform/stm32_hal_register_mock platform/stm32)
target_compile_definitions(test_stm32_active_readback PRIVATE TR2_STM32_ACTIVE_READBACK=1)
add_test(NAME stm32_active_readback COMMAND test_stm32_active_readback)

add_executable(test_stm32_register_probe tests/platform/test_stm32_register_probe.c
    platform/stm32/stm32_iis3dwb_register_probe.c)
target_include_directories(test_stm32_register_probe PRIVATE
    tests/platform/stm32_hal_register_mock platform/stm32)
target_compile_definitions(test_stm32_register_probe PRIVATE TR2_STM32_REGISTER_PROBE=1)
add_test(NAME stm32_register_probe COMMAND test_stm32_register_probe)


# Test only the new isolated FIFO harness; portable core is unchanged.
add_executable(test_stm32_iis3dwb_fifo
    tests/platform/test_stm32_iis3dwb_fifo.c
    platform/stm32/stm32_iis3dwb_vibration_source.c
    platform/stm32/stm32_iis3dwb_fifo_capture.c)
target_include_directories(test_stm32_iis3dwb_fifo PRIVATE
    tests/platform/stm32_hal_iis3dwb_mock platform/stm32)
target_compile_definitions(test_stm32_iis3dwb_fifo PRIVATE TR2_STM32_FIFO_CAPTURE=1)
target_link_libraries(test_stm32_iis3dwb_fifo PRIVATE tr2_core)
add_test(NAME stm32_iis3dwb_fifo COMMAND test_stm32_iis3dwb_fifo)

# Execute the real sensor driver and bounded diagnostic against a host SPI HAL.
add_executable(test_stm32_iis3dwb
    tests/platform/test_stm32_iis3dwb.c
    platform/stm32/stm32_iis3dwb_vibration_source.c
    platform/stm32/stm32_iis3dwb_capture.c)
target_include_directories(test_stm32_iis3dwb PRIVATE
    tests/platform/stm32_hal_iis3dwb_mock platform/stm32)
target_compile_definitions(test_stm32_iis3dwb PRIVATE TR2_STM32_ACQUISITION_CAPTURE=1)
target_link_libraries(test_stm32_iis3dwb PRIVATE tr2_core)
add_test(NAME stm32_iis3dwb COMMAND test_stm32_iis3dwb)

# Host mock exercises the actual STM32 board GPIO source, without target access.
add_executable(test_stm32_rs485_rx_gpio
    tests/platform/test_stm32_rs485_rx_gpio.c
    platform/stm32/stm32_rs485_rx_gpio.c
)
target_include_directories(test_stm32_rs485_rx_gpio PRIVATE
    tests/platform/stm32_hal_rx_mock platform/stm32
)
add_test(NAME stm32_rs485_rx_gpio COMMAND test_stm32_rs485_rx_gpio)

# Compile the real STM32 adapter against a host HAL, both qualification states.
foreach(tx_qualified 0 1)
    foreach(irq_observer 0 1)
        if(irq_observer)
            set(test_name stm32_irq_observer_${tx_qualified})
        else()
            set(test_name stm32_rtu_tx_${tx_qualified})
        endif()
        add_executable(test_${test_name}
            tests/platform/test_stm32_rtu_tx.c
            platform/stm32/stm32_serial_transport.c
            platform/stm32/stm32_modbus_diagnostic.c
        )
        target_include_directories(test_${test_name} PRIVATE
            tests/platform/stm32_hal_tx_mock platform/stm32)
        target_compile_definitions(test_${test_name} PRIVATE
            TR2_STM32_RS485_TX_QUALIFIED=${tx_qualified}
            TR2_STM32_IRQ_DIAGNOSTICS=${irq_observer})
        target_link_libraries(test_${test_name} PRIVATE tr2_core)
        add_test(NAME ${test_name} COMMAND test_${test_name})
    endforeach()
endforeach()

function(tr2_add_core_test target source test_name)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE tr2_core)
    add_test(NAME ${test_name} COMMAND ${target})
endfunction()

add_executable(test_stm32_modbus_diagnostic
    tests/platform/test_stm32_modbus_diagnostic.c
    platform/stm32/stm32_modbus_diagnostic.c)
target_include_directories(test_stm32_modbus_diagnostic PRIVATE platform/stm32)
target_link_libraries(test_stm32_modbus_diagnostic PRIVATE tr2_core)
add_test(NAME stm32_modbus_diagnostic COMMAND test_stm32_modbus_diagnostic)

tr2_add_core_test(test_codec tests/unit/test_codec.c codec)
tr2_add_core_test(test_b4_configuration_codec tests/unit/test_b4_configuration_codec.c b4_configuration_codec)
tr2_add_core_test(test_register_model tests/unit/test_register_model.c register_model)
tr2_add_core_test(test_projection_b0 tests/unit/test_projection_b0.c projection_b0)
tr2_add_core_test(test_projection_b1 tests/unit/test_projection_b1.c projection_b1)
tr2_add_core_test(test_projection_b2 tests/unit/test_projection_b2.c projection_b2)
tr2_add_core_test(test_projection_b3 tests/unit/test_projection_b3.c projection_b3)
tr2_add_core_test(test_projection_b4 tests/unit/test_projection_b4.c projection_b4)
tr2_add_core_test(test_campaign_inventory_b6 tests/unit/test_campaign_inventory_b6.c campaign_inventory_b6)
tr2_add_core_test(test_b6_adapters tests/unit/test_b6_adapters.c b6_adapters)
tr2_add_core_test(test_time_service tests/unit/test_time_service.c time_service)
tr2_add_core_test(test_time_recovery_types tests/unit/test_time_recovery_types.c time_recovery_types)
tr2_add_core_test(test_time_history_record tests/unit/test_time_history_record.c time_history_record)
tr2_add_core_test(test_time_history_store tests/unit/test_time_history_store.c time_history_store)
tr2_add_core_test(test_time_synchronization tests/unit/test_time_synchronization.c time_synchronization)
tr2_add_core_test(test_configuration_types tests/unit/test_configuration_types.c configuration_types)
tr2_add_core_test(test_configuration_staging tests/unit/test_configuration_staging.c configuration_staging)
tr2_add_core_test(test_configuration_validator tests/unit/test_configuration_validator.c configuration_validator)
tr2_add_core_test(test_configuration_workflow tests/unit/test_configuration_workflow.c configuration_workflow)
tr2_add_core_test(test_configuration_service tests/unit/test_configuration_service.c configuration_service)
tr2_add_core_test(test_persistent_storage_core tests/unit/test_persistent_storage_core.c persistent_storage_core)
tr2_add_core_test(test_configuration_record tests/unit/test_configuration_record.c configuration_record)
tr2_add_core_test(test_campaign_record tests/unit/test_campaign_record.c campaign_record)
tr2_add_core_test(test_campaign_repository_store tests/unit/test_campaign_repository_store.c campaign_repository_store)
tr2_add_core_test(test_campaign_bulk_aggregator tests/unit/test_campaign_bulk_aggregator.c campaign_bulk_aggregator)
tr2_add_core_test(test_campaign_bulk_block tests/unit/test_campaign_bulk_block.c campaign_bulk_block)
tr2_add_core_test(test_campaign_bulk_block_stream tests/unit/test_campaign_bulk_block_stream.c campaign_bulk_block_stream)
tr2_add_core_test(test_campaign_bulk_checkpoint tests/unit/test_campaign_bulk_checkpoint.c campaign_bulk_checkpoint)
tr2_add_core_test(test_campaign_bulk_media tests/unit/test_campaign_bulk_media.c campaign_bulk_media)
tr2_add_core_test(test_campaign_bulk_media_region tests/unit/test_campaign_bulk_media_region.c campaign_bulk_media_region)
tr2_add_core_test(test_campaign_bulk_metadata tests/unit/test_campaign_bulk_metadata.c campaign_bulk_metadata)
tr2_add_core_test(test_campaign_data_store_bulk tests/unit/test_campaign_data_store_bulk.c campaign_data_store_bulk)
tr2_add_core_test(test_campaign_data_store_persistent tests/unit/test_campaign_data_store_persistent.c campaign_data_store_persistent)
tr2_add_core_test(test_campaign_service tests/unit/test_campaign_service.c campaign_service)
tr2_add_core_test(test_configuration_store tests/unit/test_configuration_store.c configuration_store)
tr2_add_core_test(test_configuration_store_recovery tests/unit/test_configuration_store_recovery.c configuration_store_recovery)
tr2_add_core_test(test_configuration_store_recovery_unavailable tests/unit/test_configuration_store_recovery_unavailable.c configuration_store_recovery_unavailable)
tr2_add_core_test(test_read_adapter tests/unit/test_read_adapter.c read_adapter)
tr2_add_core_test(test_read_adapter_b3 tests/unit/test_read_adapter_b3.c read_adapter_b3)
tr2_add_core_test(test_write_adapter tests/unit/test_write_adapter.c write_adapter)
tr2_add_core_test(test_p5a_contracts tests/unit/test_p5a_contracts.c p5a_contracts)
tr2_add_core_test(test_p6a_contracts tests/unit/test_p6a_contracts.c p6a_contracts)
tr2_add_core_test(test_p7a_command_types tests/unit/test_p7a_command_types.c p7a_command_types)
tr2_add_core_test(test_p7b_command_request_mailbox tests/unit/test_p7b_command_request_mailbox.c p7b_command_request_mailbox)
tr2_add_core_test(test_p7c_command_journal tests/unit/test_p7c_command_journal.c p7c_command_journal)
tr2_add_core_test(test_p7d_command_journal_store tests/unit/test_p7d_command_journal_store.c p7d_command_journal_store)
tr2_add_core_test(test_p7e_command_engine_admission tests/unit/test_p7e_command_engine_admission.c p7e_command_engine_admission)
tr2_add_core_test(test_p7f_command_engine_lifecycle tests/unit/test_p7f_command_engine_lifecycle.c p7f_command_engine_lifecycle)
tr2_add_core_test(test_p7g_apply_configuration tests/unit/test_p7g_apply_configuration.c p7g_apply_configuration)
tr2_add_core_test(test_p7h_synchronize_time tests/unit/test_p7h_synchronize_time.c p7h_synchronize_time)
tr2_add_core_test(test_p7i_start_acquisition tests/unit/test_p7i_start_acquisition.c p7i_start_acquisition)
tr2_add_core_test(test_p7j_stop_acquisition tests/unit/test_p7j_stop_acquisition.c p7j_stop_acquisition)
tr2_add_core_test(test_p7k_command_boot_recovery tests/unit/test_p7k_command_boot_recovery.c p7k_command_boot_recovery)
tr2_add_core_test(test_p7l_b5_adapters tests/unit/test_p7l_b5_adapters.c p7l_b5_adapters)
tr2_add_core_test(test_p7m_command_policy tests/unit/test_p7m_command_policy.c p7m_command_policy)
tr2_add_core_test(test_p7n_transaction_fault_injection tests/unit/test_p7n_transaction_fault_injection.c p7n_transaction_fault_injection)
tr2_add_core_test(test_p9b_diagnostic_service tests/unit/test_p9b_diagnostic_service.c p9b_diagnostic_service)
tr2_add_core_test(test_p9c_diagnostic_recovery tests/unit/test_p9c_diagnostic_recovery.c p9c_diagnostic_recovery)
tr2_add_core_test(test_p9h_selftest_command tests/unit/test_p9h_selftest_command.c p9h_selftest_command)
tr2_add_core_test(test_acquisition_service tests/unit/test_acquisition_service.c acquisition_service)
tr2_add_core_test(test_indicator_calculator tests/unit/test_indicator_calculator.c indicator_calculator)
tr2_add_core_test(test_threshold_evaluator tests/unit/test_threshold_evaluator.c threshold_evaluator)
tr2_add_core_test(test_supervision_service tests/unit/test_supervision_service.c supervision_service)
tr2_add_core_test(test_supervision_temporal_envelope tests/unit/test_supervision_temporal_envelope.c supervision_temporal_envelope)
tr2_add_core_test(test_time_fault_injection tests/integration/test_time_fault_injection.c time_fault_injection)
tr2_add_core_test(test_configuration_b4_flow tests/integration/test_configuration_b4_flow.c configuration_b4_flow)
tr2_add_core_test(test_configuration_activation_adapter tests/integration/test_configuration_activation_adapter.c configuration_activation_adapter)
tr2_add_core_test(test_p12c_pdu_server tests/unit/test_p12c_pdu_server.c p12c_pdu_server)
tr2_add_core_test(test_p12d_rtu_receiver tests/unit/test_p12d_rtu_receiver.c p12d_rtu_receiver)
tr2_add_core_test(test_p12e_serial_transport tests/unit/test_p12e_serial_transport.c p12e_serial_transport)
tr2_add_core_test(test_p12h_rtu_server_runtime tests/unit/test_p12h_rtu_server_runtime.c p12h_rtu_server_runtime)
tr2_add_core_test(test_p12h3c3b_command_journal_bounded_record tests/unit/test_p12h3c3b_command_journal_bounded_record.c p12h3c3b_command_journal_bounded_record)
tr2_add_core_test(test_p12h3c3c1_bounded_slot tests/unit/test_p12h3c3c1_bounded_slot.c p12h3c3c1_bounded_slot)
tr2_add_core_test(test_p12h3c3c2_bounded_recovery tests/unit/test_p12h3c3c2_bounded_recovery.c p12h3c3c2_bounded_recovery)
tr2_add_core_test(test_p12h3c3c3_bounded_reader tests/unit/test_p12h3c3c3_bounded_reader.c p12h3c3c3_bounded_reader)
tr2_add_core_test(test_p12h3c3c4_bounded_writer tests/unit/test_p12h3c3c4_bounded_writer.c p12h3c3c4_bounded_writer)
tr2_add_core_test(test_p12h3c3c4b_bounded_admission tests/unit/test_p12h3c3c4b_bounded_admission.c p12h3c3c4b_bounded_admission)
tr2_add_core_test(test_p12h3c3c4c1_bounded_store tests/unit/test_p12h3c3c4c1_bounded_store.c p12h3c3c4c1_bounded_store)
tr2_add_core_test(test_p12h3c3c4d1_fault_media tests/unit/test_p12h3c3c4d1_fault_media.c p12h3c3c4d1_fault_media)
tr2_add_core_test(test_p12h3c3c4d2_admission_power_loss tests/unit/test_p12h3c3c4d2_admission_power_loss.c p12h3c3c4d2_admission_power_loss)
tr2_add_core_test(test_p12h3c3c4d3_mutation_power_loss tests/unit/test_p12h3c3c4d3_mutation_power_loss.c p12h3c3c4d3_mutation_power_loss)
tr2_add_core_test(test_p12h3c3c4d4_completion_power_loss tests/unit/test_p12h3c3c4d4_completion_power_loss.c p12h3c3c4d4_completion_power_loss)
tr2_add_core_test(test_p12h3c3c4d5_readmission_power_loss tests/unit/test_p12h3c3c4d5_readmission_power_loss.c p12h3c3c4d5_readmission_power_loss)
tr2_add_core_test(test_p12h3c3c4d6_d8_consolidation tests/unit/test_p12h3c3c4d6_d8_consolidation.c p12h3c3c4d6_d8_consolidation)
tr2_add_core_test(test_p12h3d2_transactional_image_media tests/unit/test_p12h3d2_transactional_image_media.c p12h3d2_transactional_image_media)
tr2_add_core_test(test_p12h3d2b_transactional_image_power_loss tests/unit/test_p12h3d2b_transactional_image_power_loss.c p12h3d2b_transactional_image_power_loss)
tr2_add_core_test(test_p12h3d2c_transactional_image_recovery tests/unit/test_p12h3d2c_transactional_image_recovery.c p12h3d2c_transactional_image_recovery)
tr2_add_core_test(test_p12h3d2d_persistent_storage_core_integration tests/unit/test_p12h3d2d_persistent_storage_core_integration.c p12h3d2d_persistent_storage_core_integration)
tr2_add_core_test(test_p12h3d2e_campaign_transactional_media tests/unit/test_p12h3d2e_campaign_transactional_media.c p12h3d2e_campaign_transactional_media)
tr2_add_core_test(test_p12h3d2f_transactional_media_format_policy tests/unit/test_p12h3d2f_transactional_media_format_policy.c p12h3d2f_transactional_media_format_policy)
tr2_add_core_test(test_p12h3d2g_transactional_media_format_power_loss tests/unit/test_p12h3d2g_transactional_media_format_power_loss.c p12h3d2g_transactional_media_format_power_loss)
tr2_add_core_test(test_p12h3d2h1_transactional_media_geometry tests/unit/test_p12h3d2h1_transactional_media_geometry.c p12h3d2h1_transactional_media_geometry)
tr2_add_core_test(test_p12h3d2h3_transactional_media_alternate_geometry tests/unit/test_p12h3d2h3_transactional_media_alternate_geometry.c p12h3d2h3_transactional_media_alternate_geometry)

function(tr2_add_host_test target source test_name)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE tr2_platform_host)
    add_test(NAME ${test_name} COMMAND ${target})
endfunction()

tr2_add_host_test(test_system_runtime tests/integration/test_system_runtime.c system_runtime)
tr2_add_host_test(test_system_runtime_bulk tests/integration/test_system_runtime_bulk.c system_runtime_bulk)
tr2_add_host_test(test_p8i_system_runtime_acquisition_commands tests/integration/test_p8i_system_runtime_acquisition_commands.c p8i_system_runtime_acquisition_commands)
tr2_add_host_test(test_p8j_system_runtime_b3_projection tests/integration/test_p8j_system_runtime_b3_projection.c p8j_system_runtime_b3_projection)
tr2_add_host_test(test_p8k_power_loss_boot_recovery tests/integration/test_p8k_power_loss_boot_recovery.c p8k_power_loss_boot_recovery)
tr2_add_host_test(test_p12h3c3c4e3_runtime_bounded_journal tests/integration/test_p12h3c3c4e3_runtime_bounded_journal.c p12h3c3c4e3_runtime_bounded_journal)
tr2_add_host_test(test_time_boot_recovery tests/integration/test_time_boot_recovery.c time_boot_recovery)
tr2_add_host_test(test_configuration_fault_injection tests/integration/test_configuration_fault_injection.c configuration_fault_injection)
tr2_add_host_test(test_p5h_acquisition_supervision_b3 tests/integration/test_p5h_acquisition_supervision_b3.c p5h_acquisition_supervision_b3)
tr2_add_host_test(test_post_p9_a5_host_modbus_read tests/integration/test_post_p9_a5_host_modbus_read.c post_p9_a5_host_modbus_read)

tr2_add_host_test(test_modbus_system_server tests/integration/test_modbus_system_server.c modbus_system_server)

tr2_add_host_test(test_production_commands tests/integration/test_production_commands.c production_commands)

tr2_add_core_test(test_production_provisioning tests/unit/test_production_provisioning.c production_provisioning)

tr2_add_core_test(test_persistent_identity tests/unit/test_persistent_identity.c persistent_identity)

tr2_add_core_test(test_production_commissioning tests/unit/test_production_commissioning.c production_commissioning)

set(ST_REF "${CMAKE_CURRENT_SOURCE_DIR}/../../experiments/iis3dwb_st_reference")
add_executable(test_st_fifo_window ${ST_REF}/tests/test_fifo_window.c ${ST_REF}/src/fifo_window.c ${ST_REF}/vendor/st/iis3dwb_reg.c)
target_include_directories(test_st_fifo_window PRIVATE ${ST_REF}/src ${ST_REF}/vendor/st)
target_compile_options(test_st_fifo_window PRIVATE -Wall -Wextra -Werror -UNDEBUG)
add_test(NAME st_fifo_window_integration COMMAND test_st_fifo_window)
add_executable(test_st_fifo_admission tests/platform/test_st_fifo_admission.c)
target_include_directories(test_st_fifo_admission PRIVATE platform/stm32 ${ST_REF}/src ${ST_REF}/vendor/st)
target_compile_options(test_st_fifo_admission PRIVATE -Wall -Wextra -Werror -UNDEBUG)
add_test(NAME st_fifo_admission COMMAND test_st_fifo_admission)
add_executable(test_st_spi_diagnostic ${ST_REF}/tests/test_spi_diagnostic.c ${ST_REF}/src/spi_diagnostic.c)
target_include_directories(test_st_spi_diagnostic PRIVATE ${ST_REF}/src)
target_compile_options(test_st_spi_diagnostic PRIVATE -Wall -Wextra -Werror -UNDEBUG)
add_test(NAME st_spi_completion_integration COMMAND test_st_spi_diagnostic)

add_executable(test_st_window_adapter tests/platform/test_st_window_adapter.c ${ST_REF}/src/spi_diagnostic.c)
target_include_directories(test_st_window_adapter PRIVATE tests/platform/stm32_hal_st_window_mock include ${ST_REF}/src ${ST_REF}/official ${ST_REF}/vendor/st)
target_compile_options(test_st_window_adapter PRIVATE -Wall -Wextra -Werror -UNDEBUG -ffunction-sections -fdata-sections)
target_compile_definitions(test_st_window_adapter PRIVATE BENCH_DUPLEX=0)
target_link_options(test_st_window_adapter PRIVATE -Wl,--gc-sections)
add_test(NAME st_window_primary_preflight_and_transport COMMAND test_st_window_adapter)

add_executable(test_fifo_cooperative tests/platform/test_fifo_cooperative.c ${ST_REF}/src/fifo_cooperative.c ${ST_REF}/src/fifo_window.c ${ST_REF}/vendor/st/iis3dwb_reg.c)
target_include_directories(test_fifo_cooperative PRIVATE ${ST_REF}/src ${ST_REF}/vendor/st)
target_compile_definitions(test_fifo_cooperative PRIVATE BENCH_WINDOW_MAX_WORDS=36864)
target_compile_options(test_fifo_cooperative PRIVATE -Wall -Wextra -Werror -UNDEBUG)
add_test(NAME fifo_cooperative_v1_windows_and_faults COMMAND test_fifo_cooperative)

# A04-02: real RTU/command journal + cooperative FIFO with explicit I/O costs.
tr2_add_host_test(test_modbus_fifo_io_budget tests/integration/test_modbus_fifo_io_budget.c modbus_fifo_io_budget)
target_sources(test_modbus_fifo_io_budget PRIVATE ${ST_REF}/src/fifo_cooperative.c ${ST_REF}/src/fifo_window.c ${ST_REF}/vendor/st/iis3dwb_reg.c)
target_include_directories(test_modbus_fifo_io_budget PRIVATE ${ST_REF}/src ${ST_REF}/vendor/st)
target_compile_definitions(test_modbus_fifo_io_budget PRIVATE BENCH_WINDOW_MAX_WORDS=36864)

# M016 adapter error-state regression, real SD adapter with a bounded HAL model.
add_executable(test_sdmmc_bulk_media tests/platform/test_sdmmc_bulk_media.c platform/stm32/stm32_sdmmc_bulk_media.c)
target_include_directories(test_sdmmc_bulk_media PRIVATE tests/platform/stm32_hal_sdmmc_mock platform/stm32)
target_link_libraries(test_sdmmc_bulk_media PRIVATE tr2_core)
add_test(NAME sdmmc_bulk_media COMMAND test_sdmmc_bulk_media)

# R1: actual transactional stores, immutable R0 vectors and bounded tear injection.
tr2_add_core_test(test_live_configuration tests/unit/test_live_configuration.c live_configuration)
tr2_add_core_test(test_live_command_engine tests/unit/test_live_command_engine.c live_command_engine)

add_executable(test_live_fifo tests/platform/test_live_fifo.c platform/stm32/live_fifo_acquisition.c
 ${ST_REF}/src/fifo_live.c ${ST_REF}/src/fifo_window.c ${ST_REF}/src/fifo_cooperative.c ${ST_REF}/vendor/st/iis3dwb_reg.c)
target_include_directories(test_live_fifo PRIVATE platform/stm32 ${ST_REF}/src ${ST_REF}/vendor/st)
target_compile_definitions(test_live_fifo PRIVATE BENCH_WINDOW_MAX_WORDS=36864)
target_link_libraries(test_live_fifo PRIVATE tr2_core)
add_test(NAME live_fifo_continuous_owner COMMAND test_live_fifo)

add_executable(test_live_io_cost tests/unit/test_live_io_cost.c)
target_link_libraries(test_live_io_cost PRIVATE tr2_core)
add_test(NAME live_io_cost COMMAND test_live_io_cost)

add_executable(test_live_metrology tests/unit/test_live_metrology.c)
target_link_libraries(test_live_metrology PRIVATE tr2_core)
add_test(NAME live_metrology COMMAND test_live_metrology)

add_executable(test_cooperative_fram_fifo tests/platform/test_cooperative_fram_fifo.c platform/stm32/live_fifo_acquisition.c
 ${ST_REF}/src/fifo_live.c ${ST_REF}/src/fifo_window.c ${ST_REF}/vendor/st/iis3dwb_reg.c)
target_include_directories(test_cooperative_fram_fifo PRIVATE platform/stm32 ${ST_REF}/src ${ST_REF}/vendor/st)
target_link_libraries(test_cooperative_fram_fifo PRIVATE tr2_core)
add_test(NAME cooperative_fram_fifo COMMAND test_cooperative_fram_fifo)

add_executable(test_cooperative_work tests/unit/test_cooperative_work.c)
target_link_libraries(test_cooperative_work PRIVATE tr2_core)
add_test(NAME cooperative_work COMMAND test_cooperative_work)

add_executable(test_stm32_fram_bounded tests/platform/test_stm32_fram_bounded.c platform/stm32/stm32_fram_storage.c)
target_include_directories(test_stm32_fram_bounded PRIVATE tests/platform/stm32_hal_fram_mock platform/stm32)
target_link_libraries(test_stm32_fram_bounded PRIVATE tr2_core)
add_test(NAME stm32_fram_bounded COMMAND test_stm32_fram_bounded)

# R4M: isolated maintenance safety, real R4 snapshot archived losslessly.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_executable(test_r4m_maintenance tests/platform/test_r4m_maintenance.c platform/stm32/r4m_maintenance.c)
target_include_directories(test_r4m_maintenance PRIVATE platform/stm32)
target_link_libraries(test_r4m_maintenance PRIVATE tr2_core)
add_test(NAME r4m_maintenance COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/platform/run_r4m_maintenance.py
    $<TARGET_FILE:test_r4m_maintenance> ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/r4_started_snapshot.zlib)

tr2_add_core_test(test_live_effect_timing tests/unit/test_live_effect_timing.c live_effect_timing)

add_executable(test_r4p_maintenance tests/platform/test_r4p_maintenance.c platform/stm32/r4m_maintenance.c)
target_include_directories(test_r4p_maintenance PRIVATE platform/stm32)
target_link_libraries(test_r4p_maintenance PRIVATE tr2_core)
add_test(NAME r4p_maintenance COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/platform/run_r4p_maintenance.py $<TARGET_FILE:test_r4p_maintenance> ${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/r4p_started_snapshot.zlib)

add_executable(test_live_modbus_server tests/platform/test_live_modbus_server.c platform/stm32/live_fifo_acquisition.c
    ${ST_REF}/src/fifo_live.c ${ST_REF}/src/fifo_window.c ${ST_REF}/vendor/st/iis3dwb_reg.c)
target_include_directories(test_live_modbus_server PRIVATE platform/stm32 ${ST_REF}/src ${ST_REF}/vendor/st)
target_link_libraries(test_live_modbus_server PRIVATE tr2_core)
add_test(NAME live_modbus_server COMMAND test_live_modbus_server)

add_executable(test_fifo_event_time tests/platform/test_fifo_event_time.c platform/stm32/fifo_event_time.c)
target_include_directories(test_fifo_event_time PRIVATE platform/stm32 ${ST_REF}/src ${ST_REF}/vendor/st)
target_link_libraries(test_fifo_event_time PRIVATE tr2_core)
add_test(NAME fifo_event_time COMMAND test_fifo_event_time)
