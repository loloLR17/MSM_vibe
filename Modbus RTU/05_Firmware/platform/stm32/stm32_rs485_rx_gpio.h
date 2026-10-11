#ifndef TR2_STM32_RS485_RX_GPIO_H
#define TR2_STM32_RS485_RX_GPIO_H

/* Board bring-up only: confirmed 3.31 V VDDIO2, DE wired to PG4.
 * Call from HAL_MspInit before UART alternate-function configuration.
 * Does not control a separate /RE pin or enable transmission.
 */
void stm32_rs485_rx_gpio_init(void);

#endif
