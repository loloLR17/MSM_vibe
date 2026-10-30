#ifndef R3_WITNESS_UART_H
#define R3_WITNESS_UART_H
#include <stdint.h>
int witness_uart_init(void);
int witness_uart_step(const uint32_t uid[3],uint32_t flags,uint64_t mcu_us,uint64_t session,uint32_t accepted);
extern uint32_t witness_sequence,witness_frames,witness_errors;
void witness_frame(uint8_t out[48],const uint32_t uid[3],uint32_t sequence,uint64_t mcu_us,uint32_t flags,uint64_t session,uint32_t accepted);
#endif
