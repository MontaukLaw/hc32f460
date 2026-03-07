#ifndef _APP_GPIO_H_
#define _APP_GPIO_H_

#include "hc32_ll.h"

#define SW4067_S0_PORT (GPIO_PORT_H)
#define SW4067_S0_PIN (GPIO_PIN_02)

#define SW4067_S1_PORT (GPIO_PORT_C)
#define SW4067_S1_PIN (GPIO_PIN_13)

#define SW4067_S2_PORT (GPIO_PORT_C)
#define SW4067_S2_PIN (GPIO_PIN_14)

#define SW4067_S3_PORT (GPIO_PORT_C)
#define SW4067_S3_PIN (GPIO_PIN_15)

#define OUT_D0_PORT (GPIO_PORT_B)
#define OUT_D0_PIN (GPIO_PIN_05)

#define OUT_D1_PORT (GPIO_PORT_B)
#define OUT_D1_PIN (GPIO_PIN_04)

#define OUT_D2_PORT (GPIO_PORT_B)
#define OUT_D2_PIN (GPIO_PIN_03)

#define OUT_D3_PORT (GPIO_PORT_A)
#define OUT_D3_PIN (GPIO_PIN_15)

#define OUT_D4_PORT (GPIO_PORT_A)
#define OUT_D4_PIN (GPIO_PIN_12)

#define OUT_D5_PORT (GPIO_PORT_A)
#define OUT_D5_PIN (GPIO_PIN_11)

#define OUT_D6_PORT (GPIO_PORT_A)
#define OUT_D6_PIN (GPIO_PIN_10)

#define OUT_D7_PORT (GPIO_PORT_A)
#define OUT_D7_PIN (GPIO_PIN_09)

#define OUT_D8_PORT (GPIO_PORT_A)
#define OUT_D8_PIN (GPIO_PIN_08)

#define OUT_D9_PORT (GPIO_PORT_B)
#define OUT_D9_PIN (GPIO_PIN_15)

#define OUT_D10_PORT (GPIO_PORT_B)
#define OUT_D10_PIN (GPIO_PIN_14)

#define OUT_D11_PORT (GPIO_PORT_B)
#define OUT_D11_PIN (GPIO_PIN_13)

#define OUT_D12_PORT (GPIO_PORT_A)
#define OUT_D12_PIN (GPIO_PIN_05)

#define OUT_D13_PORT (GPIO_PORT_A)
#define OUT_D13_PIN (GPIO_PIN_06)

#define OUT_D14_PORT (GPIO_PORT_A)
#define OUT_D14_PIN (GPIO_PIN_07)

#define OUT_D15_PORT (GPIO_PORT_B)
#define OUT_D15_PIN (GPIO_PIN_00)

#define LED_G_PORT (GPIO_PORT_A)
#define LED_G_PIN (GPIO_PIN_04)

#define LED_B_PORT (GPIO_PORT_A)
#define LED_B_PIN (GPIO_PIN_00)

#define LED_R_PORT (GPIO_PORT_A)
#define LED_R_PIN (GPIO_PIN_01)

#define TEST_POINT_PORT (GPIO_PORT_B)
#define TEST_POINT_PIN (GPIO_PIN_12)

#define BL_LOW_POWER_PORT (GPIO_PORT_B)
#define BL_LOW_POWER_PIN (GPIO_PIN_07)

// uint8_t u8Port, uint16_t u16Pin,
typedef struct
{
    uint8_t port;
    uint16_t pin;
} GPIO_Channel;

#define CH_DEF(n) {OUT_D##n##_PORT, OUT_D##n##_PIN}

void LED_Init(void);

void led_test(void);

void set_output(uint8_t signal_ch, uint8_t is_on);

void gpio_init(void);

#endif /* _APP_GPIO_H_ */
