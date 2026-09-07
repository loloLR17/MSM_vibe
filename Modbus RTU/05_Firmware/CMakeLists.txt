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
    src/application/command_start_acquisition.c
    src/application/command_stop_acquisition.c
    src/application/command_synchronize_time.c
    src/application/configuration_activation_adapter.c
    src/application/configuration_service.c
    src/application/configuration_workflow.c
    src/application/supervision_service.c
    src/application/system_runtime.c
    src/application/system_runtime_acquisition_command.c
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
    src/modbus/register_model.c
    src/modbus/projection.c
    src/modbus/read_adapter.c
    src/modbus/write_adapter.c
    src/persistence/campaign_data_store_persistent.c
    src/persistence/campaign_record.c
    src/persistence/campaign_repository_store.c
    src/persistence/command_journal.c
    src/persistence/command_journal_context_store.c
    src/persistence/command_journal_record.c
    src/persistence/command_journal_store.c
    src/persistence/configuration_record.c
    src/persistence/configuration_store.c
    src/persistence/persistent_storage_core.c
    src/persistence/time_history_record.c
    src/persistence/time_history_store.c
)

target_include_directories(tr2_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(tr2_platform_host STATIC
    platform/host/host_platform.c
)

target_include_directories(tr2_platform_host
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/host/include
)

target_link_libraries(tr2_platform_host PUBLIC tr2_core)

add_executable(tr2_host
    platform/host/main.c
)

target_link_libraries(tr2_host PRIVATE tr2_platform_host)

enable_testing()

add_executable(test_codec tests/unit/test_codec.c)
target_link_libraries(test_codec PRIVATE tr2_core)
add_test(NAME codec COMMAND test_codec)

add_executable(test_b4_configuration_codec tests/unit/test_b4_configuration_codec.c)
target_link_libraries(test_b4_configuration_codec PRIVATE tr2_core)
add_test(NAME b4_configuration_codec COMMAND test_b4_configuration_codec)

add_executable(test_register_model tests/unit/test_register_model.c)
target_link_libraries(test_register_model PRIVATE tr2_core)
add_test(NAME register_model COMMAND test_register_model)

add_executable(test_projection_b0 tests/unit/test_projection_b0.c)
target_link_libraries(test_projection_b0 PRIVATE tr2_core)
add_test(NAME projection_b0 COMMAND test_projection_b0)

add_executable(test_projection_b1 tests/unit/test_projection_b1.c)
target_link_libraries(test_projection_b1 PRIVATE tr2_core)
add_test(NAME projection_b1 COMMAND test_projection_b1)

add_executable(test_projection_b2 tests/unit/test_projection_b2.c)
target_link_libraries(test_projection_b2 PRIVATE tr2_core)
add_test(NAME projection_b2 COMMAND test_projection_b2)

add_executable(test_projection_b3 tests/unit/test_projection_b3.c)
target_link_libraries(test_projection_b3 PRIVATE tr2_core)
add_test(NAME projection_b3 COMMAND test_projection_b3)

add_executable(test_projection_b4 tests/unit/test_projection_b4.c)
target_link_libraries(test_projection_b4 PRIVATE tr2_core)
add_test(NAME projection_b4 COMMAND test_projection_b4)

add_executable(test_campaign_inventory_b6 tests/unit/test_campaign_inventory_b6.c)
target_link_libraries(test_campaign_inventory_b6 PRIVATE tr2_core)
add_test(NAME campaign_inventory_b6 COMMAND test_campaign_inventory_b6)

add_executable(test_b6_adapters tests/unit/test_b6_adapters.c)
target_link_libraries(test_b6_adapters PRIVATE tr2_core)
add_test(NAME b6_adapters COMMAND test_b6_adapters)

add_executable(test_time_service tests/unit/test_time_service.c)
target_link_libraries(test_time_service PRIVATE tr2_core)
add_test(NAME time_service COMMAND test_time_service)

add_executable(test_time_recovery_types tests/unit/test_time_recovery_types.c)
target_link_libraries(test_time_recovery_types PRIVATE tr2_core)
add_test(NAME time_recovery_types COMMAND test_time_recovery_types)

add_executable(test_time_history_record tests/unit/test_time_history_record.c)
target_link_libraries(test_time_history_record PRIVATE tr2_core)
add_test(NAME time_history_record COMMAND test_time_history_record)

add_executable(test_time_history_store tests/unit/test_time_history_store.c)
target_link_libraries(test_time_history_store PRIVATE tr2_core)
add_test(NAME time_history_store COMMAND test_time_history_store)

add_executable(test_time_synchronization tests/unit/test_time_synchronization.c)
target_link_libraries(test_time_synchronization PRIVATE tr2_core)
add_test(NAME time_synchronization COMMAND test_time_synchronization)

add_executable(test_configuration_types tests/unit/test_configuration_types.c)
target_link_libraries(test_configuration_types PRIVATE tr2_core)
add_test(NAME configuration_types COMMAND test_configuration_types)

add_executable(test_configuration_staging tests/unit/test_configuration_staging.c)
target_link_libraries(test_configuration_staging PRIVATE tr2_core)
add_test(NAME configuration_staging COMMAND test_configuration_staging)

add_executable(test_configuration_validator tests/unit/test_configuration_validator.c)
target_link_libraries(test_configuration_validator PRIVATE tr2_core)
add_test(NAME configuration_validator COMMAND test_configuration_validator)

add_executable(test_configuration_workflow tests/unit/test_configuration_workflow.c)
target_link_libraries(test_configuration_workflow PRIVATE tr2_core)
add_test(NAME configuration_workflow COMMAND test_configuration_workflow)

add_executable(test_configuration_service tests/unit/test_configuration_service.c)
target_link_libraries(test_configuration_service PRIVATE tr2_core)
add_test(NAME configuration_service COMMAND test_configuration_service)

add_executable(test_persistent_storage_core tests/unit/test_persistent_storage_core.c)
target_link_libraries(test_persistent_storage_core PRIVATE tr2_core)
add_test(NAME persistent_storage_core COMMAND test_persistent_storage_core)

add_executable(test_configuration_record tests/unit/test_configuration_record.c)
target_link_libraries(test_configuration_record PRIVATE tr2_core)
add_test(NAME configuration_record COMMAND test_configuration_record)

add_executable(test_campaign_record tests/unit/test_campaign_record.c)
target_link_libraries(test_campaign_record PRIVATE tr2_core)
add_test(NAME campaign_record COMMAND test_campaign_record)

add_executable(test_campaign_repository_store tests/unit/test_campaign_repository_store.c)
target_link_libraries(test_campaign_repository_store PRIVATE tr2_core)
add_test(NAME campaign_repository_store COMMAND test_campaign_repository_store)

add_executable(test_campaign_data_store_persistent tests/unit/test_campaign_data_store_persistent.c)
target_link_libraries(test_campaign_data_store_persistent PRIVATE tr2_core)
add_test(NAME campaign_data_store_persistent COMMAND test_campaign_data_store_persistent)

add_executable(test_campaign_service tests/unit/test_campaign_service.c)
target_link_libraries(test_campaign_service PRIVATE tr2_core)
add_test(NAME campaign_service COMMAND test_campaign_service)

add_executable(test_configuration_store tests/unit/test_configuration_store.c)
target_link_libraries(test_configuration_store PRIVATE tr2_core)
add_test(NAME configuration_store COMMAND test_configuration_store)

add_executable(test_configuration_store_recovery tests/unit/test_configuration_store_recovery.c)
target_link_libraries(test_configuration_store_recovery PRIVATE tr2_core)
add_test(NAME configuration_store_recovery COMMAND test_configuration_store_recovery)

add_executable(test_configuration_store_recovery_unavailable tests/unit/test_configuration_store_recovery_unavailable.c)
target_link_libraries(test_configuration_store_recovery_unavailable PRIVATE tr2_core)
add_test(NAME configuration_store_recovery_unavailable COMMAND test_configuration_store_recovery_unavailable)

add_executable(test_read_adapter tests/unit/test_read_adapter.c)
target_link_libraries(test_read_adapter PRIVATE tr2_core)
add_test(NAME read_adapter COMMAND test_read_adapter)

add_executable(test_read_adapter_b3 tests/unit/test_read_adapter_b3.c)
target_link_libraries(test_read_adapter_b3 PRIVATE tr2_core)
add_test(NAME read_adapter_b3 COMMAND test_read_adapter_b3)

add_executable(test_write_adapter tests/unit/test_write_adapter.c)
target_link_libraries(test_write_adapter PRIVATE tr2_core)
add_test(NAME write_adapter COMMAND test_write_adapter)

add_executable(test_p5a_contracts tests/unit/test_p5a_contracts.c)
target_link_libraries(test_p5a_contracts PRIVATE tr2_core)
add_test(NAME p5a_contracts COMMAND test_p5a_contracts)

add_executable(test_p6a_contracts tests/unit/test_p6a_contracts.c)
target_link_libraries(test_p6a_contracts PRIVATE tr2_core)
add_test(NAME p6a_contracts COMMAND test_p6a_contracts)

add_executable(test_p7a_command_types tests/unit/test_p7a_command_types.c)
target_link_libraries(test_p7a_command_types PRIVATE tr2_core)
add_test(NAME p7a_command_types COMMAND test_p7a_command_types)

add_executable(test_p7b_command_request_mailbox tests/unit/test_p7b_command_request_mailbox.c)
target_link_libraries(test_p7b_command_request_mailbox PRIVATE tr2_core)
add_test(NAME p7b_command_request_mailbox COMMAND test_p7b_command_request_mailbox)

add_executable(test_p7c_command_journal tests/unit/test_p7c_command_journal.c)
target_link_libraries(test_p7c_command_journal PRIVATE tr2_core)
add_test(NAME p7c_command_journal COMMAND test_p7c_command_journal)

add_executable(test_p7d_command_journal_store tests/unit/test_p7d_command_journal_store.c)
target_link_libraries(test_p7d_command_journal_store PRIVATE tr2_core)
add_test(NAME p7d_command_journal_store COMMAND test_p7d_command_journal_store)

add_executable(test_p7e_command_engine_admission tests/unit/test_p7e_command_engine_admission.c)
target_link_libraries(test_p7e_command_engine_admission PRIVATE tr2_core)
add_test(NAME p7e_command_engine_admission COMMAND test_p7e_command_engine_admission)

add_executable(test_p7f_command_engine_lifecycle tests/unit/test_p7f_command_engine_lifecycle.c)
target_link_libraries(test_p7f_command_engine_lifecycle PRIVATE tr2_core)
add_test(NAME p7f_command_engine_lifecycle COMMAND test_p7f_command_engine_lifecycle)

add_executable(test_p7g_apply_configuration tests/unit/test_p7g_apply_configuration.c)
target_link_libraries(test_p7g_apply_configuration PRIVATE tr2_core)
add_test(NAME p7g_apply_configuration COMMAND test_p7g_apply_configuration)

add_executable(test_p7h_synchronize_time tests/unit/test_p7h_synchronize_time.c)
target_link_libraries(test_p7h_synchronize_time PRIVATE tr2_core)
add_test(NAME p7h_synchronize_time COMMAND test_p7h_synchronize_time)

add_executable(test_p7i_start_acquisition tests/unit/test_p7i_start_acquisition.c)
target_link_libraries(test_p7i_start_acquisition PRIVATE tr2_core)
add_test(NAME p7i_start_acquisition COMMAND test_p7i_start_acquisition)

add_executable(test_p7j_stop_acquisition tests/unit/test_p7j_stop_acquisition.c)
target_link_libraries(test_p7j_stop_acquisition PRIVATE tr2_core)
add_test(NAME p7j_stop_acquisition COMMAND test_p7j_stop_acquisition)

add_executable(test_p7k_command_boot_recovery tests/unit/test_p7k_command_boot_recovery.c)
target_link_libraries(test_p7k_command_boot_recovery PRIVATE tr2_core)
add_test(NAME p7k_command_boot_recovery COMMAND test_p7k_command_boot_recovery)

add_executable(test_p7l_b5_adapters tests/unit/test_p7l_b5_adapters.c)
target_link_libraries(test_p7l_b5_adapters PRIVATE tr2_core)
add_test(NAME p7l_b5_adapters COMMAND test_p7l_b5_adapters)

add_executable(test_p7m_command_policy tests/unit/test_p7m_command_policy.c)
target_link_libraries(test_p7m_command_policy PRIVATE tr2_core)
add_test(NAME p7m_command_policy COMMAND test_p7m_command_policy)

add_executable(test_p7n_transaction_fault_injection tests/unit/test_p7n_transaction_fault_injection.c)
target_link_libraries(test_p7n_transaction_fault_injection PRIVATE tr2_core)
add_test(NAME p7n_transaction_fault_injection COMMAND test_p7n_transaction_fault_injection)

add_executable(test_acquisition_service tests/unit/test_acquisition_service.c)
target_link_libraries(test_acquisition_service PRIVATE tr2_core)
add_test(NAME acquisition_service COMMAND test_acquisition_service)

add_executable(test_indicator_calculator tests/unit/test_indicator_calculator.c)
target_link_libraries(test_indicator_calculator PRIVATE tr2_core)
add_test(NAME indicator_calculator COMMAND test_indicator_calculator)

add_executable(test_threshold_evaluator tests/unit/test_threshold_evaluator.c)
target_link_libraries(test_threshold_evaluator PRIVATE tr2_core)
add_test(NAME threshold_evaluator COMMAND test_threshold_evaluator)

add_executable(test_supervision_service tests/unit/test_supervision_service.c)
target_link_libraries(test_supervision_service PRIVATE tr2_core)
add_test(NAME supervision_service COMMAND test_supervision_service)

add_executable(test_supervision_temporal_envelope tests/unit/test_supervision_temporal_envelope.c)
target_link_libraries(test_supervision_temporal_envelope PRIVATE tr2_core)
add_test(NAME supervision_temporal_envelope COMMAND test_supervision_temporal_envelope)

add_executable(test_system_runtime tests/integration/test_system_runtime.c)
target_link_libraries(test_system_runtime PRIVATE tr2_platform_host)
add_test(NAME system_runtime COMMAND test_system_runtime)

add_executable(test_p8i_system_runtime_acquisition_commands tests/integration/test_p8i_system_runtime_acquisition_commands.c)
target_link_libraries(test_p8i_system_runtime_acquisition_commands PRIVATE tr2_platform_host)
add_test(NAME p8i_system_runtime_acquisition_commands COMMAND test_p8i_system_runtime_acquisition_commands)

add_executable(test_p8j_system_runtime_b3_projection tests/integration/test_p8j_system_runtime_b3_projection.c)
target_link_libraries(test_p8j_system_runtime_b3_projection PRIVATE tr2_platform_host)
add_test(NAME p8j_system_runtime_b3_projection COMMAND test_p8j_system_runtime_b3_projection)

add_executable(test_time_boot_recovery tests/integration/test_time_boot_recovery.c)
target_link_libraries(test_time_boot_recovery PRIVATE tr2_platform_host)
add_test(NAME time_boot_recovery COMMAND test_time_boot_recovery)

add_executable(test_time_fault_injection tests/integration/test_time_fault_injection.c)
target_link_libraries(test_time_fault_injection PRIVATE tr2_core)
add_test(NAME time_fault_injection COMMAND test_time_fault_injection)

add_executable(test_configuration_b4_flow tests/integration/test_configuration_b4_flow.c)
target_link_libraries(test_configuration_b4_flow PRIVATE tr2_core)
add_test(NAME configuration_b4_flow COMMAND test_configuration_b4_flow)

add_executable(test_configuration_fault_injection tests/integration/test_configuration_fault_injection.c)
target_link_libraries(test_configuration_fault_injection PRIVATE tr2_platform_host)
add_test(NAME configuration_fault_injection COMMAND test_configuration_fault_injection)

add_executable(test_configuration_activation_adapter tests/integration/test_configuration_activation_adapter.c)
target_link_libraries(test_configuration_activation_adapter PRIVATE tr2_core)
add_test(NAME configuration_activation_adapter COMMAND test_configuration_activation_adapter)

add_executable(test_p5h_acquisition_supervision_b3 tests/integration/test_p5h_acquisition_supervision_b3.c)
target_link_libraries(test_p5h_acquisition_supervision_b3 PRIVATE tr2_platform_host)
add_test(NAME p5h_acquisition_supervision_b3 COMMAND test_p5h_acquisition_supervision_b3)
