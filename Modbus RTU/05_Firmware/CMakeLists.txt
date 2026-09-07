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
    src/application/command_engine.c
    src/application/command_request_mailbox.c
    src/application/command_start_acquisition.c
    src/application/command_synchronize_time.c
    src/application/configuration_activation_adapter.c
    src/application/configuration_service.c
    src/application/configuration_workflow.c
    src/application/supervision_service.c
    src/application/system_runtime.c
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
tr2_add_core_test(test_acquisition_service tests/unit/test_acquisition_service.c acquisition_service)
tr2_add_core_test(test_indicator_calculator tests/unit/test_indicator_calculator.c indicator_calculator)
tr2_add_core_test(test_threshold_evaluator tests/unit/test_threshold_evaluator.c threshold_evaluator)
tr2_add_core_test(test_supervision_service tests/unit/test_supervision_service.c supervision_service)
tr2_add_core_test(test_supervision_temporal_envelope tests/unit/test_supervision_temporal_envelope.c supervision_temporal_envelope)

add_executable(test_system_runtime tests/integration/test_system_runtime.c)
target_link_libraries(test_system_runtime PRIVATE tr2_platform_host)
add_test(NAME system_runtime COMMAND test_system_runtime)

add_executable(test_time_boot_recovery tests/integration/test_time_boot_recovery.c)
target_link_libraries(test_time_boot_recovery PRIVATE tr2_platform_host)
add_test(NAME time_boot_recovery COMMAND test_time_boot_recovery)

tr2_add_core_test(test_time_fault_injection tests/integration/test_time_fault_injection.c time_fault_injection)
tr2_add_core_test(test_configuration_b4_flow tests/integration/test_configuration_b4_flow.c configuration_b4_flow)

add_executable(test_configuration_fault_injection tests/integration/test_configuration_fault_injection.c)
target_link_libraries(test_configuration_fault_injection PRIVATE tr2_platform_host)
add_test(NAME configuration_fault_injection COMMAND test_configuration_fault_injection)

tr2_add_core_test(test_configuration_activation_adapter tests/integration/test_configuration_activation_adapter.c configuration_activation_adapter)

add_executable(test_p5h_acquisition_supervision_b3 tests/integration/test_p5h_acquisition_supervision_b3.c)
target_link_libraries(test_p5h_acquisition_supervision_b3 PRIVATE tr2_platform_host)
add_test(NAME p5h_acquisition_supervision_b3 COMMAND test_p5h_acquisition_supervision_b3)
