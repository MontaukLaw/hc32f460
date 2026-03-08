#include "user_comm.h"

uint8_t bl_tx_buf[OLD_FRAME_LEN] = {0};
uint8_t bl_mac_get = 0;
uint8_t mac_add[20] = {0};
uint8_t MAC_AT_FB[] = {
    '+',
    'M',
    'A',
    'C',
    ':',
};
uint8_t bl_conn_status = 0;

void bl_at_mac(void)
{
    sprintf((char *)bl_tx_buf, "AT+MAC?\r\n");
    uart4_send_data((uint8_t *)bl_tx_buf, strlen((char *)bl_tx_buf));
}

void recv_handler_bl(uint8_t len)
{
    if (memcmp(u4_rx_buf, MAC_AT_FB, sizeof(MAC_AT_FB)) == 0)
    {
        // printf("MAC Address: ");
        for (uint8_t i = sizeof(MAC_AT_FB); i < len; i++)
        {
            mac_add[i - sizeof(MAC_AT_FB)] = u4_rx_buf[i];
            // printf("%c", uart2_rx_buf[i]);
        }
        bl_mac_get = 1;
        // printf("\r\n");
    }
}

void bl_task(void)
{
    if (SET == u4RxFrameEnd)
    {
        // u4_send_back();
        recv_handler_bl(u4_rx_len);
        re_config_u4_rx_dma();

        u4RxFrameEnd = RESET;
    }
}

void ble_task(void)
{
    static uint32_t last_run_ts = 0;
    if (SysTick_GetTick() - last_run_ts < 50)
        return;

    uart4_send_data(bl_tx_buf, OLD_FRAME_LEN);
    last_run_ts = SysTick_GetTick();
}

void get_bl_mac(void)
{
    if (bl_mac_get)
        return;

    bl_at_mac();
}

void bl_link_status_check(void)
{
    static uint32_t last_run_ts = 0;
    if (SysTick_GetTick() - last_run_ts < 1000)
        return;

    get_bl_mac();

    // 读取bl link引脚
    if (GPIO_ReadInputPins(BL_LINK_PORT, BL_LINK_PIN) == PIN_SET)
    {
        bl_conn_status = 1;
    }
    else
    {
        bl_conn_status = 0;
    }

    last_run_ts = SysTick_GetTick();
}
