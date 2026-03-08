#ifndef _UART_APP_H_
#define _UART_APP_H_

void uart4_init(void);
void uart2_init(void);

void enable_uart4(void);
void enable_uart2(void);

void u2_send_str_sync(const char *s, uint16_t len);

void uart4_send_data(uint8_t *data, uint16_t len);
void uart2_send_data(uint8_t *data, uint16_t len);

void re_config_u2_rx_dma(void);
void re_config_u4_rx_dma(void);

void u4_send_back(void);

void u4_task(void);
void u2_task(void);

extern __IO en_flag_status_t u4RxFrameEnd;
extern __IO en_flag_status_t u2RxFrameEnd;

extern __IO uint16_t u4_rx_len;
extern __IO uint16_t u2_rx_len;

extern uint8_t u4_rx_buf[];
extern uint8_t u2_rx_buf[];

#endif /* _UART_APP_H_ */
