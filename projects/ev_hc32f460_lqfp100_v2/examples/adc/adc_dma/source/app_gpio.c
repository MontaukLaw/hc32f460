#include "user_comm.h"

const GPIO_Channel channels[] = {CH_DEF(0), CH_DEF(1), CH_DEF(2), CH_DEF(3), CH_DEF(4), CH_DEF(5), CH_DEF(6), CH_DEF(7),
                                   CH_DEF(8), CH_DEF(9), CH_DEF(10), CH_DEF(11), CH_DEF(12), CH_DEF(13), CH_DEF(14), CH_DEF(15)};

/**
 * @brief  LED Init
 * @param  None
 * @retval None
 */
void LED_Init(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;

    (void)GPIO_Init(LED_G_PORT, LED_G_PIN, &stcGpioInit);
    (void)GPIO_Init(LED_B_PORT, LED_B_PIN, &stcGpioInit);
    (void)GPIO_Init(LED_R_PORT, LED_R_PIN, &stcGpioInit);
}

void led_test(void)
{
    // 全部先拉高
    GPIO_SetPins(LED_G_PORT, LED_G_PIN);
    GPIO_SetPins(LED_B_PORT, LED_B_PIN);
    GPIO_SetPins(LED_R_PORT, LED_R_PIN);

    // 测试一下, 亮蓝灯
    GPIO_ResetPins(LED_B_PORT, LED_B_PIN);
}

void set_output(uint8_t signal_ch, uint8_t is_on)
{
    if (signal_ch < MAX_CHANNELS)
    {
        if (is_on)
        {
            GPIO_SetPins(channels[signal_ch].port, channels[signal_ch].pin);
        }
        else
        {
            GPIO_ResetPins(channels[signal_ch].port, channels[signal_ch].pin);
        }
    }
}

void gpio_init(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    
    GPIO_SetDebugPort(GPIO_PIN_TDI|GPIO_PIN_TRST|GPIO_PIN_TDO, DISABLE);

    for (size_t i = 0; i < sizeof(channels) / sizeof(channels[0]); i++)
    {
        (void)GPIO_Init(channels[i].port, channels[i].pin, &stcGpioInit);
    }

    GPIO_Init(SW4067_S0_PORT, SW4067_S0_PIN, &stcGpioInit);
    GPIO_Init(SW4067_S1_PORT, SW4067_S1_PIN, &stcGpioInit);
    GPIO_Init(SW4067_S2_PORT, SW4067_S2_PIN, &stcGpioInit);
    GPIO_Init(SW4067_S3_PORT, SW4067_S3_PIN, &stcGpioInit);

    GPIO_Init(TEST_POINT_PORT, TEST_POINT_PIN, &stcGpioInit);

    GPIO_Init(BL_LOW_POWER_PORT, BL_LOW_POWER_PIN, &stcGpioInit);

    // 拉低
    GPIO_ResetPins(BL_LOW_POWER_PORT, BL_LOW_POWER_PIN);

}