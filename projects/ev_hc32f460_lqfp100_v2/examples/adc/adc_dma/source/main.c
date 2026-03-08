#include "user_comm.h"

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

/**
 * @brief  SysTick interrupt handler function.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
    SysTick_IncTick();

    __DSB();  /* Arm Errata 838869 */
}


int32_t main(void)
{

    /* System clock config */
    init_clock();

    /* MCU Peripheral registers write unprotected. */
    LL_PERIPH_WE(LL_PERIPH_GPIO | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_EFM | LL_PERIPH_SRAM);

    /* Configures ADC. */
    AdcConfig();

    LED_Init();

    gpio_init();

    DelayUs_Init();
    
    uart2_init();
    uart4_init();
    
    SysTick_Init(1000U);
    NVIC_SetPriority(SysTick_IRQn, DDL_IRQ_PRIO_14);

    /* MCU Peripheral registers write protected. */
    LL_PERIPH_WP(LL_PERIPH_GPIO | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_EFM | LL_PERIPH_SRAM);

    uint16_t v = 0;
    led_test();
    stc_clock_freq_t clkFreq;
    CLK_GetClockFreq(&clkFreq);

    enable_uart4();
    enable_uart2();

    fill_tx_data();

    // 打开输出0
    // set_output(0, 1);

    // 打开开关中的通道0
    // set_ch(0);
    // delay init
    // bl_at_mac();
    while (1)
    {

        main_task_adc_first();

        bl_link_status_check();
		
        bl_task();

        led_task();

        com_task();

        ble_task();

        // test_task();
        // u4_task();

        // u2_task();
    }
    
    while (1)
    {

        // DDL_Printf("Running\r\n");
        GPIO_TogglePins(TEST_POINT_PORT, TEST_POINT_PIN);
        // Uart_SendStr("Please send data to USART4 (Max length: 500 bytes) ...\r\n");
        uart4_send_data((uint8_t *)"u4 hello\r\n", 7);
        uart2_send_data((uint8_t *)"u2 hello\r\n", 7);
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

