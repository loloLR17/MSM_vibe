#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FIRMWARE_DIR="${SCRIPT_DIR}/Modbus RTU/05_Firmware"
HOST_BUILD_DIR="${FIRMWARE_DIR}/build-host-validation"
STM32_BUILD_SCRIPT="${FIRMWARE_DIR}/platform/stm32/tr2_build_stm32.sh"
CUBE_ROOT="${STM32CUBE_U5_ROOT:-${1:-}}"

log_section() {
    printf '\n============================================================\n'
    printf '%s\n' "$1"
    printf '============================================================\n'
}

fail() {
    echo "error: $*" >&2
    exit 1
}

[[ -f "${FIRMWARE_DIR}/CMakeLists.txt" ]] || \
    fail "firmware CMakeLists.txt not found at ${FIRMWARE_DIR}"

[[ -f "${STM32_BUILD_SCRIPT}" ]] || \
    fail "STM32 cross-build helper not found at ${STM32_BUILD_SCRIPT}"

required_host_tools=(cmake)
for tool in "${required_host_tools[@]}"; do
    command -v "${tool}" >/dev/null 2>&1 || fail "missing required host tool: ${tool}"
done

if [[ -z "${CUBE_ROOT}" ]]; then
    cat >&2 <<'EOF'
error: STM32CubeU5 root is required for the P12-A cross-build.

Use either:
  STM32CUBE_U5_ROOT=/path/to/STM32CubeU5 bash ./tr2_validate.sh

or:
  bash ./tr2_validate.sh /path/to/STM32CubeU5
EOF
    exit 2
fi

log_section "TR2 VALIDATION — HOST"
rm -rf "${HOST_BUILD_DIR}"

cmake \
    -S "${FIRMWARE_DIR}" \
    -B "${HOST_BUILD_DIR}"

cmake --build "${HOST_BUILD_DIR}"

ctest \
    --test-dir "${HOST_BUILD_DIR}" \
    --output-on-failure

echo "HOST VALIDATED: firmware host build and tests passed."

log_section "TR2 VALIDATION — STM32 CROSS-BUILD P12-A"
STM32CUBE_U5_ROOT="${CUBE_ROOT}" \
    bash "${STM32_BUILD_SCRIPT}"

echo "CROSS-BUILD VALIDATED: portable tr2_core compiled and linked for STM32U575 Cortex-M33."

log_section "TR2 VALIDATION RESULT"
echo "HOST VALIDATED"
echo "CROSS-BUILD VALIDATED"
echo "HARDWARE PENDING: no NUCLEO-U575ZI-Q runtime or RS-485 hardware claim is made."
