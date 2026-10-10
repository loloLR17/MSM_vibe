#!/bin/bash
set -o pipefail
run() {
 printf '\nCOMMAND: '; printf '%q ' "$@"; printf '\n'
 "$@"
 rc=$?
 echo "EXIT_CODE=$rc"
 if [ "$rc" -ne 0 ]; then exit "$rc"; fi
}
mkdir -p host arm
cat > host/CMakeLists.txt <<'EOF'
cmake_minimum_required(VERSION 3.20)
project(T1000Qualification LANGUAGES C CXX)
enable_testing()
add_executable(host_c main.c)
add_executable(host_cpp main.cpp)
target_compile_options(host_c PRIVATE -Wall -Wextra -Werror)
target_compile_options(host_cpp PRIVATE -Wall -Wextra -Werror)
add_test(NAME host_c COMMAND host_c)
add_test(NAME host_cpp COMMAND host_cpp)
EOF
cat > host/main.c <<'EOF'
#include <stdio.h>
int main(void) { int value = 6 * 7; printf("T1000 C: %d\n", value); return value == 42 ? 0 : 1; }
EOF
cat > host/main.cpp <<'EOF'
#include <iostream>
#include <numeric>
#include <vector>
int main() { std::vector<int> v{10,12,20}; auto sum = std::accumulate(v.begin(),v.end(),0); std::cout << "T1000 C++: " << sum << '\n'; return sum == 42 ? 0 : 1; }
EOF
cat > arm/minimal.c <<'EOF'
#include <stdint.h>
#include <string.h>
volatile uint32_t result;
void _start(void) { char dst[4]; const char src[4] = {'T','1','0','0'}; memcpy(dst,src,4); result = (uint32_t)dst[0]; for (;;) {} }
EOF
cat > arm/minimal.cpp <<'EOF'
extern "C" int cpp_add(int a, int b) { return a + b; }
EOF
run cmake -S host -B host/build -G Ninja -DCMAKE_BUILD_TYPE=Release
run cmake --build host/build --verbose
run ctest --test-dir host/build --output-on-failure
run host/build/host_c
run host/build/host_cpp
run arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -ffreestanding -fno-builtin -Wall -Wextra -Werror -c arm/minimal.c -o arm/minimal.o
run arm-none-eabi-g++ -mcpu=cortex-m33 -mthumb -ffreestanding -fno-exceptions -fno-rtti -Wall -Wextra -Werror -c arm/minimal.cpp -o arm/minimal_cpp.o
run arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -nostartfiles -Wl,-e,_start -Wl,-Ttext=0x08000000 arm/minimal.o arm/minimal_cpp.o -lc -lgcc -o arm/minimal.elf
run arm-none-eabi-readelf -h -A arm/minimal.o arm/minimal.elf
run arm-none-eabi-nm arm/minimal.elf
run arm-none-eabi-size arm/minimal.elf
run arm-none-eabi-objcopy -O binary arm/minimal.elf arm/minimal.bin
run file arm/minimal.o arm/minimal.elf
