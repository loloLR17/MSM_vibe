# TR2 STM32 target — P11-B minimal skeleton

## Scope

This directory is the first physical-target build boundary for `NUCLEO-U575ZI-Q / STM32U575ZIT6Q`.

P11-B intentionally contains no TR2 business-service wiring and no peripheral driver. It proves only that a minimal Cortex-M33 image can be cross-compiled and linked for the retained MCU without changing the Host build.

The portable core remains in `src/` and generic contracts remain in `include/tr2/platform/`. STM32-specific dependencies must remain below `platform/stm32/`.

## Required toolchain

Install an Arm GNU Embedded toolchain exposing at least:

- `arm-none-eabi-gcc`
- `arm-none-eabi-objcopy`
- `arm-none-eabi-size`
- CMake >= 3.20

No STM32CubeU5 package is required by P11-B because HAL/CMSIS integration is deliberately deferred.

## Build

From `Modbus RTU/05_Firmware`:

```sh
cmake -S platform/stm32 \
      -B build-stm32-p11b \
      -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake
cmake --build build-stm32-p11b
```

Expected artifacts:

- `tr2_stm32_p11b.elf`
- `tr2_stm32_p11b.bin`
- `tr2_stm32_p11b.map`

## Memory contract used by this bootstrap

The linker script uses the STM32U575ZIT6Q flash configuration retained for the prototype:

- Flash: `0x08000000`, 2048 KiB
- main RAM region: `0x20000000`, 768 KiB

SRAM4 and backup SRAM are intentionally not used by this minimal image.

## Non-goals of P11-B

P11-B does not yet configure clocks, GPIO, SPI, SDMMC, LPUART, RTC, GPDMA, watchdog, HAL, CMSIS device startup, SysTick, or any TR2 domain/application service.

The next physical-target slices must introduce those dependencies explicitly and keep them isolated from `tr2_core`.
