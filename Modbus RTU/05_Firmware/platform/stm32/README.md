# TR2 STM32 target — P11-C CMSIS/HAL bring-up

## Scope

P11-C turns the P11-B bare-metal skeleton into the first real `NUCLEO-U575ZI-Q / STM32U575ZIT6Q` bring-up image.

This slice deliberately remains below the TR2 portable core. It does not link `tr2_core`, does not instantiate any TR2 service and does not configure SPI, SDMMC, LPUART, RTC, GPDMA or IWDG.

The purpose is limited to proving the physical-target software foundation:

- Arm GNU cross-toolchain;
- official STM32U5 CMSIS Device startup and vector table;
- official `system_stm32u5xx.c`;
- minimal STM32U5 HAL subset;
- 160 MHz system clock from MSI + PLL;
- Nucleo power supply configuration;
- SysTick HAL time base;
- visible board bring-up through LED1 (green, PC7).

## Validation state

The P11-C software side is cross-build validated with STM32CubeU5 v1.9.0 and Arm GNU 12.2.1.

Hardware validation is explicitly pending until a NUCLEO-U575ZI-Q is available. Until then, no claim is made about real silicon boot, 160 MHz clock operation, SysTick timing, SMPS configuration, LED1 toggling or SWD after programming.

## STM32CubeU5 dependency

P11-C is validated against **STM32CubeU5 v1.9.0**.

The dependency is intentionally external to the TR2 repository. Do not vendor HAL/CMSIS into `tr2_core` or `include/tr2/platform`.

Example installation beside the TR2 repository:

```sh
git clone --recursive --branch v1.9.0 \
    https://github.com/STMicroelectronics/STM32CubeU5.git
```

A recursive checkout is required because STM32CubeU5 publishes CMSIS Device and HAL as submodules.

## Reproducible cross-build

From `Modbus RTU/05_Firmware`, use the P11-D helper script:

```sh
STM32CUBE_U5_ROOT=/absolute/path/to/STM32CubeU5 \
    ./platform/stm32/tr2_build_stm32.sh
```

The Cube root may alternatively be passed as the first argument:

```sh
./platform/stm32/tr2_build_stm32.sh /absolute/path/to/STM32CubeU5
```

The script checks the required host tools and CubeU5 files, performs a clean Ninja configuration/build using the STM32 toolchain file, and verifies the expected artifacts.

Equivalent manual commands from `Modbus RTU/05_Firmware`:

```sh
cmake -G Ninja \
      -S platform/stm32 \
      -B build-stm32-p11c \
      -DCMAKE_TOOLCHAIN_FILE="$PWD/platform/stm32/cmake/arm-none-eabi-gcc.cmake" \
      -DSTM32CUBE_U5_ROOT=/absolute/path/to/STM32CubeU5
cmake --build build-stm32-p11c
```

Expected artifacts:

- `tr2_stm32_p11c.elf`
- `tr2_stm32_p11c.bin`
- `tr2_stm32_p11c.map`

## Hardware acceptance for this slice

When the target board becomes available, flash `tr2_stm32_p11c.bin` at `0x08000000` using ST-LINK / STM32CubeProgrammer.

Expected observable result after reset:

- LED1 (green, PC7) toggles every 250 ms;
- SWD remains available;
- no TR2 peripheral or Modbus behavior is expected yet.

A failure to blink is a target bring-up failure and must be resolved before introducing any TR2 peripheral driver.

## Architectural boundary

The dependency direction remains:

```text
tr2_core
    ↓
include/tr2/platform
    ↓
platform/stm32
    ↓
STM32CubeU5 CMSIS / HAL
```

P11-C adds only the bottom layer. No STM32 header may be introduced into `src/` or generic `include/tr2/platform/` contracts.

## Deferred work

Explicitly deferred after P11-C:

- IIS3DWB / SPI1 / EXTI / GPDMA;
- MB85RS2MTA and W25Q64JV / SPI2;
- SDMMC1 bulk storage;
- ADM2587E / LPUART1 / DE-/RE;
- RTC/LSE/VBAT service binding;
- IWDG;
- connection of the generic TR2 platform contracts to STM32 implementations.
