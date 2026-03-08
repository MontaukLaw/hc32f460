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

void led_blue_on(void)
{
    all_led_off();
    GPIO_ResetPins(LED_B_PORT, LED_B_PIN);
}

void blink_blue(void)
{
    static uint32_t last_toggle_ts = 0;
    if (SysTick_GetTick() - last_toggle_ts < 500)
        return;

    GPIO_TogglePins(LED_B_PORT, LED_B_PIN);

    last_toggle_ts = SysTick_GetTick();
}

void led_ctl(void)
{
    if (bl_conn_status == 0)
    {
        blink_blue();
    }
    else
    {
        led_blue_on();
    }
}

void led_task(void)
{

    led_ctl();
}