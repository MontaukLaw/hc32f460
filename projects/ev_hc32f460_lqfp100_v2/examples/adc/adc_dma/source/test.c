#include "user_comm.h"

void test_task(void)
{
    
    static uint32_t last_ts = 0;
    if (SysTick_GetTick() - last_ts < 1000)
        return;

    // uart2_send_data((uint8_t *)"u2 hello\r\n", 7);
    bl_at_mac();

    last_ts = SysTick_GetTick();

}