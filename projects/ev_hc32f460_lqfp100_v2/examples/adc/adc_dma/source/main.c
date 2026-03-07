#include "user_comm.h"

#define ADC_UNIT (CM_ADC1)

#define ADC_PERIPH_CLK (FCG3_PERIPH_ADC1)
/* --- single channel --- */
#define ADC_CHX (ADC_CH9)
#define ADC_CHX_PORT (GPIO_PORT_B)
#define ADC_CHX_PIN (GPIO_PIN_01)
#define ADC_SEQ (ADC_SEQ_A)

/* DR for this channel */
#define ADC_DR_ADDR ((uint32_t)&ADC_UNIT->DR9) // CH9 -> DR9（按你示例习惯）

/* ring buffer length */
#define ADC_BUF_LEN (64U)
#define DMA_SRC_ADDR (ADC_DR_ADDR)
#define DMA_TRANS_CNT (0U)  // always
#define DMA_BLOCK_SIZE (1U) // ★每次触发搬1个
#define DMA_DATA_WIDTH (DMA_DATAWIDTH_8BIT)
#define DMA_DEST_ADDR ((uint32_t)(&m_au16AdcBuf[0U]))
#define DMA_TRIG_EVT (EVT_SRC_ADC1_EOCA)

#define DMA_UNIT (CM_DMA1)
#define DMA_PERIPH_CLK (FCG0_PERIPH_DMA1)
#define DMA_CH (DMA_CH0)
#define DMA_AOS_TRIG_SEL (AOS_DMA1_0)

#define DMA_INT_TYPE (DMA_INT_BTC_CH0)
#define DMA_INT_SRC (INT_SRC_DMA1_BTC0)
#define DMA_INT_IRQn (INT038_IRQn)
#define DMA_INT_PRIO (DDL_IRQ_PRIO_03)
#define DMA_INT_FLAG (DMA_FLAG_BTC_CH0)

static uint8_t m_au16AdcBuf[ADC_BUF_LEN];
__IO static uint8_t m_u8AdcHalf = 0U;
__IO static uint8_t m_u8AdcFull = 0U;

static void AdcInitConfig(void);
static void AdcSetPinAnalogMode(void);
static void SystemClockConfig(void);
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

static void DmaConfig(void)
{
    stc_dma_init_t stcDmaInit;
    stc_dma_repeat_init_t stcDmaRptInit;

    (void)DMA_StructInit(&stcDmaInit);

    stcDmaInit.u32IntEn = DMA_INT_ENABLE; // 先关中断，跑通再开
    stcDmaInit.u32SrcAddr = DMA_SRC_ADDR;
    stcDmaInit.u32DestAddr = DMA_DEST_ADDR;
    stcDmaInit.u32DataWidth = DMA_DATA_WIDTH;
    stcDmaInit.u32BlockSize = DMA_BLOCK_SIZE;      // 1
    stcDmaInit.u32TransCount = DMA_TRANS_CNT;      // 0
    stcDmaInit.u32SrcAddrInc = DMA_SRC_ADDR_FIX;   // ★源固定
    stcDmaInit.u32DestAddrInc = DMA_DEST_ADDR_INC; // ★目的递增

    FCG_Fcg0PeriphClockCmd(DMA_PERIPH_CLK, ENABLE);
    (void)DMA_Init(DMA_UNIT, DMA_CH, &stcDmaInit);

    stcDmaRptInit.u32Mode = DMA_RPT_DEST; // DMA_RPT_DEST; // ★优先只让 DEST 循环（如果没有 DEST 模式就用 DMA_RPT_BOTH）
    stcDmaRptInit.u32SrcCount = 1U;
    stcDmaRptInit.u32DestCount = ADC_BUF_LEN;
    (void)DMA_RepeatInit(DMA_UNIT, DMA_CH, &stcDmaRptInit);

    FCG_Fcg0PeriphClockCmd(FCG0_PERIPH_AOS, ENABLE);
    AOS_SetTriggerEventSrc(DMA_AOS_TRIG_SEL, DMA_TRIG_EVT);

    /* DMA IRQ configuration. */
    DmaIrqConfig();

    DMA_Cmd(DMA_UNIT, ENABLE);
    DMA_ChCmd(DMA_UNIT, DMA_CH, ENABLE);
}

static void AdcConfig(void)
{

    AdcInitConfig();
    // DmaConfig();

    // ★启动一次后持续跑（请用你头文件里的真实启动函数）
    ADC_Start(ADC_UNIT);
}

static void adc_oneshot_test(void)
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

#if 0
/**
 * @brief  Set XTAL as system clock source.
 * @param  None
 * @retval None
 */
static void SystemClockConfig(void)
{
    stc_clock_xtal_init_t stcXtalInit;

    /* XTAL config */
    GPIO_AnalogCmd(BSP_XTAL_PORT, BSP_XTAL_PIN, ENABLE);
    (void)CLK_XtalStructInit(&stcXtalInit);
    /* Config XTAL and Enable XTAL */
    stcXtalInit.u8State = CLK_XTAL_ON;
    stcXtalInit.u8Mode = CLK_XTAL_MD_OSC;
    stcXtalInit.u8Drv = CLK_XTAL_DRV_LOW; // CLK_XTAL_DRV_ULOW;
    stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;
    (void)CLK_XtalInit(&stcXtalInit);

    // CLK_SetSysClockSrc(CLK_SYSCLK_SRC_XTAL);
}
int32_t main_(void)
{
    LL_PERIPH_WE(LL_PERIPH_GPIO | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU);
    SystemClockConfig();
    DDL_PrintfInit(BSP_PRINTF_DEVICE, BSP_PRINTF_BAUDRATE, BSP_PRINTF_Preinit);
    adc_oneshot_test();
    LL_PERIPH_WP(LL_PERIPH_GPIO | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU);

    // 清标志
    ADC_ClearStatus(CM_ADC1, ADC_FLAG_EOCA);

    // 开始一次
    ADC_Start(CM_ADC1);

    // 等待完成
    while (RESET == ADC_GetStatus(CM_ADC1, ADC_FLAG_EOCA))
    {
    }

    // 读数
    uint16_t v = ADC_GetValue(CM_ADC1, ADC_CH9);
    DDL_Printf("ADC_CH9=%u\r\n", v);

    ADC_ClearStatus(CM_ADC1, ADC_FLAG_EOCA);
    while (1)
        ;
}

static void TargetClockInit(void)
{
    stc_clock_xtal32_init_t stcXtal32Init;
    stc_clock_pll_init_t stcMPLLInit;
    stc_clock_pllx_init_t stcUPLLInit;

    /* Xtal32 config */
    GPIO_AnalogCmd(BSP_XTAL32_PORT, BSP_XTAL32_PIN, DISABLE);

    (void)CLK_HrcCmd(ENABLE);
    (void)CLK_MrcCmd(ENABLE);
    (void)CLK_LrcCmd(ENABLE);

    /* PCLK0, HCLK  Max 200MHz */
    /* PCLK1, PCLK4 Max 100MHz */
    /* PCLK2, PCLK3 Max 60MHz  */
    /* EX BUS Max 100MHz */
    CLK_SetClockDiv(CLK_BUS_CLK_ALL,
                    (CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 | CLK_PCLK2_DIV4 |
                     CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2 | CLK_EXCLK_DIV2 |
                     CLK_HCLK_DIV1));

    /* PLLH config */
    (void)CLK_PLLStructInit(&stcMPLLInit);
    /**
        VCO = 8/1*50 = 400MHz
        8MHz/M*N = 8/1*50/2 =200MHz
    */
    stcMPLLInit.u8PLLState = CLK_PLLX_ON;
    stcMPLLInit.PLLCFGR = 0UL;

#if 1
    stcMPLLInit.PLLCFGR_f.PLLM = (1UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLN = (50UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLR = (2UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLQ = (2UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLP = (5UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL; /* Xtal = 8MHz */
#endif

#if 0
    stcMPLLInit.PLLCFGR_f.PLLM = (1UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLN = (50UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLP = (2UL - 1UL); // 关键改这里
    stcMPLLInit.PLLCFGR_f.PLLQ = (2UL - 1UL); // 按需
    stcMPLLInit.PLLCFGR_f.PLLR = (2UL - 1UL); // 按需
    stcMPLLInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL;
#endif
    (void)CLK_PLLInit(&stcMPLLInit);

    /* PLLA config */
    (void)CLK_PLLxStructInit(&stcUPLLInit);
    /**
        VCO = 8/2*100 = 400MHz
        8MHz/M*N = 8/2*100/2 =200MHz
    */
    stcUPLLInit.u8PLLState = CLK_PLLX_ON;
    stcUPLLInit.PLLCFGR = 0UL;
    stcUPLLInit.PLLCFGR_f.PLLM = (2UL - 1UL);
    stcUPLLInit.PLLCFGR_f.PLLN = (100UL - 1UL);
    stcUPLLInit.PLLCFGR_f.PLLR = (2UL - 1UL);
    stcUPLLInit.PLLCFGR_f.PLLQ = (2UL - 1UL);
    stcUPLLInit.PLLCFGR_f.PLLP = (8UL - 1UL);

    (void)CLK_PLLxInit(&stcUPLLInit);

    CLK_SetSysClockSrc(CLK_SYSCLK_SRC_PLL);

    SystemCoreClockUpdate();
}

#endif

int32_t main(void)
{

    init_clock();

    /* MCU Peripheral registers write unprotected. */
    LL_PERIPH_WE(LL_PERIPH_GPIO | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_EFM | LL_PERIPH_SRAM);

    /* System clock config */

    /* Configures ADC. */
    AdcConfig();

    LED_Init();

    gpio_init();

    DelayUs_Init();

    uart4_init();
    uart2_init();

    /* MCU Peripheral registers write protected. */
    LL_PERIPH_WP(LL_PERIPH_GPIO | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_EFM | LL_PERIPH_SRAM);

    uint16_t v = 0;
    led_test();
    stc_clock_freq_t clkFreq;
    CLK_GetClockFreq(&clkFreq);

    enable_uart4();
    enable_uart2();

    // 打开输出0
    // set_output(0, 1);

    // 打开开关中的通道0
    // set_ch(0);
    // delay init

    while (1)
    {
        GPIO_TogglePins(TEST_POINT_PORT, TEST_POINT_PIN);
        // Uart_SendStr("Please send data to USART4 (Max length: 500 bytes) ...\r\n");
        uart4_send_data((uint8_t *)"hello\r\n", 7);
        uart2_send_data((uint8_t *)"hello\r\n", 7);        
        delay_ms(100);

        // DDL_DelayMS(50);
    }

    uint16_t i;
    while (0)
    {
        GPIO_SetPins(TEST_POINT_PORT, TEST_POINT_PIN);
        // DDL_DelayUS(100);
        delay_us(100);
        // delay_tick(100);
        GPIO_ResetPins(TEST_POINT_PORT, TEST_POINT_PIN);
        delay_us(100);
        // delay_tick(100);
        // DDL_DelayUS(100);
    }
    while (1)
    {
        for (i = 0; i < 16; i++)
        {
            set_output(i, 1);
            delay_us(1);
            set_output(i, 0);
            delay_us(1);
        }
    }

    while (1)
    {
        v = ADC_GetValue(ADC_UNIT, ADC_CHX);
        DDL_Printf("adc=%u\r\n", v);
        DDL_DelayMS(200);
    }
}

/**
 * @brief  DMA interrupt configuration.
 * @param  None
 * @retval None
 */
static void DmaIrqConfig(void)
{
    stc_irq_signin_config_t stcIrqSignConfig;

    stcIrqSignConfig.enIntSrc = DMA_INT_SRC;
    stcIrqSignConfig.enIRQn = DMA_INT_IRQn;
    stcIrqSignConfig.pfnCallback = &DMA_IrqCallback;

    (void)INTC_IrqSignIn(&stcIrqSignConfig);
    DMA_ClearTransCompleteStatus(DMA_UNIT, DMA_INT_FLAG);

    /* NVIC setting */
    NVIC_ClearPendingIRQ(DMA_INT_IRQn);
    NVIC_SetPriority(DMA_INT_IRQn, DMA_INT_PRIO);
    NVIC_EnableIRQ(DMA_INT_IRQn);
}

/**
 * @brief  DMA IRQ handler.
 * @param  None
 * @retval None
 */
static void DMA_IrqCallback(void)
{
    DMA_ClearTransCompleteStatus(DMA_UNIT, DMA_INT_FLAG);
}
