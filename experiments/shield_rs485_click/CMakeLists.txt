cmake_minimum_required(VERSION 3.20)
project(shield_rs485_click LANGUAGES C ASM)
set(CMAKE_C_STANDARD 11)
get_filename_component(FW "${CMAKE_CURRENT_SOURCE_DIR}/../../Modbus RTU/05_Firmware" ABSOLUTE)
option(CLICK_HOST_TESTS "Only new isolated bench policy tests" OFF)
if(CLICK_HOST_TESTS)
  enable_testing()
  add_executable(click_policy_test tests/policy_test.c src/policy.c "${FW}/src/modbus/codec.c")
  target_include_directories(click_policy_test PRIVATE src "${FW}/include")
  target_compile_options(click_policy_test PRIVATE -Wall -Wextra -Werror -UNDEBUG)
  add_test(NAME click_policy COMMAND click_policy_test)
  return()
endif()
if(NOT CMAKE_CROSSCOMPILING)
  message(FATAL_ERROR "Explicit ARM toolchain required")
endif()
set(CLICK_BOARD "" CACHE STRING "Explicit target: 001 or002")
if(CLICK_BOARD STREQUAL "001")
  set(UID0 0x004A003D)
elseif(CLICK_BOARD STREQUAL "002")
  set(UID0 0x001B0046)
else()
  message(FATAL_ERROR "Explicit CLICK_BOARD=001 or002 required")
endif()
set(STM32CUBE_U5_ROOT "" CACHE PATH "Qualified CubeU5 SDK")
set(HAL "${STM32CUBE_U5_ROOT}/Drivers/STM32U5xx_HAL_Driver")
set(DEV "${STM32CUBE_U5_ROOT}/Drivers/CMSIS/Device/ST/STM32U5xx")
add_compile_options(-mcpu=cortex-m33 -mthumb -O2 -g3 -ffunction-sections -fdata-sections)
add_subdirectory("${FW}" core EXCLUDE_FROM_ALL)
set(SDK)
foreach(module hal hal_cortex hal_dma hal_flash hal_flash_ex hal_gpio hal_pwr hal_pwr_ex hal_rcc hal_rcc_ex hal_uart hal_uart_ex hal_tim hal_tim_ex)
  list(APPEND SDK "${HAL}/Src/stm32u5xx_${module}.c")
endforeach()
add_executable(click_bench src/main.c src/policy.c
  "${FW}/platform/stm32/stm32_serial_transport.c"
  "${FW}/platform/stm32/stm32_rs485_rx_gpio.c"
  "${FW}/platform/stm32/stm32_modbus_diagnostic.c"
  ${SDK} "${DEV}/Source/Templates/gcc/startup_stm32u575xx.s"
  "${DEV}/Source/Templates/system_stm32u5xx.c")
target_include_directories(click_bench PRIVATE src "${FW}/include" "${FW}/platform/stm32"
  "${HAL}/Inc" "${DEV}/Include" "${STM32CUBE_U5_ROOT}/Drivers/CMSIS/Include")
# This is an experimental TX-enabled test image, not a declaration of qualification.
# Boot and each TX stage remain gated by one-shot RAM arm and bounded credits.
target_compile_definitions(click_bench PRIVATE STM32U575xx USE_HAL_DRIVER CLICK_UID0=${UID0}
  TR2_STM32_IRQ_DIAGNOSTICS=1 TR2_STM32_RS485_TX_QUALIFIED=1)
target_compile_options(click_bench PRIVATE -Wall -Wextra -Werror)
target_link_libraries(click_bench PRIVATE tr2_core m)
target_link_options(click_bench PRIVATE -mcpu=cortex-m33 -mthumb --specs=nano.specs --specs=nosys.specs
  -Wl,--gc-sections -Wl,--build-id=none -Wl,--wrap=HAL_GPIO_WritePin
  "-Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/click_bench.map"
  "-T${FW}/platform/stm32/linker/STM32U575ZITXQ_FLASH.ld")
set_target_properties(click_bench PROPERTIES SUFFIX .elf)
find_program(OBJCOPY arm-none-eabi-objcopy REQUIRED)
add_custom_command(TARGET click_bench POST_BUILD COMMAND ${OBJCOPY} -O binary
  $<TARGET_FILE:click_bench> ${CMAKE_CURRENT_BINARY_DIR}/click_bench.bin)
