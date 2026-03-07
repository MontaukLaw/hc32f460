#include "user_comm.h"

static void XtalInit(void);
static void MPLLInit(void);

static uint8_t au8SysClockTbl[] = {
    CLK_SYSCLK_SRC_HRC,
    CLK_SYSCLK_SRC_MRC,
    CLK_SYSCLK_SRC_LRC,
    CLK_SYSCLK_SRC_XTAL,
    CLK_SYSCLK_SRC_XTAL32,
    CLK_SYSCLK_SRC_PLL,
};

static void XtalInit(void)
{
    stc_clock_xtal_init_t stcXtalInit;

    /* XTAL config */
    GPIO_AnalogCmd(BSP_XTAL_PORT, BSP_XTAL_PIN, ENABLE);
    (void)CLK_XtalStructInit(&stcXtalInit);
    /* Config Xtal and Enable Xtal */
    stcXtalInit.u8State = CLK_XTAL_ON;
    stcXtalInit.u8Mode = CLK_XTAL_MD_OSC;
    stcXtalInit.u8Drv = CLK_XTAL_DRV_ULOW;
    stcXtalInit.u8StableTime = CLK_XTAL_STB_2MS;
    (void)CLK_XtalInit(&stcXtalInit);
}

/**
 * @brief  MPLL initialize
 * @param  None
 * @retval None
 */
static void MPLLInit(void)
{
    stc_clock_pll_init_t      stcMPLLInit;

    (void)CLK_PLLStructInit(&stcMPLLInit);
    /* MPLL config */
    /* 8MHz/M*N = 8/1*50/2 = 200MHz */
    stcMPLLInit.PLLCFGR = 0UL;
    stcMPLLInit.PLLCFGR_f.PLLM = (1UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLN = (50UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLP = (2UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLQ = (2UL - 1UL);
    stcMPLLInit.PLLCFGR_f.PLLR = (2UL - 1UL);
    stcMPLLInit.u8PLLState = CLK_PLL_ON;
    stcMPLLInit.PLLCFGR_f.PLLSRC = CLK_PLL_SRC_XTAL;     /* Xtal = 8MHz */
    (void)CLK_PLLInit(&stcMPLLInit);
}

// 这个函数是整个系统时钟配置的入口函数
// 主要功能包括：
// 1. 配置总线时钟分频
// 2. 配置SRAM和Flash的等待周期
// 3. 初始化外部晶振和主PLL
// 4. 启用LRC和HRC
// 5. 切换系统时钟源为PLL
void init_clock(void)
{
    uint8_t i = 0U;

    /* Register write unprotected for some required peripherals. */
    LL_PERIPH_WE(LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_GPIO | LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_SRAM);

    /* Set bus clock div. */
    CLK_SetClockDiv(CLK_BUS_CLK_ALL, (CLK_HCLK_DIV1 | CLK_EXCLK_DIV2 | CLK_PCLK0_DIV1 | CLK_PCLK1_DIV2 |
                                      CLK_PCLK2_DIV4 | CLK_PCLK3_DIV4 | CLK_PCLK4_DIV2));
    /* BSP key initialize */
    // BSP_KEY_Init();
    
    /* sram init include read/write wait cycle setting */
    SRAM_SetWaitCycle(SRAM_SRAM_ALL, SRAM_WAIT_CYCLE1, SRAM_WAIT_CYCLE1);
    SRAM_SetWaitCycle(SRAM_SRAMH, SRAM_WAIT_CYCLE0, SRAM_WAIT_CYCLE0);
    
    /* flash read wait cycle setting */
    EFM_SetWaitCycle(EFM_WAIT_CYCLE5);
    /* output system clock */
    // 暂时不用, gpio不够
    // MCOInit();
    
    /* Xtal initialize */
    XtalInit();
    
    /* MPLL initialize */
    MPLLInit();
    
    /* enable LRC */
    (void)CLK_LrcCmd(ENABLE);
    /* enable HRC */
    (void)CLK_HrcCmd(ENABLE);
    
    /* Switch driver ability */
    PWC_HighSpeedToHighPerformance();
    
    /* Register write protected for some required peripherals. */
    LL_PERIPH_WP(LL_PERIPH_EFM | LL_PERIPH_GPIO | LL_PERIPH_SRAM);
    
    // 设置系统时钟源为PLL
    CLK_SetSysClockSrc(au8SysClockTbl[5]);

}

