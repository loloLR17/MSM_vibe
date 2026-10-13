# STM32 TR2 — explicit build profiles (A04-01)

The source tree supports cumulative M014/M015 and historical qualification modes.
A build is not a hardware qualification or permission to program a board.
The former implicit P11-C/P12-A harness selection is rejected: that harness can
write microSD automatically at boot. Old `tr2_stm32_p11c.*` files are ambiguous;
do not select them for a new programming operation by filename alone.

## Canonical cumulative M014/M015

From the repository root, with the qualified Linux SDK:

```sh
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
    bash ./tr2_validate.sh --cross-build-only
```

This explicitly calls the build helper with `--profile cumulative`. The helper
selects `TR2_STM32_BUILD_PROFILE=CUMULATIVE`, Debug, production, qualified TX,
IRQ observer, cooperative FIFO, SPI3 4 MHz, and the reviewed M014-H manifest
and attribution registry. It never deletes the build directory. Reuse requires
a matching profile label; conflicting/unlabelled caches require a fresh directory.
The helper verifies ELF/BIN/map and writes `artifact-sha256.txt`.

Equivalent direct configuration for a **fresh** directory (from repository root):

```sh
cmake -G Ninja -S 'Modbus RTU/05_Firmware/platform/stm32' -B /tmp/tr2-cumulative-fresh \
  -DCMAKE_TOOLCHAIN_FILE="$PWD/Modbus RTU/05_Firmware/platform/stm32/cmake/arm-none-eabi-gcc.cmake" \
  -DSTM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  -DTR2_STM32_BUILD_PROFILE=CUMULATIVE \
  -DTR2_STM32_FIFO_SPI_MHZ=4 \
  -DTR2_COMMISSIONING_MANIFEST="$PWD/Modbus RTU/00_gouvernance/COMMISSIONING_M014_H.json" \
  -DTR2_COMMISSIONING_REGISTRY="$PWD/Modbus RTU/00_gouvernance/REGISTRE_ATTRIBUTION_TR2.json"
cmake --build /tmp/tr2-cumulative-fresh
```

Artifacts: `tr2-cumulative-m014-m015-spi4.{elf,bin,map}` and `build-profile.txt`.
The canonical reference BIN is 169380 bytes, SHA-256
`8529630a5a6c9aaa2449fde512b06d6ca16a89a08b7afea2abc9ecca3b75d480`.
Another toolchain/SDK/input or SPI8 selection requires comparison and a mission
covering any necessary qualification; the recipe name alone proves no equivalence.
This profile does not initialize SDMMC or link a usable SD campaign backend.
FRAM/Modbus/FIFO functionality and existing limitations remain unchanged.

## Specialized and historical profiles

Existing individual mode flags remain supported in a fresh CMake directory,
with their existing exclusion, TX and commissioning-authority guards. Examples:
`TR2_STM32_ST_FIFO_WINDOW=ON`, `TR2_STM32_REGISTER_PROBE=ON`, or the dedicated
M014 production options without cooperative FIFO. These emit
`tr2-special-<mode>-NON-CUMULATIVE.{elf,bin,map}`. `build-profile.txt` records all
STM32 options, build type and commissioning paths; inspect it for modifiers
such as silence windows, diagnostic address and TX. Manufacturing is never the
cumulative server. Multiple primary modes and implicit all-OFF are refused.

The generic historical harness requires `TR2_STM32_LEGACY_SD_HARNESS=ON`, or:

```sh
STM32CUBE_U5_ROOT=/home/lolo/dev/msm/tools/STM32CubeU5-v1.9.0 \
  bash 'Modbus RTU/05_Firmware/platform/stm32/tr2_build_stm32.sh' --profile legacy-sd-harness
```

Its output is `tr2-legacy-SD-WRITES-NON-PRODUCTION.{elf,bin,map}`. It cannot be
combined with dedicated modes, qualified TX or the IRQ observer. It retains
historical automatic SD writes, including LBA2048 and campaign metadata tests;
**only sacrificial media and an explicitly authorized hardware mission** may
use it. A compile-time guard in `main.c` also rejects direct all-OFF builds.

## Before any future programming operation

Follow AGENTS.md and the mission's flash/debug permissions, target/ST-LINK
identity, preservation and the already qualified programming/debug procedure.
Match the chosen BIN, ELF, map, `build-profile.txt`, actual CMake cache and
SHA-256 to the intended profile and tested image; never reuse a breakpoint from
another ELF. Standard Flash address remains the linker origin `0x08000000`;
this document supplies no new flash command or permission. A filename/profile
is a selection guard, not an authorization or physical proof. A04-01 performs
software builds only; it neither flashes nor observes the connected STM32.
