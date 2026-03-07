#ifndef APP_GPIO_H
#define APP_GPIO_H
extern int led_status;
extern void init_gpio(void);
extern void gpio_set_level(en_port_t port,uint16_t gpio,int num);
extern void hc595Send4Byte(uint32_t byte);
extern void hc4067Chose1Channel32(uint8_t channel);
#endif


