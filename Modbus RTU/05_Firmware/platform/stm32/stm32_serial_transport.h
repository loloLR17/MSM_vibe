#ifndef TR2_STM32_SERIAL_TRANSPORT_H
#define TR2_STM32_SERIAL_TRANSPORT_H

#include "tr2/platform/serial_transport.h"

/* Board qualification gate. The checked-in firmware stays RX-only.
 * A qualified board build may define 1; this does not provision production
 * runtime, unit ID, or identity, nor bypass the response/silence policy.
 */
#ifndef TR2_STM32_RS485_TX_QUALIFIED
#define TR2_STM32_RS485_TX_QUALIFIED 0
#endif

Tr2Result stm32_serial_transport_init(SerialTransport *transport);
void stm32_serial_transport_irq_handler(void);
void stm32_serial_transport_tim6_irq_handler(void);

#endif
