#include <stdint.h>

int main(void)
{
    volatile uint32_t alive = 0U;

    for (;;) {
        alive++;
        __asm volatile ("nop");
    }
}
