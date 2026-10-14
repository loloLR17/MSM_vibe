cmake_minimum_required(VERSION 3.20)
project(sdmmc1_firstfault LANGUAGES C ASM)
set(REFERENCE_ROOT "" CACHE PATH "Pinned unchanged VILLO88/NUCLEOU575 checkout")
set(FF_PROFILE "" CACHE STRING "Explicit profile 1 or 4; 4 only after physical A PASS")
if(NOT FF_PROFILE MATCHES "^(1|4)$")
 message(FATAL_ERROR "Explicit FF_PROFILE=1 or 4 required")
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(OVERLAY ${CMAKE_CURRENT_BINARY_DIR}/overlay)
execute_process(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate.py
 --reference ${REFERENCE_ROOT} --out ${OVERLAY} COMMAND_ERROR_IS_FATAL ANY)
set(HAL ${REFERENCE_ROOT}/Drivers/STM32U5xx_HAL_Driver)
set(CMSIS ${REFERENCE_ROOT}/Drivers/CMSIS)
file(GLOB HAL_SOURCES ${HAL}/Src/*.c)
list(REMOVE_ITEM HAL_SOURCES ${HAL}/Src/stm32u5xx_hal_sd.c ${HAL}/Src/stm32u5xx_ll_sdmmc.c)
add_executable(sdmmc1_firstfault probe.c ${OVERLAY}/main.c ${OVERLAY}/stm32u5xx_it.c
 ${OVERLAY}/stm32u5xx_hal_sd.c ${OVERLAY}/stm32u5xx_ll_sdmmc.c ${HAL_SOURCES}
 ${REFERENCE_ROOT}/Core/Src/stm32u5xx_hal_msp.c
 ${REFERENCE_ROOT}/Core/Src/stm32u5xx_hal_timebase_tim.c
 ${REFERENCE_ROOT}/Core/Src/system_stm32u5xx.c
 ${REFERENCE_ROOT}/STM32CubeIDE/Application/User/Startup/startup_stm32u575zitxq.s)
target_include_directories(sdmmc1_firstfault PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
 ${REFERENCE_ROOT}/Core/Inc ${HAL}/Inc ${HAL}/Inc/Legacy ${CMSIS}/Include ${CMSIS}/Device/ST/STM32U5xx/Include)
target_compile_definitions(sdmmc1_firstfault PRIVATE USE_HAL_DRIVER STM32U575xx FF_PROFILE=${FF_PROFILE})
target_compile_options(sdmmc1_firstfault PRIVATE -mcpu=cortex-m33 -mthumb -mfpu=fpv5-sp-d16
 -mfloat-abi=hard -Os -g3 -ffunction-sections -fdata-sections -Wall -Wextra)
target_link_options(sdmmc1_firstfault PRIVATE -mcpu=cortex-m33 -mthumb -mfpu=fpv5-sp-d16
 -mfloat-abi=hard --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections -Wl,--build-id=none
 -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/sdmmc1_firstfault.map
 -T${REFERENCE_ROOT}/STM32CubeIDE/STM32U575ZITXQ_FLASH.ld)
set_target_properties(sdmmc1_firstfault PROPERTIES SUFFIX .elf)
find_program(OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(SIZE arm-none-eabi-size REQUIRED)
add_custom_command(TARGET sdmmc1_firstfault POST_BUILD
 COMMAND ${OBJCOPY} -O binary $<TARGET_FILE:sdmmc1_firstfault> ${CMAKE_CURRENT_BINARY_DIR}/sdmmc1_firstfault.bin
 COMMAND ${SIZE} $<TARGET_FILE:sdmmc1_firstfault>)
