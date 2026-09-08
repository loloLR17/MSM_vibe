cmake_minimum_required(VERSION 3.20)
project(tr2_stm32_p11c LANGUAGES C ASM)

if(NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "P11-C STM32 build requires the ARM cross-toolchain. Configure with -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(STM32CUBE_U5_ROOT "" CACHE PATH "Path to a recursive STM32CubeU5 checkout (P11-C validated against v1.9.0)")
if(NOT STM32CUBE_U5_ROOT)
    message(FATAL_ERROR "Set -DSTM32CUBE_U5_ROOT=/path/to/STM32CubeU5 (recursive checkout, v1.9.0)")
endif()

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
)
    if(NOT EXISTS ${required_path})
        message(FATAL_ERROR "STM32CubeU5 dependency missing: ${required_path}. Clone STM32CubeU5 recursively and select v1.9.0.")
    endif()
endforeach()

add_executable(tr2_stm32_p11c
    main.c
    stm32u5xx_it.c
    ${CMSIS_DEVICE_DIR}/Source/Templates/gcc/startup_stm32u575xx.s
    ${CMSIS_DEVICE_DIR}/Source/Templates/system_stm32u5xx.c
    ${HAL_DIR}/Src/stm32u5xx_hal.c
    ${HAL_DIR}/Src/stm32u5xx_hal_cortex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_gpio.c
    ${HAL_DIR}/Src/stm32u5xx_hal_pwr.c
    ${HAL_DIR}/Src/stm32u5xx_hal_pwr_ex.c
    ${HAL_DIR}/Src/stm32u5xx_hal_rcc.c
    ${HAL_DIR}/Src/stm32u5xx_hal_rcc_ex.c
)

target_include_directories(tr2_stm32_p11c PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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
