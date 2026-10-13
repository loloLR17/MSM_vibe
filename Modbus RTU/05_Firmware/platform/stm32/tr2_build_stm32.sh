#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FIRMWARE_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
PROFILE=""
CUBE_ROOT="${STM32CUBE_U5_ROOT:-}"
while [[ $# -gt 0 ]]; do
    case "$1" in
        --profile) PROFILE="${2:?missing profile}"; shift ;;
        --help|-h)
            echo "Usage: $0 --profile cumulative|legacy-sd-harness [STM32CubeU5-root]"
            echo "No flash. Legacy profile writes microSD at boot; sacrificial media only."
            exit 0 ;;
        -*) echo "error: unknown option: $1" >&2; exit 2 ;;
        *) [[ -z "${CUBE_ROOT}" ]] || { echo "error: extra SDK argument" >&2; exit 2; }; CUBE_ROOT="$1" ;;
    esac
    shift
done
case "${PROFILE}" in
    cumulative)
        profile_options=(-DTR2_STM32_BUILD_PROFILE=CUMULATIVE -DTR2_STM32_FIFO_SPI_MHZ=4
            -DTR2_COMMISSIONING_MANIFEST="${FIRMWARE_DIR}/../00_gouvernance/COMMISSIONING_M014_H.json"
            -DTR2_COMMISSIONING_REGISTRY="${FIRMWARE_DIR}/../00_gouvernance/REGISTRE_ATTRIBUTION_TR2.json")
        artifact=tr2-cumulative-m014-m015-spi4 ;;
    legacy-sd-harness)
        profile_options=(-DTR2_STM32_LEGACY_SD_HARNESS=ON)
        artifact=tr2-legacy-SD-WRITES-NON-PRODUCTION ;;
    *) echo "error: explicit --profile cumulative|legacy-sd-harness required" >&2; exit 2 ;;
esac
BUILD_DIR="${FIRMWARE_DIR}/build-stm32-${PROFILE}"

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

# Preserve artefacts; reuse only a directory already labelled for this profile.
if [[ -e "${BUILD_DIR}" ]] && ! [[ -f "${BUILD_DIR}/build-profile.txt" && "$(head -n 1 "${BUILD_DIR}/build-profile.txt")" == "artifact=${artifact}" ]]; then
    echo "error: unlabelled or conflicting build directory; use a fresh direct CMake build: ${BUILD_DIR}" >&2
    exit 6
fi

cmake -G Ninja \
    -S "${SCRIPT_DIR}" \
    -B "${BUILD_DIR}" \
    -DCMAKE_TOOLCHAIN_FILE="${SCRIPT_DIR}/cmake/arm-none-eabi-gcc.cmake" \
    -DSTM32CUBE_U5_ROOT="${CUBE_ROOT}" \
    "${profile_options[@]}"

cmake --build "${BUILD_DIR}"

artifacts=(
    "${artifact}.elf"
    "${artifact}.bin"
    "${artifact}.map"
)

for expected_artifact in "${artifacts[@]}"; do
    [[ -s "${BUILD_DIR}/${expected_artifact}" ]] || {
        echo "error: expected artifact missing or empty: ${BUILD_DIR}/${expected_artifact}" >&2
        exit 5
    }
done

sha256sum "${BUILD_DIR}/${artifact}.bin" "${BUILD_DIR}/${artifact}.elf" > "${BUILD_DIR}/artifact-sha256.txt"
echo "TR2 ${PROFILE} cross-build complete: ${artifact}."
echo "Profile: ${BUILD_DIR}/build-profile.txt; hashes: ${BUILD_DIR}/artifact-sha256.txt"
echo "Software build only; no hardware qualification or flash."
