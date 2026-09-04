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
    src/application/system_runtime.c
    src/domain/configuration/configuration_staging.c
    src/domain/configuration/configuration_validator.c
    src/domain/identity/identity.c
    src/domain/system_state/system_state.c
    src/domain/time/time_service.c
    src/modbus/codec.c
    src/modbus/register_model.c
    src/modbus/projection.c
    src/modbus/read_adapter.c
    src/modbus/write_adapter.c
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

add_executable(test_codec
    tests/unit/test_codec.c
)
target_link_libraries(test_codec PRIVATE tr2_core)
add_test(NAME codec COMMAND test_codec)

add_executable(test_register_model
    tests/unit/test_register_model.c
)
target_link_libraries(test_register_model PRIVATE tr2_core)
add_test(NAME register_model COMMAND test_register_model)

add_executable(test_projection_b0
    tests/unit/test_projection_b0.c
)
target_link_libraries(test_projection_b0 PRIVATE tr2_core)
add_test(NAME projection_b0 COMMAND test_projection_b0)

add_executable(test_projection_b1
    tests/unit/test_projection_b1.c
)
target_link_libraries(test_projection_b1 PRIVATE tr2_core)
add_test(NAME projection_b1 COMMAND test_projection_b1)

add_executable(test_projection_b2
    tests/unit/test_projection_b2.c
)
target_link_libraries(test_projection_b2 PRIVATE tr2_core)
add_test(NAME projection_b2 COMMAND test_projection_b2)

add_executable(test_time_service
    tests/unit/test_time_service.c
)
target_link_libraries(test_time_service PRIVATE tr2_core)
add_test(NAME time_service COMMAND test_time_service)

add_executable(test_configuration_types
    tests/unit/test_configuration_types.c
)
target_link_libraries(test_configuration_types PRIVATE tr2_core)
add_test(NAME configuration_types COMMAND test_configuration_types)

add_executable(test_configuration_staging
    tests/unit/test_configuration_staging.c
)
target_link_libraries(test_configuration_staging PRIVATE tr2_core)
add_test(NAME configuration_staging COMMAND test_configuration_staging)

add_executable(test_configuration_validator
    tests/unit/test_configuration_validator.c
)
target_link_libraries(test_configuration_validator PRIVATE tr2_core)
add_test(NAME configuration_validator COMMAND test_configuration_validator)

add_executable(test_read_adapter
    tests/unit/test_read_adapter.c
)
target_link_libraries(test_read_adapter PRIVATE tr2_core)
add_test(NAME read_adapter COMMAND test_read_adapter)

add_executable(test_write_adapter
    tests/unit/test_write_adapter.c
)
target_link_libraries(test_write_adapter PRIVATE tr2_core)
add_test(NAME write_adapter COMMAND test_write_adapter)

add_executable(test_system_runtime
    tests/integration/test_system_runtime.c
)
target_link_libraries(test_system_runtime PRIVATE tr2_platform_host)
add_test(NAME system_runtime COMMAND test_system_runtime)
