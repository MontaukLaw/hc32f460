#include "user_comm.h"

#define DELAY_TMRA_UNIT CM_TMRA_1

static uint8_t Delay_TMRA_GetClockDiv(uint32_t tmra_clk_hz)
{
    switch (tmra_clk_hz)
    {
    case 1000000UL:
        return TMRA_CLK_DIV1;
    case 2000000UL:
        return TMRA_CLK_DIV2;
    case 4000000UL:
        return TMRA_CLK_DIV4;
    case 8000000UL:
        return TMRA_CLK_DIV8;
    case 16000000UL:
        return TMRA_CLK_DIV16;
    case 32000000UL:
        return TMRA_CLK_DIV32;
    case 64000000UL:
        return TMRA_CLK_DIV64;
    case 128000000UL:
        return TMRA_CLK_DIV128;
    case 256000000UL:
        return TMRA_CLK_DIV256;
    case 512000000UL:
        return TMRA_CLK_DIV512;
    case 1024000000UL:
        return TMRA_CLK_DIV1024;
    default:
        return 0xFFU;
    }
}

void DelayUs_Init(void)
{
    stc_tmra_init_t stcTmraInit;
    stc_clock_freq_t stcClk;
    uint32_t tmra_clk_hz;
    uint8_t clkDiv;

    /* 1) 开 TMRA 模块时钟
       这里的宏名字请按你的工程实际改。
       常见会类似 FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_1, ENABLE); */
    FCG_Fcg2PeriphClockCmd(FCG2_PERIPH_TMRA_1, ENABLE);

    /* 5) 默认结构体 */
    TMRA_StructInit(&stcTmraInit);

    stcTmraInit.u8CountSrc = TMRA_CNT_SRC_SW;
    stcTmraInit.sw_count.u8ClockDiv = TMRA_CLK_DIV64;
    stcTmraInit.sw_count.u8CountMode = TMRA_MD_SAWTOOTH;
    stcTmraInit.sw_count.u8CountDir = TMRA_DIR_UP;
    stcTmraInit.u32PeriodValue = 0xFFFFUL;

    /* 6) 初始化 */
    TMRA_DeInit(DELAY_TMRA_UNIT);
    TMRA_Init(DELAY_TMRA_UNIT, &stcTmraInit);

    /* 7) 清计数值 */
    TMRA_SetCountValue(DELAY_TMRA_UNIT, 0UL);

    /* 8) 启动 */
    TMRA_Start(DELAY_TMRA_UNIT);
}

void delay_tick(uint16_t tick)
{
    uint16_t start = (uint16_t)TMRA_GetCountValue(DELAY_TMRA_UNIT);

    while ((uint16_t)((uint16_t)TMRA_GetCountValue(DELAY_TMRA_UNIT) - start) < tick)
    {
    }
}

// 1 tick = 0.64us，
// 这个delay不怕回绕
void delay_us(uint16_t us)
{

    uint16_t start = (uint16_t)TMRA_GetCountValue(DELAY_TMRA_UNIT);

    while ((uint16_t)((uint16_t)TMRA_GetCountValue(DELAY_TMRA_UNIT) - start) < (us * 1000UL / 640UL))
    {
    }

}

void delay_ms(uint32_t ms)
{
    while (ms--)
    {
        delay_us(1000U);
    }
}

