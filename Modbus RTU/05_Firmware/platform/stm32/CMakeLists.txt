cmake_minimum_required(VERSION 3.20)
project(tr2_stm32_p11b LANGUAGES C ASM)

if(NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "P11-B STM32 build requires the ARM cross-toolchain. Configure with -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(tr2_stm32_p11b
    main.c
    startup_stm32u575zitxq.s
)

target_compile_options(tr2_stm32_p11b PRIVATE
    -mcpu=cortex-m33
    -mthumb
    -ffreestanding
    -ffunction-sections
    -fdata-sections
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

target_link_options(tr2_stm32_p11b PRIVATE
    -mcpu=cortex-m33
    -mthumb
    -nostdlib
    -Wl,--gc-sections
    -Wl,--build-id=none
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/tr2_stm32_p11b.map
    -T${CMAKE_CURRENT_SOURCE_DIR}/linker/STM32U575ZITXQ_FLASH.ld
)

set_target_properties(tr2_stm32_p11b PROPERTIES SUFFIX ".elf")

find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(ARM_SIZE arm-none-eabi-size REQUIRED)

add_custom_command(TARGET tr2_stm32_p11b POST_BUILD
    COMMAND ${ARM_OBJCOPY} -O binary $<TARGET_FILE:tr2_stm32_p11b> ${CMAKE_CURRENT_BINARY_DIR}/tr2_stm32_p11b.bin
    COMMAND ${ARM_SIZE} $<TARGET_FILE:tr2_stm32_p11b>
    VERBATIM
)
