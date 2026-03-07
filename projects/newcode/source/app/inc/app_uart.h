#ifndef APP_UART_H
#define APP_UART_H
extern void init_uart(void);
extern void send_data(char *data,int len);
extern int esp32sendflag;

#endif  

