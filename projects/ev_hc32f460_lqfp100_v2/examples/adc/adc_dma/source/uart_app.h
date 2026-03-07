#ifndef _UART_APP_H_
#define _UART_APP_H_

void uart4_init(void);
void uart2_init(void);

void enable_uart4(void);
void enable_uart2(void);

void uart4_send_data(uint8_t *data, uint16_t len);
void uart2_send_data(uint8_t *data, uint16_t len);

#endif /* _UART_APP_H_ */
