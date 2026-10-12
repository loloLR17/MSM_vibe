cmake_minimum_required(VERSION 3.20)
project(tr2_stm32_p12a LANGUAGES C ASM)

if(NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "P12-A STM32 build requires the ARM cross-toolchain. Configure with -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(STM32CUBE_U5_ROOT "" CACHE PATH "Path to a recursive STM32CubeU5 checkout (validated against v1.9.0)")
if(NOT STM32CUBE_U5_ROOT)
    message(FATAL_ERROR "Set -DSTM32CUBE_U5_ROOT=/path/to/STM32CubeU5 (recursive checkout, v1.9.0)")
endif()

option(TR2_STM32_COMMISSIONING_READONLY "Isolated media inspection and backup, no data writes or Modbus" OFF)
if(TR2_STM32_COMMISSIONING_READONLY AND TR2_STM32_MODBUS_DIAGNOSTIC)
    message(FATAL_ERROR "Commissioning probe and diagnostic modes are exclusive")
endif()
option(TR2_STM32_RS485_TX_QUALIFIED "Enable DE/TX only for an explicitly qualified RS485 board" OFF)
option(TR2_STM32_IRQ_DIAGNOSTICS "Passive RAM IRQ/core observer for isolated diagnostics" OFF)
option(TR2_STM32_MODBUS_DIAGNOSTIC "Isolated B1/1000 diagnostic, bypassing media and production boot" OFF)
if(TR2_STM32_IRQ_DIAGNOSTICS AND NOT TR2_STM32_MODBUS_DIAGNOSTIC AND NOT TR2_STM32_PRODUCTION)
    message(FATAL_ERROR "IRQ observer requires isolated diagnostic or dedicated production mode")
endif()
set(TR2_STM32_DIAGNOSTIC_UNIT_ID "0" CACHE STRING "Explicit bench-only diagnostic address (1..247); not production provisioning")
if(TR2_STM32_MODBUS_DIAGNOSTIC)
    if(NOT TR2_STM32_DIAGNOSTIC_UNIT_ID MATCHES "^[1-9][0-9]*$" OR
       TR2_STM32_DIAGNOSTIC_UNIT_ID GREATER 247)
        message(FATAL_ERROR "Diagnostic mode requires an explicit unit ID in 1..247")
    endif()
endif()

option(TR2_STM32_FIFO_CAPTURE "Bounded RAM-only FIFO diagnostic" OFF)
option(TR2_STM32_ACQUISITION_CAPTURE "Bounded RAM-only IIS3DWB capture, no media or transport" OFF)
option(TR2_STM32_PRODUCTION "Dedicated production board composition, no harness" OFF)
option(TR2_STM32_MANUFACTURING "Explicit one-shot off-boot FRAM provisioning" OFF)
option(TR2_STM32_PRODUCTION_SILENCE_TEST "Single bounded qualification silence window" OFF)
set(TR2_COMMISSIONING_MANIFEST "" CACHE FILEPATH "Reviewed commissioning manifest")
set(TR2_COMMISSIONING_REGISTRY "" CACHE FILEPATH "Authoritative project registry")
if(TR2_STM32_PRODUCTION OR TR2_STM32_MANUFACTURING)
    if(TR2_STM32_COMMISSIONING_READONLY OR TR2_STM32_MODBUS_DIAGNOSTIC OR
       (TR2_STM32_PRODUCTION AND TR2_STM32_MANUFACTURING))
        message(FATAL_ERROR "Production, manufacturing, probe and diagnostic are exclusive")
    endif()
    if(TR2_STM32_PRODUCTION AND NOT TR2_STM32_RS485_TX_QUALIFIED)
        message(FATAL_ERROR "Production requires qualified RS485 transport")
    endif()
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    execute_process(COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/generate_commissioning_identity.py
        ${TR2_COMMISSIONING_MANIFEST} ${TR2_COMMISSIONING_REGISTRY}
        ${CMAKE_CURRENT_BINARY_DIR}/generated RESULT_VARIABLE generated_result)
    if(NOT generated_result EQUAL 0)
        message(FATAL_ERROR "Commissioning authority generation failed")
    endif()
endif()
if(TR2_STM32_PRODUCTION_SILENCE_TEST AND NOT TR2_STM32_PRODUCTION)
    message(FATAL_ERROR "Silence window requires dedicated production mode")
endif()

if(TR2_STM32_ACQUISITION_CAPTURE AND (TR2_STM32_PRODUCTION OR
    TR2_STM32_MANUFACTURING OR TR2_STM32_COMMISSIONING_READONLY OR
    TR2_STM32_MODBUS_DIAGNOSTIC OR TR2_STM32_RS485_TX_QUALIFIED OR
    TR2_STM32_IRQ_DIAGNOSTICS))
    message(FATAL_ERROR "Acquisition capture is isolated: no other profile or TX")
endif()

if(TR2_STM32_FIFO_CAPTURE AND (TR2_STM32_ACQUISITION_CAPTURE OR
    TR2_STM32_PRODUCTION OR TR2_STM32_MANUFACTURING OR
    TR2_STM32_COMMISSIONING_READONLY OR TR2_STM32_MODBUS_DIAGNOSTIC OR
    TR2_STM32_RS485_TX_QUALIFIED OR TR2_STM32_IRQ_DIAGNOSTICS))
    message(FATAL_ERROR "FIFO capture is isolated from all other profiles and TX")
endif()

set(TR2_FIRMWARE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(CMSIS_CORE_DIR ${STM32CUBE_U5_ROOT}/Drivers/CMSIS/Include)
set(CMSIS_DEVICE_DIR ${STM32CUBE_U5_ROOT}/Drivers/CMSIS/Device/ST/STM32U5xx)
set(HAL_DIR ${STM32CUBE_U5_ROOT}/Drivers/STM32U5xx_HAL_Driver)

foreach(required_path
    ${CMSIS_CORE_DIR}/core_cm33.h
    ${CMSIS_DEVICE_DIR}/Include/stm32u575xx.h
    ${CMSIS_DEVICE_DIR}/Source/Templates/gcc/startup_stm32u575xx.s
    ${CMSIS_DEVICE_DIR}/Source/Templates/system_stm32u5xx.c
    ${HAL_DIR}/Inc/stm32u5xx_hal.h
    ${HAL_DIR}/Src/stm32u5xx_hal.c
    ${HAL_DIR}/Src/stm32u5xx_hal_dma.c
    ${HAL_DIR}/Src/stm32u5xx_hal_spi.c
    ${HAL_DIR}/Src/stm32u5xx_hal_rtc.c
    ${HAL_DIR}/Src/stm32u5xx_hal_rtc_ex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_sd.c
    ${HAL_DIR}/Src/stm32u5xx_ll_sdmmc.c
    ${HAL_DIR}/Src/stm32u5xx_hal_uart.c
    ${HAL_DIR}/Src/stm32u5xx_hal_uart_ex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_tim.c
    ${HAL_DIR}/Src/stm32u5xx_hal_tim_ex.c
)
    if(NOT EXISTS ${required_path})
        message(FATAL_ERROR "STM32CubeU5 dependency missing: ${required_path}. Clone STM32CubeU5 recursively and select v1.9.0.")
    endif()
endforeach()

file(GLOB_RECURSE TR2_CORE_SOURCES CONFIGURE_DEPENDS
    "${TR2_FIRMWARE_ROOT}/src/*.c"
)

if(NOT TR2_CORE_SOURCES)
    message(FATAL_ERROR "P12-A found no portable TR2 core sources under ${TR2_FIRMWARE_ROOT}/src")
endif()

add_library(tr2_core STATIC
    ${TR2_CORE_SOURCES}
)

target_include_directories(tr2_core
    PUBLIC
        ${TR2_FIRMWARE_ROOT}/include
)

target_compile_options(tr2_core PRIVATE
    -mcpu=cortex-m33
    -mthumb
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

add_executable(tr2_stm32_p11c
    main.c
    stm32_fram_storage.c
    stm32_iis3dwb_vibration_source.c
    stm32_iis3dwb_capture.c
    stm32_iis3dwb_fifo_capture.c
    stm32_modbus_application.c
    stm32_modbus_diagnostic.c
    stm32_production_runtime.c
    stm32_rs485_rx_gpio.c
    stm32_serial_transport.c
    stm32_runtime_platform.c
    stm32_sdmmc_bulk_media.c
    stm32u5xx_it.c
    ${CMSIS_DEVICE_DIR}/Source/Templates/gcc/startup_stm32u575xx.s
    ${CMSIS_DEVICE_DIR}/Source/Templates/system_stm32u5xx.c
    ${HAL_DIR}/Src/stm32u5xx_hal.c
    ${HAL_DIR}/Src/stm32u5xx_hal_cortex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_dma.c
    ${HAL_DIR}/Src/stm32u5xx_hal_gpio.c
    ${HAL_DIR}/Src/stm32u5xx_hal_pwr.c
    ${HAL_DIR}/Src/stm32u5xx_hal_pwr_ex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_rcc.c
    ${HAL_DIR}/Src/stm32u5xx_hal_rcc_ex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_rtc.c
    ${HAL_DIR}/Src/stm32u5xx_hal_rtc_ex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_sd.c
    ${HAL_DIR}/Src/stm32u5xx_ll_sdmmc.c
    ${HAL_DIR}/Src/stm32u5xx_hal_spi.c
    ${HAL_DIR}/Src/stm32u5xx_hal_uart.c
    ${HAL_DIR}/Src/stm32u5xx_hal_uart_ex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_tim.c
    ${HAL_DIR}/Src/stm32u5xx_hal_tim_ex.c
)

if(TR2_STM32_PRODUCTION OR TR2_STM32_MANUFACTURING)
    target_sources(tr2_stm32_p11c PRIVATE stm32_production_board.c)
    target_include_directories(tr2_stm32_p11c PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)
endif()
target_compile_definitions(tr2_stm32_p11c PRIVATE
    TR2_STM32_FIFO_CAPTURE=$<BOOL:${TR2_STM32_FIFO_CAPTURE}>
    TR2_STM32_ACQUISITION_CAPTURE=$<BOOL:${TR2_STM32_ACQUISITION_CAPTURE}>
    TR2_STM32_PRODUCTION=$<BOOL:${TR2_STM32_PRODUCTION}>
    TR2_STM32_MANUFACTURING=$<BOOL:${TR2_STM32_MANUFACTURING}>
    TR2_STM32_PRODUCTION_SILENCE_TEST=$<BOOL:${TR2_STM32_PRODUCTION_SILENCE_TEST}>
    TR2_STM32_COMMISSIONING_READONLY=$<BOOL:${TR2_STM32_COMMISSIONING_READONLY}>
    TR2_STM32_IRQ_DIAGNOSTICS=$<BOOL:${TR2_STM32_IRQ_DIAGNOSTICS}>
    TR2_STM32_RS485_TX_QUALIFIED=$<BOOL:${TR2_STM32_RS485_TX_QUALIFIED}>
    TR2_STM32_MODBUS_DIAGNOSTIC=$<BOOL:${TR2_STM32_MODBUS_DIAGNOSTIC}>
    TR2_STM32_DIAGNOSTIC_UNIT_ID=${TR2_STM32_DIAGNOSTIC_UNIT_ID})

target_include_directories(tr2_stm32_p11c PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TR2_FIRMWARE_ROOT}/include
    ${CMSIS_CORE_DIR}
    ${CMSIS_DEVICE_DIR}/Include
    ${HAL_DIR}/Inc
)

target_compile_definitions(tr2_stm32_p11c PRIVATE
    STM32U575xx
    USE_HAL_DRIVER
)

target_compile_options(tr2_stm32_p11c PRIVATE
    -mcpu=cortex-m33
    -mthumb
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

target_link_libraries(tr2_stm32_p11c PRIVATE
    tr2_core
)

target_link_options(tr2_stm32_p11c PRIVATE
    -mcpu=cortex-m33
    -mthumb
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,--gc-sections
    -Wl,--build-id=none
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/tr2_stm32_p11c.map
    -T${CMAKE_CURRENT_SOURCE_DIR}/linker/STM32U575ZITXQ_FLASH.ld
)

set_target_properties(tr2_stm32_p11c PROPERTIES SUFFIX ".elf")

find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(ARM_SIZE arm-none-eabi-size REQUIRED)

add_custom_command(TARGET tr2_stm32_p11c POST_BUILD
    COMMAND ${ARM_OBJCOPY} -O binary $<TARGET_FILE:tr2_stm32_p11c> ${CMAKE_CURRENT_BINARY_DIR}/tr2_stm32_p11c.bin
    COMMAND ${ARM_SIZE} $<TARGET_FILE:tr2_stm32_p11c>
    VERBATIM
)
