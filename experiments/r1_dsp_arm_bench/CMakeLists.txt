cmake_minimum_required(VERSION 3.20)
project(r1_dsp_arm_bench LANGUAGES C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
option(R1_HOST_TESTS "Only simulated control/statistics tests, never cycles proof" OFF)
get_filename_component(FW "${CMAKE_CURRENT_SOURCE_DIR}/../../Modbus RTU/05_Firmware" ABSOLUTE)
set(COMMON src/micro_bench.c "${FW}/src/domain/acquisition/fir_decimator10.c")
if(R1_HOST_TESTS)
 enable_testing()
 add_executable(test_micro_bench tests/test_micro_bench.c ${COMMON})
 target_include_directories(test_micro_bench PRIVATE src "${FW}/include")
 target_compile_options(test_micro_bench PRIVATE -Wall -Wextra -Wpedantic -Werror -UNDEBUG)
 add_test(NAME micro_bench_control_only COMMAND test_micro_bench)
 find_package(Python3 REQUIRED COMPONENTS Interpreter)
 add_test(NAME micro_capture_guards COMMAND ${Python3_EXECUTABLE} -B ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_tools.py)
 add_test(NAME fifo_capture_guards COMMAND ${Python3_EXECUTABLE} -B ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_fifo_tools.py)
 return()
endif()
if(NOT CMAKE_CROSSCOMPILING)
 message(FATAL_ERROR "ARM toolchain required, or R1_HOST_TESTS")
endif()
set(STM32CUBE_U5_ROOT "" CACHE PATH "Qualified CubeU5 v1.9.0 recursive checkout")
set(HAL "${STM32CUBE_U5_ROOT}/Drivers/STM32U5xx_HAL_Driver")
set(DEV "${STM32CUBE_U5_ROOT}/Drivers/CMSIS/Device/ST/STM32U5xx")
set(SDK)
foreach(module hal hal_cortex hal_dma hal_flash hal_flash_ex hal_gpio hal_pwr hal_pwr_ex hal_rcc hal_rcc_ex)
 if(NOT EXISTS "${HAL}/Src/stm32u5xx_${module}.c")
  message(FATAL_ERROR "Missing CubeU5 source ${module}")
 endif()
 list(APPEND SDK "${HAL}/Src/stm32u5xx_${module}.c")
endforeach()
# Reuse the existing ST reference clock routine/config/license. No GPIO main.
add_executable(r1_dsp_micro src/main.c ${COMMON}
 "${CMAKE_CURRENT_SOURCE_DIR}/../gpio_dat123_st_reference/src/clock.c" ${SDK}
 "${DEV}/Source/Templates/gcc/startup_stm32u575xx.s"
 "${DEV}/Source/Templates/system_stm32u5xx.c")
target_include_directories(r1_dsp_micro PRIVATE src "${FW}/include"
 "${CMAKE_CURRENT_SOURCE_DIR}/../gpio_dat123_st_reference/src"
 "${HAL}/Inc" "${DEV}/Include" "${STM32CUBE_U5_ROOT}/Drivers/CMSIS/Include")
target_compile_definitions(r1_dsp_micro PRIVATE STM32U575xx USE_HAL_DRIVER)
target_compile_options(r1_dsp_micro PRIVATE -mcpu=cortex-m33 -mthumb -O2 -g3 -Wall -Wextra -Werror -ffunction-sections -fdata-sections -fstack-usage)
target_link_options(r1_dsp_micro PRIVATE -mcpu=cortex-m33 -mthumb --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections -Wl,--build-id=none
 "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/r1_dsp_micro.map"
 "-T${FW}/platform/stm32/linker/STM32U575ZITXQ_FLASH.ld")
set_target_properties(r1_dsp_micro PROPERTIES SUFFIX .elf)
find_program(OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(SIZE arm-none-eabi-size REQUIRED)
add_custom_command(TARGET r1_dsp_micro POST_BUILD COMMAND ${OBJCOPY} -O binary $<TARGET_FILE:r1_dsp_micro> ${CMAKE_CURRENT_BINARY_DIR}/r1_dsp_micro.bin COMMAND ${SIZE} $<TARGET_FILE:r1_dsp_micro>)

# Separately named sensor image: same FIR source; qualified second-board SPI3.
set(STREF "${CMAKE_CURRENT_SOURCE_DIR}/../iis3dwb_st_reference")
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(OBSERVED "${CMAKE_CURRENT_BINARY_DIR}/generated/stm32u5xx_hal_spi_observed.c")
execute_process(COMMAND ${Python3_EXECUTABLE} -B "${STREF}/tools/instrument_hal.py"
 "${HAL}/Src/stm32u5xx_hal_spi.c" "${OBSERVED}" RESULT_VARIABLE OBS_RESULT)
if(NOT OBS_RESULT EQUAL 0)
 message(FATAL_ERROR "Qualified HAL fingerprint/instrumentation refused")
endif()
add_executable(r1_dsp_fifo fifo/main.c "${FW}/src/domain/acquisition/fir_decimator10.c"
 "${FW}/platform/stm32/fifo_event_time.c" "${FW}/src/application/live_metrology.c"
 "${FW}/src/application/live_dynamic.c" "${FW}/src/domain/acquisition/window_statistics.c"
 "${FW}/src/domain/supervision/indicator_calculator.c"
 "${STREF}/src/platform.c" "${STREF}/src/spi_diagnostic.c"
 "${STREF}/src/fifo_live.c" "${STREF}/src/fifo_window.c" "${STREF}/vendor/st/iis3dwb_reg.c"
 "${OBSERVED}" ${SDK} "${DEV}/Source/Templates/gcc/startup_stm32u575xx.s" "${DEV}/Source/Templates/system_stm32u5xx.c")
target_include_directories(r1_dsp_fifo PRIVATE "${STREF}/src" "${STREF}/vendor/st"
 "${FW}/include" "${FW}/platform/stm32" "${HAL}/Inc" "${DEV}/Include" "${STM32CUBE_U5_ROOT}/Drivers/CMSIS/Include")
target_compile_definitions(r1_dsp_fifo PRIVATE STM32U575xx USE_HAL_DRIVER BENCH_CORE_MHZ=160 BENCH_SPI_DIVIDER=4 BENCH_SPI_MODE3=1 BENCH_DUPLEX=0)
target_compile_options(r1_dsp_fifo PRIVATE -mcpu=cortex-m33 -mthumb -O2 -g3 -Wall -Wextra -Werror -ffunction-sections -fdata-sections -fstack-usage)
target_link_options(r1_dsp_fifo PRIVATE -mcpu=cortex-m33 -mthumb --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections -Wl,--build-id=none "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/r1_dsp_fifo.map" "-T${FW}/platform/stm32/linker/STM32U575ZITXQ_FLASH.ld")
set_target_properties(r1_dsp_fifo PROPERTIES SUFFIX .elf)
add_custom_command(TARGET r1_dsp_fifo POST_BUILD COMMAND ${OBJCOPY} -O binary $<TARGET_FILE:r1_dsp_fifo> ${CMAKE_CURRENT_BINARY_DIR}/r1_dsp_fifo.bin COMMAND ${SIZE} $<TARGET_FILE:r1_dsp_fifo>)
