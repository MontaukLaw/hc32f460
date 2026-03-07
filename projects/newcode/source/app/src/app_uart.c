#include "allinc.h"

xQueueHandle uartqueue = NULL;

#define PWC_FCG0_PERIPH_PTDIS ((uint32_t)0x00020000)

#define DMA_UNIT (M4_DMA1)
#define DMA_CH_RX (DmaCh0)
#define DMA_RX_TRG_SEL (EVT_USART1_RI)

#define DMA_CH_TX (DmaCh1)
#define DMA_TX_TRG_SEL (EVT_USART1_TI)

/* USART channel definition */
#define USART_CH (M4_USART1)

/* USART baudrate definition */
#define USART_BAUDRATE (1000000)

/* USART RX Port/Pin definition */
#define USART_RX_PORT (PortA)
#define USART_RX_PIN (Pin02)
#define USART_RX_FUNC (Func_Usart1_Rx)

/* USART TX Port/Pin definition */
#define USART_TX_PORT (PortA)
#define USART_TX_PIN (Pin03)
#define USART_TX_FUNC (Func_Usart1_Tx)

/* USART interrupt  */
#define USART_EI_NUM (INT_USART1_EI)
#define USART_EI_IRQn (Int001_IRQn)

/* DMA block transfer complete interrupt */
#define DMA_TC0_INT_NUM (INT_DMA1_TC0)
#define DMA_TC0_INT_IRQn (Int002_IRQn)

#define DMA_TC1_INT_NUM (INT_DMA1_TC1)
#define DMA_TC1_INT_IRQn (Int003_IRQn)

#define TIMER6_OVF_INT_NUM (INT_TMR61_GOVF)
#define TIMER6_OVF_INT_IRQn (Int004_IRQn)

#define TXSIZE 1500

uint8_t u8RxBuffer1[128];
uint8_t u8TxBuffer1[TXSIZE];
uint8_t u8FlagDMATxTrans;
uint8_t u8FlagDMARxFinished;
uint16_t u16RxBuffSize;

#define NOTCOMPLETE 1
#define COMPLETE 0

static void DmaInit(void);
static void Timer61Init(void);
static void DmaTxIrqCallback(void);
static void DmaRxIrqCallback(void);
static void UsartErrIrqCallback(void);
static void Timer6_OverFlow_CallBack(void);

static void DmaInit(void)
{
    stc_dma_config_t stcDmaInit;
    stc_irq_regi_conf_t stcIrqRegiCfg;

    /* Enable peripheral clock */
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_DMA1, Enable);
    /* Enable peripheral circuit trigger function. */
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS, Enable);

    /* Enable DMA. */
    DMA_Cmd(DMA_UNIT, Enable);

    /* Initialize DMA CH0 for Rx. */
    MEM_ZERO_STRUCT(stcDmaInit);
    stcDmaInit.u16TransferCnt = 128;
    stcDmaInit.u16BlockSize = 1u; /* 1 block */
    stcDmaInit.u16DesRptSize = 128;
    stcDmaInit.stcDmaChCfg.enDesRptEn = Enable;
    stcDmaInit.u32SrcAddr = ((uint32_t)(&USART_CH->DR) + 2ul); /* Set source address. */
    stcDmaInit.u32DesAddr = (uint32_t)(&u8RxBuffer1[0]);       /* Set destination address. */
    stcDmaInit.stcDmaChCfg.enSrcInc = AddressFix;              /* Set source address mode. */
    stcDmaInit.stcDmaChCfg.enDesInc = AddressIncrease;         /* Set destination address mode. */
    stcDmaInit.stcDmaChCfg.enIntEn = Enable;                   /* Enable interrupt. */
    stcDmaInit.stcDmaChCfg.enTrnWidth = Dma8Bit;               /* Set data width 8bit. */
    DMA_InitChannel(DMA_UNIT, DMA_CH_RX, &stcDmaInit);

    DMA_ChannelCmd(DMA_UNIT, DMA_CH_RX, Enable);           /* Enable the specified DMA channel. */
    DMA_ClearIrqFlag(DMA_UNIT, DMA_CH_RX, TrnCpltIrq);     /* Clear DMA flag. */
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS, Enable); /* Enable peripheral circuit trigger function. */

    /* Set DMA trigger source. */
    DMA_SetTriggerSrc(DMA_UNIT, DMA_CH_RX, DMA_RX_TRG_SEL);

    /* Set DMA block transfer complete IRQ */
    stcIrqRegiCfg.enIRQn = DMA_TC0_INT_IRQn;
    stcIrqRegiCfg.pfnCallback = &DmaRxIrqCallback;
    stcIrqRegiCfg.enIntSrc = DMA_TC0_INT_NUM;
    enIrqRegistration(&stcIrqRegiCfg);
    NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_08);
    NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);

    /* Initialize DMA CH1 for Tx. */
    MEM_ZERO_STRUCT(stcDmaInit);
    stcDmaInit.u16TransferCnt = TXSIZE;
    stcDmaInit.u16BlockSize = 1; /* 1 block */
    // stcDmaInit.u16SrcRptSize = TXSIZE;
    stcDmaInit.stcDmaChCfg.enSrcRptEn = Disable;
    stcDmaInit.u32SrcAddr = ((uint32_t)(&u8TxBuffer1[0])); /* Set source address. */
    stcDmaInit.u32DesAddr = (uint32_t)(&USART_CH->DR);     /* Set destination address. */
    stcDmaInit.stcDmaChCfg.enSrcInc = AddressIncrease;     /* Set source address mode. */
    stcDmaInit.stcDmaChCfg.enDesInc = AddressFix;          /* Set destination address mode. */
    stcDmaInit.stcDmaChCfg.enIntEn = Enable;               /* Enable interrupt. */
    stcDmaInit.stcDmaChCfg.enTrnWidth = Dma8Bit;           /* Set data width 8bit. */
    DMA_InitChannel(DMA_UNIT, DMA_CH_TX, &stcDmaInit);

    DMA_ChannelCmd(DMA_UNIT, DMA_CH_TX, Enable);            /* Enable the specified DMA channel. */
    DMA_ClearIrqFlag(DMA_UNIT, DMA_CH_TX, TrnCpltIrq);      /* Clear DMA flag. */
    DMA_SetTriggerSrc(DMA_UNIT, DMA_CH_TX, DMA_TX_TRG_SEL); /* Set DMA trigger source. */

    /* Set DMA block transfer complete IRQ */
    stcIrqRegiCfg.enIRQn = DMA_TC1_INT_IRQn;
    stcIrqRegiCfg.pfnCallback = &DmaTxIrqCallback;
    stcIrqRegiCfg.enIntSrc = DMA_TC1_INT_NUM;
    enIrqRegistration(&stcIrqRegiCfg);
    NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_08);
    NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
}

/**
 *******************************************************************************
 ** \brief Initialize Timer61.
 **
 ** \param [in] None
 **
 ** \retval None
 **
 ******************************************************************************/
static void Timer61Init(void)
{
    stc_timer6_basecnt_cfg_t stcTIM6BaseCntCfg;
    stc_timer6_port_output_cfg_t stcTIM6PWMxCfg;
    stc_timer6_gcmp_buf_cfg_t stcGCMPBufCfg;
    stc_port_init_t stcPortInit;
    stc_irq_regi_conf_t stcIrqRegiConf;

    MEM_ZERO_STRUCT(stcTIM6BaseCntCfg);
    MEM_ZERO_STRUCT(stcTIM6PWMxCfg);
    MEM_ZERO_STRUCT(stcGCMPBufCfg);
    MEM_ZERO_STRUCT(stcPortInit);
    MEM_ZERO_STRUCT(stcIrqRegiConf);

    PWC_Fcg2PeriphClockCmd(PWC_FCG2_PERIPH_TIM61, Enable);
    PWC_Fcg0PeriphClockCmd(PWC_FCG0_PERIPH_PTDIS, Enable);

    stcTIM6BaseCntCfg.enCntMode = Timer6CntSawtoothMode; // saw wave mode
    stcTIM6BaseCntCfg.enCntDir = Timer6CntDirUp;         // Counter counting up
    stcTIM6BaseCntCfg.enCntClkDiv = Timer6PclkDiv64;     // Count clock: pclk0/64
    Timer6_Init(M4_TMR61, &stcTIM6BaseCntCfg);           // timer6 PWM frequency, count mode and clk config

    Timer6_SetPeriod(M4_TMR61, Timer6PeriodA, 2625); // period set 1ms

    Timer6_SetTriggerSrc0(DMA_RX_TRG_SEL); // Set DMA_RX_TRG_SEL Event as Timer6 Trigger Source0

    Timer6_ConfigHwStart(M4_TMR61, Timer6HwTrigAos0);
    Timer6_EnableHwStart(M4_TMR61);

    Timer6_ConfigHwClear(M4_TMR61, Timer6HwTrigAos0); // Timer61 Hardware Clear Event Condition: Timer6 Trigger Source0(Timer62 OverFlow Event)
    Timer6_EnableHwClear(M4_TMR61);                   // Enable Timer61 Hardware Clear Event Condition

    Timer6_ConfigIrq(M4_TMR61, Timer6INTENOVF, true);

    stcIrqRegiConf.enIRQn = TIMER6_OVF_INT_IRQn;            // Register INT_TMR61_GUDF Int to Vect.No.002
    stcIrqRegiConf.enIntSrc = TIMER6_OVF_INT_NUM;           // Select Event interrupt of timer61
    stcIrqRegiConf.pfnCallback = &Timer6_OverFlow_CallBack; // Callback function
    enIrqRegistration(&stcIrqRegiConf);                     // Registration IRQ

    NVIC_ClearPendingIRQ(stcIrqRegiConf.enIRQn);                  // Clear Pending
    NVIC_SetPriority(stcIrqRegiConf.enIRQn, DDL_IRQ_PRIORITY_08); // Set priority
    NVIC_EnableIRQ(stcIrqRegiConf.enIRQn);                        // Enable NVIC
    
}

static void DmaRxIrqCallback(void)
{
    DMA_ClearIrqFlag(DMA_UNIT, DMA_CH_RX, TrnCpltIrq);
    DMA_SetTransferCnt(DMA_UNIT, DMA_CH_RX, 128);
    DMA_SetDesRptSize(DMA_UNIT, DMA_CH_RX, 128);
    DMA_SetDesAddress(DMA_UNIT, DMA_CH_RX, (uint32_t)(&u8RxBuffer1[0]));
    DMA_ChannelCmd(DMA_UNIT, DMA_CH_RX, Enable);
    u8FlagDMARxFinished = COMPLETE;
}

static void DmaTxIrqCallback(void)
{
    DMA_ClearIrqFlag(DMA_UNIT, DMA_CH_TX, TrnCpltIrq);
    u8FlagDMATxTrans = COMPLETE;
}
char dmarxdatap[QUENEDATAMAXLEN];
int esp32sendflag = 0;
static void Timer6_OverFlow_CallBack(void)
{

    DMA_ClearIrqFlag(DMA_UNIT, DMA_CH_RX, TrnCpltIrq);
    Timer6_StopCount(M4_TMR61);
    u16RxBuffSize = 128 - DMA_UNIT->MONDTCTL0_f.CNT;
    DMA_SetTransferCnt(DMA_UNIT, DMA_CH_RX, 128);
    DMA_SetDesRptSize(DMA_UNIT, DMA_CH_RX, 128);
    DMA_SetDesAddress(DMA_UNIT, DMA_CH_RX, (uint32_t)(&u8RxBuffer1[0]));
    DMA_ChannelCmd(DMA_UNIT, DMA_CH_RX, Enable);

    u8FlagDMARxFinished = COMPLETE;
    if (u8RxBuffer1[0] == 'A')
        esp32sendflag = 1;
#if 1
    if (u16RxBuffSize > 50)
        return;
    else
    {
        memset(dmarxdatap, 0, sizeof(dmarxdatap));
        memcpy(dmarxdatap, u8RxBuffer1, u16RxBuffSize);
        // sscanf(dmarxdatap,"%[^\r\n]",dmarxdatap);
        if (strcmp(dmarxdatap, "teststart") == 0)
        {
            printf("test start!\n");
        }

        xQueueSendFromISR(uartqueue, dmarxdatap, 0);
    }
#endif
}

static void UsartErrIrqCallback(void)
{
    if (Set == USART_GetStatus(USART_CH, UsartFrameErr))
    {
        USART_ClearStatus(USART_CH, UsartFrameErr);
    }

    if (Set == USART_GetStatus(USART_CH, UsartParityErr))
    {
        USART_ClearStatus(USART_CH, UsartParityErr);
    }

    if (Set == USART_GetStatus(USART_CH, UsartOverrunErr))
    {
        USART_ClearStatus(USART_CH, UsartOverrunErr);
    }
}

void send_data(char *data, int len)
{
    if (len > TXSIZE)
        return;
    memset(u8TxBuffer1, 0, sizeof u8TxBuffer1);
    memcpy(u8TxBuffer1, data, len);
    DMA_SetTransferCnt(DMA_UNIT, DMA_CH_TX, len);
    DMA_SetBlockSize(DMA_UNIT, DMA_CH_TX, 1);
    DMA_SetSrcAddress(DMA_UNIT, DMA_CH_TX, (uint32_t)(&u8TxBuffer1[0]));
    DMA_ChannelCmd(DMA_UNIT, DMA_CH_TX, Enable);
    USART_FuncCmd(USART_CH, UsartTx, Disable);
    USART_FuncCmd(USART_CH, UsartTxEmptyInt, Disable);
    USART_FuncCmd(USART_CH, UsartTxAndTxEmptyInt, Enable);
}

void draw_logo()
{
    float y, x, a;
    for (y = 1.3; y > -1.1; y -= 0.1f)
    {
        for (x = -1.3; x < 1.1; x += 0.05)
        {
            a = x * x + y * y - 1;
            printf("%c", a * a * a - x * x * y * y * y < 0 ? '*' : ' ');
        }
        printf("\n");
    }
}

void usart_task(void *pvParameters)
{

    while (1)
    {
#if 1
        char uartrecv[100] = "";
        if (xQueueReceive(uartqueue, uartrecv, 1000))
        {
            printf("%s", uartrecv);
        }
        vTaskDelay(10);
#endif
        // send_data("hello\n",6);
        vTaskDelay(1);
    }
}

void init_uart(void)
{

    uartqueue = xQueueCreate(10, 30);
    stc_irq_regi_conf_t stcIrqRegiCfg;
    uint32_t u32Fcg1Periph = PWC_FCG1_PERIPH_USART1 | PWC_FCG1_PERIPH_USART2 |
                             PWC_FCG1_PERIPH_USART3 | PWC_FCG1_PERIPH_USART4;
    const stc_usart_uart_init_t stcInitCfg = {
        UsartIntClkCkNoOutput,
        UsartClkDiv_1,
        UsartDataBits8,
        UsartDataLsbFirst,
        UsartOneStopBit,
        UsartParityNone,
        UsartSamleBit8,
        UsartStartBitFallEdge,
        UsartRtsEnable,
    };
    DmaInit();
    Timer61Init();
    PWC_Fcg1PeriphClockCmd(u32Fcg1Periph, Enable);
    PORT_SetFunc(USART_RX_PORT, USART_RX_PIN, USART_RX_FUNC, Disable);
    PORT_SetFunc(USART_TX_PORT, USART_TX_PIN, USART_TX_FUNC, Disable);
    USART_UART_Init(USART_CH, &stcInitCfg);
    USART_SetBaudrate(USART_CH, USART_BAUDRATE);
    stcIrqRegiCfg.enIRQn = USART_EI_IRQn;
    stcIrqRegiCfg.pfnCallback = &UsartErrIrqCallback;
    stcIrqRegiCfg.enIntSrc = USART_EI_NUM;
    enIrqRegistration(&stcIrqRegiCfg);
    NVIC_SetPriority(stcIrqRegiCfg.enIRQn, DDL_IRQ_PRIORITY_DEFAULT);
    NVIC_ClearPendingIRQ(stcIrqRegiCfg.enIRQn);
    NVIC_EnableIRQ(stcIrqRegiCfg.enIRQn);
    USART_FuncCmd(USART_CH, UsartRx, Enable); // must

    // USART_FuncCmd(USART_CH, UsartRxInt, Enable);	//must
    USART_FuncCmd(USART_CH, UsartTx, Enable); // must
    draw_logo();
    // xTaskCreate((TaskFunction_t )usart_task,(const char*)"usart_task",(uint16_t)1000, (void*)NULL,(UBaseType_t)2,NULL);
}
