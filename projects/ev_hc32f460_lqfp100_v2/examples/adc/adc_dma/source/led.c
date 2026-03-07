#include "user_comm.h"

void all_led_off(void)
{
    GPIO_SetPins(LED_R_PORT, LED_R_PIN);
    GPIO_SetPins(LED_G_PORT, LED_G_PIN);
    GPIO_SetPins(LED_B_PORT, LED_B_PIN);
}

void led_red_on(void)
{
    all_led_off();
    GPIO_ResetPins(LED_R_PORT, LED_R_PIN);
}