#include "user_comm.h"
// 0~15
void set_ch(uint8_t ch)
{
    if (ch >= MAX_CHANNELS)
        return;

    // S0~S3
    if (ch & 0x01)
        GPIO_SetPins(SW4067_S0_PORT, SW4067_S0_PIN);
    else
        GPIO_ResetPins(SW4067_S0_PORT, SW4067_S0_PIN);

    if (ch & 0x02)
        GPIO_SetPins(SW4067_S1_PORT, SW4067_S1_PIN);
    else
        GPIO_ResetPins(SW4067_S1_PORT, SW4067_S1_PIN);

    if (ch & 0x04)
        GPIO_SetPins(SW4067_S2_PORT, SW4067_S2_PIN);
    else
        GPIO_ResetPins(SW4067_S2_PORT, SW4067_S2_PIN);

    if (ch & 0x08)
        GPIO_SetPins(SW4067_S3_PORT, SW4067_S3_PIN);
    else
        GPIO_ResetPins(SW4067_S3_PORT, SW4067_S3_PIN);
}
