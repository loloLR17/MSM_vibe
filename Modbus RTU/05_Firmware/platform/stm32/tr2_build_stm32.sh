#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FIRMWARE_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${FIRMWARE_DIR}/build-stm32-p11c"
CUBE_ROOT="${STM32CUBE_U5_ROOT:-${1:-}}"

if [[ -z "${CUBE_ROOT}" ]]; then
    echo "error: set STM32CUBE_U5_ROOT or pass the STM32CubeU5 root as argument" >&2
    exit 2
fi

required_tools=(cmake ninja arm-none-eabi-gcc arm-none-eabi-objcopy arm-none-eabi-size)
for tool in "${required_tools[@]}"; do
    command -v "${tool}" >/dev/null 2>&1 || {
        echo "error: missing required tool: ${tool}" >&2
        exit 3
    }
done

required_cube_files=(
    "Drivers/CMSIS/Include/core_cm33.h"
    "Drivers/CMSIS/Device/ST/STM32U5xx/Include/stm32u575xx.h"
    "Drivers/CMSIS/Device/ST/STM32U5xx/Source/Templates/gcc/startup_stm32u575xx.s"
    "Drivers/CMSIS/Device/ST/STM32U5xx/Source/Templates/system_stm32u5xx.c"
    "Drivers/STM32U5xx_HAL_Driver/Inc/stm32u5xx_hal.h"
    "Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c"
)

for relative_path in "${required_cube_files[@]}"; do
    [[ -f "${CUBE_ROOT}/${relative_path}" ]] || {
        echo "error: incomplete STM32CubeU5 checkout, missing ${CUBE_ROOT}/${relative_path}" >&2
        exit 4
    }
done

rm -rf "${BUILD_DIR}"

cmake -G Ninja \
    -S "${SCRIPT_DIR}" \
    -B "${BUILD_DIR}" \
    -DCMAKE_TOOLCHAIN_FILE="${SCRIPT_DIR}/cmake/arm-none-eabi-gcc.cmake" \
    -DSTM32CUBE_U5_ROOT="${CUBE_ROOT}"

cmake --build "${BUILD_DIR}"

artifacts=(
    "tr2_stm32_p11c.elf"
    "tr2_stm32_p11c.bin"
    "tr2_stm32_p11c.map"
)

for artifact in "${artifacts[@]}"; do
    [[ -s "${BUILD_DIR}/${artifact}" ]] || {
        echo "error: expected artifact missing or empty: ${BUILD_DIR}/${artifact}" >&2
        exit 5
    }
done

echo "TR2 P11-C STM32 cross-build complete."
echo "Hardware validation remains pending until the NUCLEO-U575ZI-Q is available."
