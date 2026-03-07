/**
 *******************************************************************************
 * @file  usart/usart_uart_dma/source/main.c
 * @brief This example demonstrates UART data receive and transfer by DMA.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
   2022-10-31       CDT             Delete the redundant code
                                    Read USART_DR.RDR when USART overrun error occur.
   2023-01-15       CDT             Update UART timeout function calculating formula for Timer0 CMP value
   2023-09-30       CDT             Split register USART_DR to USART_RDR and USART_TDR
   2024-11-08       CDT             Optimize function: USART_TxComplete_IrqCallback
                                    Add function: USART_StopTimeoutTimer
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022-2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "main.h"
#include "dma_uart.h"
/**
 * @addtogroup HC32F460_DDL_Examples
 * @{
 */

/**
 * @addtogroup USART_UART_DMA
 * @{
 */

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* Peripheral register WE/WP selection */

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/

/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/

static void Uart_SendByte(uint8_t b)
{
    /* 等待发送数据寄存器空 */
    while (RESET == USART_GetStatus(USART_UNIT, USART_FLAG_TX_EMPTY))
    {
    }
    USART_WriteData(USART_UNIT, b);

    /* 等待发送完成（最后一个 stop bit 发完）*/
    while (RESET == USART_GetStatus(USART_UNIT, USART_FLAG_TX_CPLT))
    {
    }
    USART_ClearStatus(USART_UNIT, USART_FLAG_TX_CPLT);
}

static void Uart_SendStr(const char *s)
{
    while (*s)
        Uart_SendByte((uint8_t)*s++);
}


uint8_t msg[] = "DMA hello\r\n";
#if 0
void Uart_DmaSend(const uint8_t *buf, uint16_t len)
{
    if (len == 0)
        return;

    g_tx_dma_done = 0;
    g_tx_uart_done = 0;

    // 1) 确保 DMA 通道处于关闭（避免正在传输时被改配置）
    DMA_ChCmd(TX_DMA_UNIT, TX_DMA_CH, DISABLE);

    // 2) 设置源地址和长度
    DMA_SetSrcAddr(TX_DMA_UNIT, TX_DMA_CH, (uint32_t)buf);
    DMA_SetTransCount(TX_DMA_UNIT, TX_DMA_CH, len);

    // 3) 清 DMA TC flag（很关键，不然可能立刻触发“假完成”）
    DMA_ClearTransCompleteStatus(TX_DMA_UNIT, TX_DMA_TC_FLAG);

    // 4) 先开 USART_TX（TE=1），让 USART 的 TI 触发源能工作
    USART_FuncCmd(USART_UNIT, USART_TX, ENABLE);

    // 5) 使能 DMA 通道开始搬运
    DMA_ChCmd(TX_DMA_UNIT, TX_DMA_CH, ENABLE);

    // 6) 等 DMA 搬完（表示 len 字节都写进 TDR 了）
    while (!g_tx_dma_done)
    {
    }

    // 7) 等 USART 真正发完（stop bit 发完）
    while (!g_tx_uart_done)
    {
    }
}
#endif

/**
 * @brief  Main function of UART DMA project
 * @param  None
 * @retval int32_t return value, if needed
 */
int32_t main(void)
{
    uint8_t i;
    
    uart_dma_init();

    while (1)
    {
        uart_send_test();
        DDL_DelayMS(1000);
    }

    uint8_t tx[1] = {0};

    while (1)
    {
        DDL_DelayMS(100);
        Uart_DmaSend(msg, sizeof(msg) - 1);
    }

    while (1)
    {
        i++;
        tx[0] = i;
        Uart_SendStr(tx);
        // Uart_SendStr("HELLO\r\n");
        DDL_DelayMS(500);
    }

#if 0
    for (;;)
    {
        if (SET == m_enRxFrameEnd)
        {
            DMA_SetSrcAddr(TX_DMA_UNIT, TX_DMA_CH, (uint32_t)m_au8RxBuf);

            DMA_SetTransCount(TX_DMA_UNIT, TX_DMA_CH, m_u16RxLen);

            (void)DMA_ChCmd(TX_DMA_UNIT, TX_DMA_CH, ENABLE);

            USART_FuncCmd(USART_UNIT, USART_TX, ENABLE);

            m_enRxFrameEnd = RESET;
        }
    }
#endif
}

/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
