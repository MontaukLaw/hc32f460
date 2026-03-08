#include "user_comm.h"

const char *SELF_AT_CMD_MAC = "AT+MAC";
const char *HEAD_STR = "::::::::MAC Address:::::\r\n";

void self_cmd_handler(void)
{

    if (memcmp(u2_rx_buf, SELF_AT_CMD_MAC, strlen(SELF_AT_CMD_MAC)) == 0)
    {

        u2_send_str_sync(HEAD_STR, strlen(HEAD_STR));
        u2_send_str_sync((const char *)mac_add, 12);
        u2_send_str_sync("\r\n", 2);
        stop_u1 = 1;
    }
}

void com_task(void)
{

    if (u2RxFrameEnd == SET)
    {
        /* Send back received data. */
        // u2_send_back();
        // comm_handler();
        self_cmd_handler();

        // uart2_send_data();
        re_config_u2_rx_dma();

        u2RxFrameEnd = RESET;
    }
}