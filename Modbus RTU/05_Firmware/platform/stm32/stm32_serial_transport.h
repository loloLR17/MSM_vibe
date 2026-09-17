#ifndef TR2_STM32_SERIAL_TRANSPORT_H
#define TR2_STM32_SERIAL_TRANSPORT_H

#include "tr2/platform/serial_transport.h"

Tr2Result stm32_serial_transport_init(SerialTransport *transport);
void stm32_serial_transport_irq_handler(void);
void stm32_serial_transport_tim6_irq_handler(void);

#endif
