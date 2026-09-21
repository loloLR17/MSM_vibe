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

enable_testing()

function(tr2_add_core_test target source test_name)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE tr2_core)
    add_test(NAME ${test_name} COMMAND ${target})
endfunction()

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

function(tr2_add_host_test target source test_name)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE tr2_platform_host)
    add_test(NAME ${test_name} COMMAND ${target})
endfunction()

tr2_add_host_test(test_system_runtime tests/integration/test_system_runtime.c system_runtime)
tr2_add_host_test(test_p8i_system_runtime_acquisition_commands tests/integration/test_p8i_system_runtime_acquisition_commands.c p8i_system_runtime_acquisition_commands)
tr2_add_host_test(test_p8j_system_runtime_b3_projection tests/integration/test_p8j_system_runtime_b3_projection.c p8j_system_runtime_b3_projection)
tr2_add_host_test(test_p8k_power_loss_boot_recovery tests/integration/test_p8k_power_loss_boot_recovery.c p8k_power_loss_boot_recovery)
tr2_add_host_test(test_p12h3c3c4e3_runtime_bounded_journal tests/integration/test_p12h3c3c4e3_runtime_bounded_journal.c p12h3c3c4e3_runtime_bounded_journal)
tr2_add_host_test(test_time_boot_recovery tests/integration/test_time_boot_recovery.c time_boot_recovery)
tr2_add_host_test(test_configuration_fault_injection tests/integration/test_configuration_fault_injection.c configuration_fault_injection)
tr2_add_host_test(test_p5h_acquisition_supervision_b3 tests/integration/test_p5h_acquisition_supervision_b3.c p5h_acquisition_supervision_b3)
tr2_add_host_test(test_post_p9_a5_host_modbus_read tests/integration/test_post_p9_a5_host_modbus_read.c post_p9_a5_host_modbus_read)
