#include "user_comm.h"

__IO en_flag_status_t u4RxFrameEnd = RESET;
// static __IO en_flag_status_t m_enRxFrameEnd;
__IO uint16_t u4_rx_len;
__IO uint16_t u4_tx_len;
uint8_t u4_rx_buf[APP_FRAME_LEN_MAX];
uint8_t u4_tx_buf[APP_FRAME_LEN_MAX];

/**
 * @brief  DMA transfer complete IRQ callback function.
 * @param  None
 * @retval None
 */
static void RX_DMA_TC_IrqCallback(void)
{

    // m_enRxFrameEnd = SET;
    u4RxFrameEnd = SET;

    u4_rx_len = APP_FRAME_LEN_MAX;

    USART_FuncCmd(U4_USART_UNIT, USART_RX_TIMEOUT, DISABLE);

    DMA_ClearTransCompleteStatus(U4_RX_DMA_UNIT, U4_RX_DMA_TC_FLAG);
}

#if 0
void u4_send_back(uint16_t tx_len)
{
    u4_tx_len = tx_len;

    DMA_SetSrcAddr(U4_TX_DMA_UNIT, U4_TX_DMA_CH, (uint32_t)u4_tx_buf);

    DMA_SetTransCount(U4_TX_DMA_UNIT, U4_TX_DMA_CH, u4_tx_len);

    (void)DMA_ChCmd(U4_TX_DMA_UNIT, U4_TX_DMA_CH, ENABLE);

    USART_FuncCmd(U4_USART_UNIT, USART_TX, ENABLE);
}
#endif

/**
 * @brief  DMA transfer complete IRQ callback function.
 * @param  None
 * @retval None
 */
static void TX_DMA_TC_IrqCallback(void)
{
    USART_FuncCmd(U4_USART_UNIT, USART_INT_TX_CPLT, ENABLE);

    DMA_ClearTransCompleteStatus(U4_TX_DMA_UNIT, U4_TX_DMA_TC_FLAG);
}

void re_config_u4_rx_dma(void)
{
    DMA_ClearTransCompleteStatus(U4_RX_DMA_UNIT, U4_RX_DMA_TC_FLAG);

    // 手动关通道
    DMA_ChCmd(U4_RX_DMA_UNIT, U4_RX_DMA_CH, DISABLE);

    // 重新设置传输计数 / 目的地址
    DMA_SetSrcAddr(U4_RX_DMA_UNIT, U4_RX_DMA_CH, (uint32_t)&U4_USART_UNIT->RDR);
    DMA_SetDestAddr(U4_RX_DMA_UNIT, U4_RX_DMA_CH, (uint32_t)u4_rx_buf);
    DMA_SetTransCount(U4_RX_DMA_UNIT, U4_RX_DMA_CH, ARRAY_SZ(u4_rx_buf));

    // 重新开通道
    DMA_ChCmd(U4_RX_DMA_UNIT, U4_RX_DMA_CH, ENABLE);
}

/**
 * @brief  Initialize DMA.
 * @param  None
 * @retval int32_t:
 *           - LL_OK:                   Initialize successfully.
 *           - LL_ERR_INVD_PARAM:       Initialization parameters is invalid.
 */
static int32_t DMA_Config(void)
{
    int32_t i32Ret;
    stc_dma_init_t stcDmaInit;
    stc_dma_llp_init_t stcDmaLlpInit;
    stc_irq_signin_config_t stcIrqSignConfig;
    static stc_dma_llp_descriptor_t stcLlpDesc;

    /* DMA&AOS FCG enable */
    U4_RX_DMA_FCG_ENABLE();
    U4_TX_DMA_FCG_ENABLE();
    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);

    /* USART_RX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = ARRAY_SZ(u4_rx_buf);
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)u4_rx_buf;
    stcDmaInit.u32SrcAddr = (uint32_t)(&U4_USART_UNIT->RDR);
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC;
    i32Ret = DMA_Init(U4_RX_DMA_UNIT, U4_RX_DMA_CH, &stcDmaInit);
    if (LL_OK == i32Ret)
    {
        (void)DMA_LlpStructInit(&stcDmaLlpInit);
        stcDmaLlpInit.u32State = DMA_LLP_ENABLE;
        stcDmaLlpInit.u32Mode = DMA_LLP_WAIT;
        stcDmaLlpInit.u32Addr = (uint32_t)&stcLlpDesc;
        (void)DMA_LlpInit(U4_RX_DMA_UNIT, U4_RX_DMA_CH, &stcDmaLlpInit);

        stcLlpDesc.SARx = stcDmaInit.u32SrcAddr;
        stcLlpDesc.DARx = stcDmaInit.u32DestAddr;
        stcLlpDesc.DTCTLx = (stcDmaInit.u32TransCount << DMA_DTCTL_CNT_POS) | (stcDmaInit.u32BlockSize << DMA_DTCTL_BLKSIZE_POS);
        ;
        stcLlpDesc.LLPx = (uint32_t)&stcLlpDesc;
        stcLlpDesc.CHCTLx = stcDmaInit.u32SrcAddrInc | stcDmaInit.u32DestAddrInc | stcDmaInit.u32DataWidth |
                            stcDmaInit.u32IntEn | stcDmaLlpInit.u32State | stcDmaLlpInit.u32Mode;

        DMA_ReconfigLlpCmd(U4_RX_DMA_UNIT, U4_RX_DMA_CH, ENABLE);
        DMA_ReconfigCmd(U4_RX_DMA_UNIT, ENABLE);

        // AOS_SetTriggerEventSrc(U4_RX_DMA_RECONF_TRIG_SEL, U4_RX_DMA_RECONF_TRIG_EVT_SRC);

        stcIrqSignConfig.enIntSrc = U4_RX_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn = U4_RX_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &RX_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        // 一个字节一个字节的搬运
        // 把DMA触发源 绑定到 USART接收事件。
        AOS_SetTriggerEventSrc(U4_RX_DMA_TRIG_SEL, U4_RX_DMA_TRIG_EVT_SRC);

        DMA_Cmd(U4_RX_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(U4_RX_DMA_UNIT, U4_RX_DMA_TC_INT, ENABLE);
        (void)DMA_ChCmd(U4_RX_DMA_UNIT, U4_RX_DMA_CH, ENABLE);
    }

    /* USART_TX_DMA */
    (void)DMA_StructInit(&stcDmaInit);
    stcDmaInit.u32IntEn = DMA_INT_ENABLE;
    stcDmaInit.u32BlockSize = 1UL;
    stcDmaInit.u32TransCount = ARRAY_SZ(u4_rx_buf);
    stcDmaInit.u32DataWidth = DMA_DATAWIDTH_8BIT;
    stcDmaInit.u32DestAddr = (uint32_t)(&U4_USART_UNIT->TDR);
    stcDmaInit.u32SrcAddr = (uint32_t)u4_rx_buf;
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_INC;
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_FIX;
    i32Ret = DMA_Init(U4_TX_DMA_UNIT, U4_TX_DMA_CH, &stcDmaInit);
    if (LL_OK == i32Ret)
    {
        stcIrqSignConfig.enIntSrc = U4_TX_DMA_TC_INT_SRC;
        stcIrqSignConfig.enIRQn = U4_TX_DMA_TC_IRQn;
        stcIrqSignConfig.pfnCallback = &TX_DMA_TC_IrqCallback;
        (void)INTC_IrqSignIn(&stcIrqSignConfig);
        NVIC_ClearPendingIRQ(stcIrqSignConfig.enIRQn);
        NVIC_SetPriority(stcIrqSignConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
        NVIC_EnableIRQ(stcIrqSignConfig.enIRQn);

        AOS_SetTriggerEventSrc(U4_TX_DMA_TRIG_SEL, U4_TX_DMA_TRIG_EVT_SRC);

        DMA_Cmd(U4_TX_DMA_UNIT, ENABLE);
        DMA_TransCompleteIntCmd(U4_TX_DMA_UNIT, U4_TX_DMA_TC_INT, ENABLE);
    }

    return i32Ret;
}

/**
 * @brief  Configure TMR0.
 * @param  [in] u16TimeoutBits:         Timeout bits
 * @retval None
 */
static void TMR0_Config(uint16_t u16TimeoutBits)
{
    uint16_t u16Div;
    uint16_t u16Delay;
    uint16_t u16CompareValue;
    stc_tmr0_init_t stcTmr0Init;

    U4_TMR0_FCG_ENABLE();

    /* Initialize TMR0 base function. */
    stcTmr0Init.u32ClockSrc = TMR0_CLK_SRC_LRC;
    stcTmr0Init.u32ClockDiv = TMR0_CLK_DIV8;
    stcTmr0Init.u32Func = TMR0_FUNC_CMP;
    if (TMR0_CLK_DIV1 == stcTmr0Init.u32ClockDiv)
    {
        u16Delay = 7U;
    }
    else if (TMR0_CLK_DIV2 == stcTmr0Init.u32ClockDiv)
    {
        u16Delay = 5U;
    }
    else if ((TMR0_CLK_DIV4 == stcTmr0Init.u32ClockDiv) ||
             (TMR0_CLK_DIV8 == stcTmr0Init.u32ClockDiv) ||
             (TMR0_CLK_DIV16 == stcTmr0Init.u32ClockDiv))
    {
        u16Delay = 3U;
    }
    else
    {
        u16Delay = 2U;
    }

    u16Div = (uint16_t)1U << (stcTmr0Init.u32ClockDiv >> TMR0_BCONR_CKDIVA_POS);
    u16CompareValue = ((u16TimeoutBits + u16Div - 1U) / u16Div) - u16Delay;
    stcTmr0Init.u16CompareValue = u16CompareValue;
    (void)TMR0_Init(U4_TMR0_UNIT, U4_TMR0_CH, &stcTmr0Init);

    TMR0_HWStartCondCmd(U4_TMR0_UNIT, U4_TMR0_CH, ENABLE);
    TMR0_HWClearCondCmd(U4_TMR0_UNIT, U4_TMR0_CH, ENABLE);
}

/**
 * @brief  Stop timeout timer.
 * @param  [in]  TMR0x                  Pointer to TMR0 instance register base.
 *                                      This parameter can be a value of the following:
 *   @arg  CM_TMR0_x or CM_TMR0
 * @param  [in]  u32Ch                  TMR0 channel.
 *                                      This parameter can be a value @ref TMR0_Channel
 */
static void USART_StopTimeoutTimer(CM_TMR0_TypeDef *TMR0x, uint32_t u32Ch)
{
    uint32_t u32ClrMask;
    uint32_t u32SetMask;
    uint32_t u32BitOffset;

    u32BitOffset = 16UL * u32Ch;

    /* Set: TMR0_BCONR.SYNCLKA<B>=1, TMR0_BCONR.SYNA<B>=0 */
    u32ClrMask = (TMR0_BCONR_SYNCLKA | TMR0_BCONR_SYNSA) << u32BitOffset;
    u32SetMask = TMR0_BCONR_SYNCLKA << u32BitOffset;
    MODIFY_REG32(TMR0x->BCONR, u32ClrMask, u32SetMask);

    /* Set: TMR0_BCONR.CSTA<B>=0, TMR0_BCONR.SYNCLKA<B>=0, TMR0_BCONR.SYNSA<B>=1 */
    u32ClrMask = (TMR0_BCONR_SYNCLKA | TMR0_BCONR_SYNSA | TMR0_BCONR_CSTA) << u32BitOffset;
    u32SetMask = TMR0_BCONR_SYNSA << u32BitOffset;
    MODIFY_REG32(TMR0x->BCONR, u32ClrMask, u32SetMask);
}

/**
 * @brief  USART RX timeout IRQ callback.
 * @param  None
 * @retval None
 */
static void USART_RxTimeout_IrqCallback(void)
{
    if (u4RxFrameEnd != SET)
    {
        u4RxFrameEnd = SET;
        u4_rx_len = APP_FRAME_LEN_MAX - (uint16_t)DMA_GetTransCount(U4_RX_DMA_UNIT, U4_RX_DMA_CH);

        /* Trigger for re-config USART RX DMA */
        AOS_SW_Trigger();
    }

    USART_StopTimeoutTimer(U4_TMR0_UNIT, U4_TMR0_CH);

    USART_ClearStatus(U4_USART_UNIT, USART_FLAG_RX_TIMEOUT);
}

/**
 * @brief  USART TX complete IRQ callback function.
 * @param  None
 * @retval None
 */
static void USART_TxComplete_IrqCallback(void)
{
    USART_FuncCmd(U4_USART_UNIT, (USART_TX | USART_INT_TX_CPLT), DISABLE);
}

/**
 * @brief  USART RX error IRQ callback.
 * @param  None
 * @retval None
 */
static void USART_RxError_IrqCallback(void)
{
    (void)USART_ReadData(U4_USART_UNIT);

    USART_ClearStatus(U4_USART_UNIT, (USART_FLAG_PARITY_ERR | USART_FLAG_FRAME_ERR | USART_FLAG_OVERRUN));
}

static void Uart4_SendByte(uint8_t b)
{
    /* 等待发送数据寄存器空 */
    while (RESET == USART_GetStatus(U4_USART_UNIT, USART_FLAG_TX_EMPTY))
    {
    }
    USART_WriteData(U4_USART_UNIT, b);

    /* 等待发送完成（最后一个 stop bit 发完）*/
    while (RESET == USART_GetStatus(U4_USART_UNIT, USART_FLAG_TX_CPLT))
    {
    }
    USART_ClearStatus(U4_USART_UNIT, USART_FLAG_TX_CPLT);
}

void Uart4_SendStr(const char *s)
{
    while (*s)
        Uart4_SendByte((uint8_t)*s++);
}

void uart4_init(void)
{
    stc_usart_uart_init_t stcUartInit;
    stc_irq_signin_config_t stcIrqSigninConfig;

    /* Initialize DMA. */
    (void)DMA_Config();

    /* Initialize TMR0. */
    TMR0_Config(U4_USART_TIMEOUT_BITS);

    /* Configure USART RX/TX pin. */
    GPIO_SetFunc(U4_USART_RX_PORT, U4_USART_RX_PIN, U4_USART_RX_GPIO_FUNC);
    GPIO_SetFunc(U4_USART_TX_PORT, U4_USART_TX_PIN, U4_USART_TX_GPIO_FUNC);

    /* Enable peripheral clock */
    U4_USART_FCG_ENABLE();

    (void)USART_UART_StructInit(&stcUartInit);

    stcUartInit.u32ClockDiv = USART_CLK_DIV64;
    stcUartInit.u32CKOutput = USART_CK_OUTPUT_ENABLE;
    stcUartInit.u32Baudrate = U4_USART_BAUDRATE;
    stcUartInit.u32OverSampleBit = USART_OVER_SAMPLE_8BIT;
    if (LL_OK != USART_UART_Init(U4_USART_UNIT, &stcUartInit, NULL))
    {
        led_red_on();
        for (;;)
        {
        }
    }

    /* Register TX complete IRQ handler. */
    stcIrqSigninConfig.enIRQn = U4_USART_TX_CPLT_IRQn;
    stcIrqSigninConfig.enIntSrc = U4_USART_TX_CPLT_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART_TxComplete_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

    /* Register RX error IRQ handler. */
    stcIrqSigninConfig.enIRQn = U4_USART_RX_ERR_IRQn;
    stcIrqSigninConfig.enIntSrc = U4_USART_RX_ERR_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART_RxError_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);

    /* Register RX timeout IRQ handler. */
    stcIrqSigninConfig.enIRQn = U4_USART_RX_TIMEOUT_IRQn;
    stcIrqSigninConfig.enIntSrc = U4_USART_RX_TIMEOUT_INT_SRC;
    stcIrqSigninConfig.pfnCallback = &USART_RxTimeout_IrqCallback;
    (void)INTC_IrqSignIn(&stcIrqSigninConfig);
    NVIC_ClearPendingIRQ(stcIrqSigninConfig.enIRQn);
    NVIC_SetPriority(stcIrqSigninConfig.enIRQn, DDL_IRQ_PRIO_DEFAULT);
    NVIC_EnableIRQ(stcIrqSigninConfig.enIRQn);
}

void enable_uart4(void)
{
    /* Enable TX && RX && RX interrupt function */
    USART_FuncCmd(U4_USART_UNIT, (USART_RX | USART_INT_RX | USART_RX_TIMEOUT | USART_INT_RX_TIMEOUT), ENABLE);
}

void uart4_send_data(uint8_t *data, uint16_t len)
{
    if (len >= APP_FRAME_LEN_MAX)
    {
        return;
    }

    memcpy(u4_tx_buf, data, len);
    u4_tx_len = len;

    DMA_SetSrcAddr(U4_TX_DMA_UNIT, U4_TX_DMA_CH, (uint32_t)u4_tx_buf);

    DMA_SetTransCount(U4_TX_DMA_UNIT, U4_TX_DMA_CH, u4_tx_len);

    (void)DMA_ChCmd(U4_TX_DMA_UNIT, U4_TX_DMA_CH, ENABLE);

    USART_FuncCmd(U4_USART_UNIT, USART_TX, ENABLE);
}

void u4_task(void)
{

}

