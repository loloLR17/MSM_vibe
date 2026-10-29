cmake_minimum_required(VERSION 3.20)
project(tr2_d10_link_only LANGUAGES C)
if(NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Use the existing STM32 ARM toolchain; never run on target under R1-DSP-A")
endif()
get_filename_component(FW "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
add_compile_options(-mcpu=cortex-m33 -mthumb -O2 -Wall -Wextra -Wpedantic -Werror
                    -ffunction-sections -fdata-sections -fstack-usage -ffp-contract=off)
foreach(profile baseline q15 candidates)
    add_executable(d10_${profile} arm_link_probe.c)
    target_include_directories(d10_${profile} PRIVATE "${FW}/include")
    if(profile STREQUAL "baseline")
        target_compile_definitions(d10_${profile} PRIVATE D10_BASELINE=1)
    else()
        target_sources(d10_${profile} PRIVATE "${FW}/src/domain/acquisition/fir_decimator10.c")
    endif()
    if(profile STREQUAL "candidates")
        target_compile_definitions(d10_${profile} PRIVATE D10_CANDIDATES=1)
        target_sources(d10_${profile} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../decim10_candidates.c")
    endif()
    # Link-only ELF: intentionally no startup/vector table or device initialization.
    # Reuse the board memory geometry, not a new flash/debug recipe.
    target_link_options(d10_${profile} PRIVATE -mcpu=cortex-m33 -mthumb -nostartfiles
        "-T${FW}/platform/stm32/linker/STM32U575ZITXQ_FLASH.ld"
        --specs=nosys.specs -Wl,--gc-sections "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/d10_${profile}.map")
endforeach()
