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

# A04-01: opt-in cumulative recipe; never overwrite conflicting cache inputs.
set(TR2_STM32_BUILD_PROFILE "" CACHE STRING "Explicit recipe: CUMULATIVE or individual specialized flags")
set_property(CACHE TR2_STM32_BUILD_PROFILE PROPERTY STRINGS "" CUMULATIVE)
if(TR2_STM32_BUILD_PROFILE STREQUAL "CUMULATIVE")
    foreach(flag PRODUCTION RS485_TX_QUALIFIED IRQ_DIAGNOSTICS FIFO_COOPERATIVE)
        if(DEFINED TR2_STM32_${flag} AND NOT TR2_STM32_${flag})
            message(FATAL_ERROR "CUMULATIVE conflicts with TR2_STM32_${flag}=OFF; use a fresh build directory")
        endif()
        set(TR2_STM32_${flag} ON CACHE BOOL "Required by CUMULATIVE")
    endforeach()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "Canonical cumulative build type" FORCE)
    elseif(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(FATAL_ERROR "CUMULATIVE requires canonical Debug build type")
    endif()
elseif(NOT TR2_STM32_BUILD_PROFILE STREQUAL "")
    message(FATAL_ERROR "Unknown TR2_STM32_BUILD_PROFILE")
endif()
option(TR2_STM32_R4M_MAINTENANCE "Isolated one-shot R4M qualification maintenance; requires mission authorization" OFF)
if(TR2_STM32_R4M_MAINTENANCE AND NOT TR2_STM32_R4_BENCH)
 message(FATAL_ERROR "R4M requires explicit R4_BENCH=ON")
endif()
option(TR2_STM32_R4P_COEXISTENCE "R4P single isolated journal transition and bounded physical coexistence" OFF)
if(TR2_STM32_R4P_COEXISTENCE AND (NOT TR2_STM32_R4_BENCH OR NOT TR2_STM32_R4M_MAINTENANCE OR TR2_STM32_R4D_DIAGNOSTIC))
 message(FATAL_ERROR "R4P requires R4_BENCH and R4M_MAINTENANCE, without R4D")
endif()
option(TR2_STM32_R4U_RUNTIME "R4U opt-in LIVE runtime qualification, NON-PRODUCTION" OFF)
if(TR2_STM32_R4U_RUNTIME AND (NOT TR2_STM32_R4S_BENCH OR TR2_STM32_R4T_RECOVERY))
 message(FATAL_ERROR "R4U requires explicit R4S/R4R recipe and forbids R4T retirement")
endif()
option(TR2_STM32_R4T_RECOVERY "R4T exact monotone retirement and physical B3; NON-PRODUCTION" OFF)
if(TR2_STM32_R4T_RECOVERY AND NOT TR2_STM32_R4S_BENCH)
 message(FATAL_ERROR "R4T requires explicit R4S/R4R hardware recipe")
endif()
option(TR2_STM32_R4S_BENCH "R4S readonly volatile service diagnostic" OFF)
if(TR2_STM32_R4S_BENCH AND NOT TR2_STM32_R4R_BENCH)
 message(FATAL_ERROR "R4S requires explicit R4R hardware recipe")
endif()
option(TR2_STM32_R4R_BENCH "R4R physical sensor time qualification; no normal ingress" OFF)
if(TR2_STM32_R4R_BENCH AND (NOT TR2_STM32_R4P_COEXISTENCE OR NOT TR2_STM32_R4M_MAINTENANCE OR NOT TR2_STM32_R4_BENCH OR TR2_STM32_R4D_DIAGNOSTIC OR TR2_STM32_R4Q_BENCH))
 message(FATAL_ERROR "R4R requires explicit R4/R4M/R4P, excludes R4D/R4Q")
endif()
option(TR2_STM32_R4Q_BENCH "R4Q read-only FRAM and RAM RTU loopback, sensor stopped" OFF)
if(TR2_STM32_R4Q_BENCH AND (NOT TR2_STM32_R4P_COEXISTENCE OR NOT TR2_STM32_R4M_MAINTENANCE OR NOT TR2_STM32_R4_BENCH OR TR2_STM32_R4D_DIAGNOSTIC))
 message(FATAL_ERROR "R4Q readonly bench requires explicit R4/R4M/R4P and no R4D; no normal ingress")
endif()
option(TR2_STM32_R4D_DIAGNOSTIC "R4D volatile-only sensor timing diagnostic; never R2 admission or FRAM writes" OFF)
if(TR2_STM32_R4D_DIAGNOSTIC AND (NOT TR2_STM32_R4_BENCH OR TR2_STM32_R4M_MAINTENANCE))
 message(FATAL_ERROR "R4D requires R4_BENCH=ON and R4M_MAINTENANCE=OFF")
endif()
option(TR2_STM32_R4_BENCH "R4 isolated RAM-triggered cooperative qualification; never production" OFF)
if(TR2_STM32_R4_BENCH AND NOT TR2_STM32_BUILD_PROFILE STREQUAL "CUMULATIVE")
 message(FATAL_ERROR "R4 bench requires the qualified cumulative hardware recipe")
endif()
option(TR2_STM32_LEGACY_SD_HARNESS "NON-PRODUCTION: historical harness writes microSD at boot; sacrificial media only" OFF)

option(TR2_STM32_STORAGE_QUALIFICATION "Isolated M016 explicitly allocated storage qualification; RAM-triggered writes only" OFF)
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

option(TR2_STM32_FIFO_COOPERATIVE "M015-V cooperative production FIFO" OFF)
set(TR2_STM32_FIFO_SPI_MHZ 4 CACHE STRING "Independent HSI16 SPI3: 4 or 8MHz")
if(NOT TR2_STM32_FIFO_SPI_MHZ MATCHES "^(4|8)$")
 message(FATAL_ERROR "SPI3 qualification is bounded to4/8MHz")
endif()
if(TR2_STM32_FIFO_COOPERATIVE AND (NOT TR2_STM32_PRODUCTION OR NOT TR2_STM32_IRQ_DIAGNOSTICS OR TR2_STM32_ST_FIFO_WINDOW OR TR2_STM32_PRODUCTION_SILENCE_TEST))
 message(FATAL_ERROR "Cooperative FIFO requires production and observer, excludes other profiles")
endif()
option(TR2_STM32_ST_FIFO_WINDOW "M015-U RAM-only official ST4096 bounded profile" OFF)
option(TR2_STM32_CONTINUOUS_FIFO "Bounded continuous FIFO4096 RAM qualification, no media" OFF)
option(TR2_STM32_CTRL1_DIFFERENTIAL "Bounded CTRL1 configuration comparison, no acquisition" OFF)
option(TR2_STM32_BURST_CAPTURE "Bounded FIFO bursts128 RAM capture, no continuous stream" OFF)
option(TR2_STM32_ACTIVE_READBACK "Bounded active FIFO status diagnostic, no data drain" OFF)
option(TR2_STM32_REGISTER_PROBE "Read-only bounded SPI register reliability diagnostic" OFF)
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

if(TR2_STM32_REGISTER_PROBE AND (TR2_STM32_FIFO_CAPTURE OR
    TR2_STM32_ACQUISITION_CAPTURE OR TR2_STM32_PRODUCTION OR
    TR2_STM32_MANUFACTURING OR TR2_STM32_COMMISSIONING_READONLY OR
    TR2_STM32_MODBUS_DIAGNOSTIC OR TR2_STM32_RS485_TX_QUALIFIED OR
    TR2_STM32_IRQ_DIAGNOSTICS))
    message(FATAL_ERROR "Register probe is read-only and isolated from all profiles and TX")
endif()

if(TR2_STM32_ACTIVE_READBACK AND (TR2_STM32_REGISTER_PROBE OR
    TR2_STM32_FIFO_CAPTURE OR TR2_STM32_ACQUISITION_CAPTURE OR
    TR2_STM32_PRODUCTION OR TR2_STM32_MANUFACTURING OR
    TR2_STM32_COMMISSIONING_READONLY OR TR2_STM32_MODBUS_DIAGNOSTIC OR
    TR2_STM32_RS485_TX_QUALIFIED OR TR2_STM32_IRQ_DIAGNOSTICS))
    message(FATAL_ERROR "Active FIFO readback is isolated from other profiles and TX")
endif()

if(TR2_STM32_BURST_CAPTURE AND (TR2_STM32_ACTIVE_READBACK OR
    TR2_STM32_REGISTER_PROBE OR TR2_STM32_FIFO_CAPTURE OR
    TR2_STM32_ACQUISITION_CAPTURE OR TR2_STM32_PRODUCTION OR
    TR2_STM32_MANUFACTURING OR TR2_STM32_COMMISSIONING_READONLY OR
    TR2_STM32_MODBUS_DIAGNOSTIC OR TR2_STM32_RS485_TX_QUALIFIED OR TR2_STM32_IRQ_DIAGNOSTICS))
    message(FATAL_ERROR "FIFO burst capture is isolated from all other profiles and TX")
endif()

if(TR2_STM32_CTRL1_DIFFERENTIAL AND (TR2_STM32_BURST_CAPTURE OR
    TR2_STM32_ACTIVE_READBACK OR TR2_STM32_REGISTER_PROBE OR
    TR2_STM32_FIFO_CAPTURE OR TR2_STM32_ACQUISITION_CAPTURE OR
    TR2_STM32_PRODUCTION OR TR2_STM32_MANUFACTURING OR
    TR2_STM32_COMMISSIONING_READONLY OR TR2_STM32_MODBUS_DIAGNOSTIC OR
    TR2_STM32_RS485_TX_QUALIFIED OR TR2_STM32_IRQ_DIAGNOSTICS))
    message(FATAL_ERROR "CTRL1 differential diagnostic is exclusive, no media/TX/acquisition")
endif()

if(TR2_STM32_CONTINUOUS_FIFO AND (TR2_STM32_CTRL1_DIFFERENTIAL OR
    TR2_STM32_BURST_CAPTURE OR TR2_STM32_ACTIVE_READBACK OR
    TR2_STM32_REGISTER_PROBE OR TR2_STM32_FIFO_CAPTURE OR
    TR2_STM32_ACQUISITION_CAPTURE OR TR2_STM32_PRODUCTION OR
    TR2_STM32_MANUFACTURING OR TR2_STM32_COMMISSIONING_READONLY OR
    TR2_STM32_MODBUS_DIAGNOSTIC OR TR2_STM32_RS485_TX_QUALIFIED OR
    TR2_STM32_IRQ_DIAGNOSTICS))
    message(FATAL_ERROR "Continuous FIFO is exclusive, no media/TX/other capture")
endif()

if(TR2_STM32_ST_FIFO_WINDOW)
 foreach(other PRODUCTION MANUFACTURING COMMISSIONING_READONLY MODBUS_DIAGNOSTIC RS485_TX_QUALIFIED IRQ_DIAGNOSTICS CONTINUOUS_FIFO CTRL1_DIFFERENTIAL BURST_CAPTURE ACTIVE_READBACK REGISTER_PROBE FIFO_CAPTURE ACQUISITION_CAPTURE)
  if(TR2_STM32_${other})
   message(FATAL_ERROR "ST FIFO window excludes media, transport and other resource owners")
  endif()
 endforeach()
endif()

if(TR2_STM32_STORAGE_QUALIFICATION AND (TR2_STM32_RS485_TX_QUALIFIED OR
    TR2_STM32_IRQ_DIAGNOSTICS OR TR2_STM32_FIFO_COOPERATIVE))
    message(FATAL_ERROR "STORAGE_QUALIFICATION is isolated: no UART TX/observer/FIFO")
endif()

# Run the established guards first, then reject an implicit generic harness.
set(TR2_SELECTED_MODES "")
foreach(mode PRODUCTION MANUFACTURING COMMISSIONING_READONLY STORAGE_QUALIFICATION MODBUS_DIAGNOSTIC
        ST_FIFO_WINDOW CONTINUOUS_FIFO CTRL1_DIFFERENTIAL BURST_CAPTURE
        ACTIVE_READBACK REGISTER_PROBE FIFO_CAPTURE ACQUISITION_CAPTURE)
    if(TR2_STM32_${mode})
        list(APPEND TR2_SELECTED_MODES ${mode})
    endif()
endforeach()
list(LENGTH TR2_SELECTED_MODES mode_count)
if(TR2_STM32_LEGACY_SD_HARNESS)
    if(mode_count GREATER 0 OR TR2_STM32_RS485_TX_QUALIFIED OR TR2_STM32_IRQ_DIAGNOSTICS)
        message(FATAL_ERROR "LEGACY_SD_HARNESS is exclusive and must not enable qualified TX/observer")
    endif()
    set(TR2_ARTIFACT_NAME "tr2-legacy-SD-WRITES-NON-PRODUCTION")
    message(WARNING "Historical harness: automatic microSD writes at boot; NEVER production")
elseif(NOT mode_count EQUAL 1)
    message(FATAL_ERROR "Select exactly one explicit STM32 mode; implicit generic SD harness is forbidden")
elseif(TR2_STM32_FIFO_COOPERATIVE)
    set(TR2_ARTIFACT_NAME "tr2-cumulative-m014-m015-spi${TR2_STM32_FIFO_SPI_MHZ}")
else()
    string(TOLOWER "${TR2_SELECTED_MODES}" mode_label)
    set(TR2_ARTIFACT_NAME "tr2-special-${mode_label}-NON-CUMULATIVE")
endif()
if(TR2_STM32_R4_BENCH)
 set(TR2_ARTIFACT_NAME "tr2-r4-qualification-NON-PRODUCTION")
 if(TR2_STM32_R4M_MAINTENANCE)
  set(TR2_ARTIFACT_NAME "tr2-r4m-qualification-NON-PRODUCTION")
 endif()
endif()
if(TR2_STM32_R4D_DIAGNOSTIC)
 set(TR2_ARTIFACT_NAME "tr2-r4d-diagnostic-NON-PRODUCTION")
endif()
if(TR2_STM32_R4P_COEXISTENCE)
 set(TR2_ARTIFACT_NAME "tr2-r4p-coexistence-NON-PRODUCTION")
endif()
if(TR2_STM32_R4Q_BENCH)
 set(TR2_ARTIFACT_NAME "tr2-r4q-readonly-rtu-NON-PRODUCTION")
endif()
if(TR2_STM32_R4R_BENCH)
 set(TR2_ARTIFACT_NAME "tr2-r4r-physical-time-NON-PRODUCTION")
endif()
if(TR2_STM32_R4S_BENCH)
 set(TR2_ARTIFACT_NAME "tr2-r4s-readonly-service-NON-PRODUCTION")
endif()
if(TR2_STM32_R4T_RECOVERY)
 set(TR2_ARTIFACT_NAME "tr2-r4t-monotone-recovery-NON-PRODUCTION")
endif()

if(TR2_STM32_R4U_RUNTIME)
 set(TR2_ARTIFACT_NAME "tr2-r4u-live-runtime-NON-PRODUCTION")
endif()

set(profile_description "artifact=${TR2_ARTIFACT_NAME}\nrecipe=${TR2_STM32_BUILD_PROFILE}\nbuild_type=${CMAKE_BUILD_TYPE}\n")
get_cmake_property(cache_variables CACHE_VARIABLES)
list(SORT cache_variables)
foreach(variable IN LISTS cache_variables)
    if(variable MATCHES "^TR2_(STM32_|COMMISSIONING_)")
        string(APPEND profile_description "${variable}=${${variable}}\n")
    endif()
endforeach()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/build-profile.txt" "${profile_description}")
message(STATUS "STM32 artifact: ${TR2_ARTIFACT_NAME}; no hardware qualification implied")

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
    stm32_iis3dwb_register_probe.c
    stm32_iis3dwb_active_readback.c
    stm32_iis3dwb_continuous.c
    stm32_iis3dwb_ctrl1_differential.c
    stm32_iis3dwb_burst_capture.c
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
if(TR2_STM32_STORAGE_QUALIFICATION)
    target_sources(tr2_stm32_p11c PRIVATE stm32_storage_qualification.c)
    # Isolated throughput qualification: normal cumulative flags stay unchanged.
    target_compile_options(tr2_core PRIVATE -O2)
    set_source_files_properties(stm32_storage_qualification.c PROPERTIES COMPILE_OPTIONS "-O2")
endif()

target_compile_definitions(tr2_stm32_p11c PRIVATE
    TR2_STM32_LEGACY_SD_HARNESS=$<BOOL:${TR2_STM32_LEGACY_SD_HARNESS}>
    TR2_STM32_FIFO_COOPERATIVE=$<BOOL:${TR2_STM32_FIFO_COOPERATIVE}>
    TR2_STM32_FIFO_SPI_MHZ=${TR2_STM32_FIFO_SPI_MHZ}
    TR2_STM32_ST_FIFO_WINDOW=$<BOOL:${TR2_STM32_ST_FIFO_WINDOW}>
    TR2_STM32_CONTINUOUS_FIFO=$<BOOL:${TR2_STM32_CONTINUOUS_FIFO}>
    TR2_STM32_CTRL1_DIFFERENTIAL=$<BOOL:${TR2_STM32_CTRL1_DIFFERENTIAL}>
    TR2_STM32_BURST_CAPTURE=$<BOOL:${TR2_STM32_BURST_CAPTURE}>
    TR2_STM32_ACTIVE_READBACK=$<BOOL:${TR2_STM32_ACTIVE_READBACK}>
    TR2_STM32_REGISTER_PROBE=$<BOOL:${TR2_STM32_REGISTER_PROBE}>
    TR2_STM32_FIFO_CAPTURE=$<BOOL:${TR2_STM32_FIFO_CAPTURE}>
    TR2_STM32_ACQUISITION_CAPTURE=$<BOOL:${TR2_STM32_ACQUISITION_CAPTURE}>
    TR2_STM32_PRODUCTION=$<BOOL:${TR2_STM32_PRODUCTION}>
    TR2_STM32_MANUFACTURING=$<BOOL:${TR2_STM32_MANUFACTURING}>
    TR2_STM32_PRODUCTION_SILENCE_TEST=$<BOOL:${TR2_STM32_PRODUCTION_SILENCE_TEST}>
    TR2_STM32_STORAGE_QUALIFICATION=$<BOOL:${TR2_STM32_STORAGE_QUALIFICATION}>
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
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${TR2_ARTIFACT_NAME}.map
    -T${CMAKE_CURRENT_SOURCE_DIR}/linker/STM32U575ZITXQ_FLASH.ld
)

set_target_properties(tr2_stm32_p11c PROPERTIES SUFFIX ".elf" OUTPUT_NAME "${TR2_ARTIFACT_NAME}")

find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(ARM_SIZE arm-none-eabi-size REQUIRED)

add_custom_command(TARGET tr2_stm32_p11c POST_BUILD
    COMMAND ${ARM_OBJCOPY} -O binary $<TARGET_FILE:tr2_stm32_p11c> ${CMAKE_CURRENT_BINARY_DIR}/${TR2_ARTIFACT_NAME}.bin
    COMMAND ${ARM_SIZE} $<TARGET_FILE:tr2_stm32_p11c>
    VERBATIM
)

if(TR2_STM32_ST_FIFO_WINDOW OR TR2_STM32_FIFO_COOPERATIVE)
 set(ST_REF "${CMAKE_CURRENT_SOURCE_DIR}/../../../../experiments/iis3dwb_st_reference")
 find_package(Python3 REQUIRED COMPONENTS Interpreter)
 set(OFFICIAL_PORTED ${CMAKE_CURRENT_BINARY_DIR}/generated/iis3dwb_fifo_ported.c)
 execute_process(COMMAND ${Python3_EXECUTABLE} -B ${ST_REF}/official/generate.py
   ${ST_REF}/vendor/st/examples/iis3dwb_fifo.c ${OFFICIAL_PORTED} RESULT_VARIABLE ported)
 if(NOT ported EQUAL 0)
  message(FATAL_ERROR "Official ST provenance refused")
 endif()
 set(SPI_OBSERVED ${CMAKE_CURRENT_BINARY_DIR}/generated/stm32u5xx_hal_spi_observed.c)
 execute_process(COMMAND ${Python3_EXECUTABLE} -B ${ST_REF}/tools/instrument_hal.py
   ${HAL_DIR}/Src/stm32u5xx_hal_spi.c ${SPI_OBSERVED} RESULT_VARIABLE observed)
 if(NOT observed EQUAL 0)
  message(FATAL_ERROR "HAL provenance refused")
 endif()
 get_target_property(all_sources tr2_stm32_p11c SOURCES)
 list(REMOVE_ITEM all_sources ${HAL_DIR}/Src/stm32u5xx_hal_spi.c)
 set_property(TARGET tr2_stm32_p11c PROPERTY SOURCES ${all_sources})
 target_sources(tr2_stm32_p11c PRIVATE stm32_iis3dwb_st_window.c
   ${ST_REF}/vendor/st/iis3dwb_reg.c
   ${ST_REF}/src/fifo_window.c ${ST_REF}/src/spi_diagnostic.c ${SPI_OBSERVED})
 if(TR2_STM32_FIFO_COOPERATIVE)
  target_sources(tr2_stm32_p11c PRIVATE stm32_fifo_cooperative.c ${ST_REF}/src/fifo_cooperative.c
    stm32_live_acquisition.c live_fifo_acquisition.c ${ST_REF}/src/fifo_live.c)
  target_compile_definitions(tr2_stm32_p11c PRIVATE BENCH_WINDOW_MAX_WORDS=36864)
  set_source_files_properties(stm32_fifo_cooperative.c ${ST_REF}/src/fifo_cooperative.c PROPERTIES COMPILE_OPTIONS "-O2")
 else()
  target_sources(tr2_stm32_p11c PRIVATE ${ST_REF}/official/port.c ${OFFICIAL_PORTED})
 endif()
 target_include_directories(tr2_stm32_p11c PRIVATE ${ST_REF}/src ${ST_REF}/official ${ST_REF}/vendor/st)
 target_compile_definitions(tr2_stm32_p11c PRIVATE BENCH_WINDOW_XYZ=4096 BENCH_DUPLEX=0 OFFICIAL_STATUS10=1)
 set_source_files_properties(${SPI_OBSERVED} ${ST_REF}/src/fifo_window.c PROPERTIES COMPILE_OPTIONS "-O2")
 target_link_options(tr2_stm32_p11c PRIVATE -Wl,-u,_printf_float)
endif()

if(TR2_STM32_R4_BENCH)
 if(TR2_STM32_R4U_RUNTIME)
  target_sources(tr2_stm32_p11c PRIVATE stm32_r4u_runtime.c fifo_event_time.c)
  target_compile_definitions(tr2_stm32_p11c PRIVATE TR2_FIFO_TIME_PROFILE=1)
  set_source_files_properties(stm32_r4u_runtime.c fifo_event_time.c
   "${TR2_FIRMWARE_ROOT}/src/application/live_runtime.c"
   "${TR2_FIRMWARE_ROOT}/src/application/live_metrology.c"
   "${TR2_FIRMWARE_ROOT}/src/domain/acquisition/window_statistics.c"
   "${TR2_FIRMWARE_ROOT}/src/domain/supervision/indicator_calculator.c" PROPERTIES COMPILE_OPTIONS "-O2")
 elseif(TR2_STM32_R4T_RECOVERY)
  target_sources(tr2_stm32_p11c PRIVATE stm32_r4t_recovery.c r4t_maintenance.c fifo_event_time.c)
  target_compile_definitions(tr2_stm32_p11c PRIVATE TR2_FIFO_TIME_PROFILE=1)
  set_source_files_properties(stm32_r4t_recovery.c r4t_maintenance.c fifo_event_time.c
   "${TR2_FIRMWARE_ROOT}/src/application/live_metrology.c"
   "${TR2_FIRMWARE_ROOT}/src/domain/acquisition/window_statistics.c"
   "${TR2_FIRMWARE_ROOT}/src/domain/supervision/indicator_calculator.c" PROPERTIES COMPILE_OPTIONS "-O2")
 elseif(TR2_STM32_R4S_BENCH)
  target_sources(tr2_stm32_p11c PRIVATE stm32_r4s_bench.c fifo_event_time.c)
  target_compile_definitions(tr2_stm32_p11c PRIVATE TR2_FIFO_TIME_PROFILE=1)
  set_source_files_properties(stm32_r4s_bench.c fifo_event_time.c
   "${TR2_FIRMWARE_ROOT}/src/application/live_metrology.c"
   "${TR2_FIRMWARE_ROOT}/src/domain/acquisition/window_statistics.c"
   "${TR2_FIRMWARE_ROOT}/src/domain/supervision/indicator_calculator.c" PROPERTIES COMPILE_OPTIONS "-O2")
 elseif(TR2_STM32_R4R_BENCH)
  target_sources(tr2_stm32_p11c PRIVATE stm32_r4r_bench.c fifo_event_time.c)
  # Qualification candidate after the measured R4R service overrun. No default
  # V1 flags change; algorithms and all deadline/quality guards stay identical.
  set_source_files_properties(stm32_r4r_bench.c fifo_event_time.c
   "${TR2_FIRMWARE_ROOT}/src/application/live_metrology.c" PROPERTIES COMPILE_OPTIONS "-O2")
 elseif(TR2_STM32_R4Q_BENCH)
  target_sources(tr2_stm32_p11c PRIVATE stm32_r4q_bench.c)
 elseif(TR2_STM32_R4D_DIAGNOSTIC)
  target_sources(tr2_stm32_p11c PRIVATE stm32_r4d_probe.c)
 else()
  target_sources(tr2_stm32_p11c PRIVATE stm32_r4_bench.c)
 endif()
 target_compile_definitions(tr2_stm32_p11c PRIVATE TR2_STM32_R4_BENCH=1)
endif()

if(TR2_STM32_R4M_MAINTENANCE)
 target_sources(tr2_stm32_p11c PRIVATE r4m_maintenance.c)
 target_compile_definitions(tr2_stm32_p11c PRIVATE TR2_STM32_R4M_MAINTENANCE=1)
endif()

if(TR2_STM32_R4P_COEXISTENCE)
 target_compile_definitions(tr2_stm32_p11c PRIVATE TR2_STM32_R4P_COEXISTENCE=1)
endif()
