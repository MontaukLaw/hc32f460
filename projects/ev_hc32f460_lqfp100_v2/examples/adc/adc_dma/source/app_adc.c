#include "user_comm.h"

uint16_t m_au16AdcBuf[ADC_BUF_LEN];
__IO static uint8_t m_u8AdcHalf = 0U;
__IO static uint8_t m_u8AdcFull = 0U;

static void AdcInitConfig(void);
static void AdcSetPinAnalogMode(void);
static void DmaIrqConfig(void);
static void DMA_IrqCallback(void);

static void AdcSetPinAnalogMode(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinAttr = PIN_ATTR_ANALOG;
    (void)GPIO_Init(ADC_CHX_PORT, ADC_CHX_PIN, &stcGpioInit);
    
}

static void AdcInitConfig(void)
{

    stc_adc_init_t stcAdcInit;

    // uint8_t au8Adc1SaSampTime[1] = {0x30};

    FCG_Fcg3PeriphClockCmd(ADC_PERIPH_CLK, ENABLE);

    (void)ADC_StructInit(&stcAdcInit);

    stcAdcInit.u16ScanMode = ADC_MD_SEQA_CONT;      // ★连续模式（用你头文件真实值）
    stcAdcInit.u16Resolution = ADC_RESOLUTION_8BIT; // ★8bit（同样用你头文件枚举值）
    stcAdcInit.u16DataAlign = ADC_DATAALIGN_RIGHT;  // ★右对齐（用真实枚举值）

    (void)ADC_Init(ADC_UNIT, &stcAdcInit);

    ADC_SetSampleTime(ADC_UNIT, ADC_CHX, 0x30);

    AdcSetPinAnalogMode();
    ADC_ChCmd(ADC_UNIT, ADC_SEQ, ADC_CHX, ENABLE); // ★只开一个通道

    ADC_ClearStatus(ADC_UNIT, ADC_FLAG_EOCA);
    ADC_IntCmd(ADC_UNIT, ADC_INT_EOCA, ENABLE); // 关键：让 EOCA 事件/中断路径打开
}

static void adcDmaConfig(void)
{
    stc_dma_init_t stcDmaInit;
    stc_dma_repeat_init_t stcDmaRptInit;

    (void)DMA_StructInit(&stcDmaInit);

    stcDmaInit.u32IntEn = 0; // ADC_DMA_INT_TYPE; 
    stcDmaInit.u32SrcAddr = ADC_DMA_SRC_ADDR;
    stcDmaInit.u32DestAddr = ADC_DMA_DEST_ADDR;
    stcDmaInit.u32DataWidth = ADC_DMA_DATA_WIDTH;
    stcDmaInit.u32BlockSize = ADC_DMA_BLOCK_SIZE;  // 1
    stcDmaInit.u32TransCount = ADC_DMA_TRANS_CNT;  // 0
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;   // ★源固定
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC; // ★目的递增

    FCG_Fcg0PeriphClockCmd(ADC_DMA_PERIPH_CLK, ENABLE);
    (void)DMA_Init(ADC_DMA_UNIT, ADC_DMA_CH, &stcDmaInit);

    stcDmaRptInit.u32Mode = DMA_RPT_DEST; // DMA_RPT_DEST; // ★优先只让 DEST 循环（如果没有 DEST 模式就用 DMA_RPT_BOTH）
    stcDmaRptInit.u32SrcCount = 1U;
    stcDmaRptInit.u32DestCount = ADC_BUF_LEN;
    (void)DMA_RepeatInit(ADC_DMA_UNIT, ADC_DMA_CH, &stcDmaRptInit);

    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);
    AOS_SetTriggerEventSrc(ADC_DMA_AOS_TRIG_SEL, ADC_DMA_TRIG_EVT);

    /* DMA IRQ configuration. */
    DmaIrqConfig();

    DMA_Cmd(ADC_DMA_UNIT, ENABLE);
    DMA_ChCmd(ADC_DMA_UNIT, ADC_DMA_CH, ENABLE);
}

void AdcConfig(void)
{

    AdcInitConfig();
    adcDmaConfig();

    ADC_Start(ADC_UNIT);
}

void adc_oneshot_test(void)
{
    stc_adc_init_t init;
    ADC_StructInit(&init);

    init.u16ScanMode = ADC_MD_SEQA_SINGLESHOT; // ★先单次
    init.u16Resolution = ADC_RESOLUTION_12BIT; // ★先12bit更直观
    init.u16DataAlign = ADC_DATAALIGN_RIGHT;

    ADC_Init(CM_ADC1, &init);

    // PB1 analog
    stc_gpio_init_t gi;
    GPIO_StructInit(&gi);
    gi.u16PinAttr = PIN_ATTR_ANALOG;
    GPIO_Init(GPIO_PORT_B, GPIO_PIN_01, &gi);

    // 选 CH9
    ADC_ChCmd(CM_ADC1, ADC_SEQ_A, ADC_CH9, ENABLE);
    ADC_SetSampleTime(CM_ADC1, ADC_CH9, 0x30);

    // 关触发（防止默认硬触发影响）
    ADC_TriggerCmd(CM_ADC1, ADC_SEQ_A, DISABLE);
}

/**
 * @brief  DMA interrupt configuration.
 * @param  None
 * @retval None
 */
static void DmaIrqConfig(void)
{
    stc_irq_signin_config_t stcIrqSignConfig;

    stcIrqSignConfig.enIntSrc = ADC_DMA_INT_SRC;
    stcIrqSignConfig.enIRQn = ADC_DMA_INT_IRQn;
    stcIrqSignConfig.pfnCallback = &DMA_IrqCallback;

    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    DMA_ClearTransCompleteStatus(ADC_DMA_UNIT, ADC_DMA_INT_FLAG);

    /* NVIC setting */
    NVIC_ClearPendingIRQ(ADC_DMA_INT_IRQn);
    NVIC_SetPriority(ADC_DMA_INT_IRQn, ADC_DMA_INT_PRIO);
    NVIC_EnableIRQ(ADC_DMA_INT_IRQn);
}

/**
 * @brief  DMA IRQ handler.
 * @param  None
 * @retval None
 */
static void DMA_IrqCallback(void)
{
    DMA_ClearTransCompleteStatus(ADC_DMA_UNIT, ADC_DMA_INT_FLAG);
}
