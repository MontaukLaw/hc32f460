
#include "allinc.h"
#include "stdint.h"

xQueueHandle adcqueue = NULL;
uint8_t uarttxbuff[1028];
uint32_t srcdata[1028];
uint32_t srcdatalast[1024];
uint16_t adc_value = 0;
uint8_t res[164] = {0};
uint8_t can_ctl_order[13] = {0};

#if 0
/* ADC1 channel definition for this example. */
#define ADC1_SA_NORMAL_CHANNEL (ADC1_CH0 | ADC1_CH10)
#define ADC1_SA_AVG_CHANNEL (ADC1_CH12 | ADC1_CH13)
#define ADC1_SA_CHANNEL (ADC1_SA_NORMAL_CHANNEL | ADC1_SA_AVG_CHANNEL)
#define ADC1_SA_CHANNEL_COUNT (1u)

#define ADC1_AVG_CHANNEL (ADC1_SA_AVG_CHANNEL)
#define ADC1_CHANNEL (ADC1_SA_CHANNEL)
#endif
/* ADC1 channel sampling time.     ADC1_CH0  ADC1_CH10  ADC1_CH12   ADC1_CH13 */
#define ADC1_SA_CHANNEL_SAMPLE_TIME {0x30}

/* ADC resolution definitions. */
#define ADC_RESOLUTION_8BIT (8u)
#define ADC_RESOLUTION_10BIT (10u)
#define ADC_RESOLUTION_12BIT (12u)

#define ADC1_RESOLUTION (ADC_RESOLUTION_12BIT)
#define ADC2_RESOLUTION (ADC_RESOLUTION_10BIT)

/* Scan mode definitions. */
#define ADC1_SCAN_MODE (AdcMode_SAOnce)
#define ADC2_SCAN_MODE (AdcMode_SAContinuous)

/* ADC reference voltage. The voltage of pin VREFH. */
#define ADC_VREF (3.288f)

/* ADC accuracy. */
#define ADC1_ACCURACY (1ul << ADC1_RESOLUTION)

/* ADC2 continuous conversion times. */
#define ADC2_CONTINUOUS_TIMES (3u)

/* Timeout value definitions. */
#define TIMEOUT_MS (10u)

static uint16_t m_au16Adc1Value[ADC1_CH_COUNT];

static void AdcSetChannelPinMode(const M4_ADC_TypeDef *ADCx, uint32_t u32Channel, en_pin_mode_t enMode)
{
	if (M4_ADC1 == ADCx)
		u32Channel &= ADC1_PIN_MASK_ALL;
	// AdcSetPinMode(1,enMode);
	stc_port_init_t stcPortInit;

	MEM_ZERO_STRUCT(stcPortInit);
	stcPortInit.enPinMode = enMode;
	stcPortInit.enPullUp = Disable;

	PORT_Init(PortB, Pin01, &stcPortInit);
}

static void AdcChannelConfig(void)
{
	stc_adc_ch_cfg_t stcChCfg;
	uint8_t au8Adc1SaSampTime[1] = {0x30};

	MEM_ZERO_STRUCT(stcChCfg);

	stcChCfg.u32Channel = ADC1_CH9;
	stcChCfg.u8Sequence = ADC_SEQ_A;
	stcChCfg.pu8SampTime = au8Adc1SaSampTime;
	/* 1. Set the ADC pin to analog mode. */
	AdcSetChannelPinMode(M4_ADC1, ADC1_CH0, Pin_Mode_Ana);
	/* 2. Add ADC channel. */
	ADC_AddAdcChannel(M4_ADC1, &stcChCfg);

#if 0
    /* 3. Configure the average channel if you need. */
    ADC_ConfigAvg(M4_ADC1, AdcAvcnt_32);
    /* 4. Add average channel if you need. */
    ADC_AddAvgChannel(M4_ADC1, ADC1_CH0);
#endif
}

static void AdcInitConfig(void)
{
	stc_adc_init_t stcAdcInit;
	MEM_ZERO_STRUCT(stcAdcInit);
	stcAdcInit.enResolution = AdcResolution_12Bit;
	stcAdcInit.enDataAlign = AdcDataAlign_Right;
	stcAdcInit.enAutoClear = AdcClren_Disable;
	stcAdcInit.enScanMode = ADC1_SCAN_MODE;
	/* 1. Enable ADC1. */
	PWC_Fcg3PeriphClockCmd(PWC_FCG3_PERIPH_ADC1, Enable);
	/* 2. Initialize ADC1. */
	ADC_Init(M4_ADC1, &stcAdcInit);
}

static void AdcClockConfig(void)
{

	stc_clk_xtal_cfg_t stcXtalCfg;
	stc_clk_upll_cfg_t stcUpllCfg;

	MEM_ZERO_STRUCT(stcXtalCfg);
	MEM_ZERO_STRUCT(stcUpllCfg);

	/* Use XTAL as UPLL source. */
	stcXtalCfg.enFastStartup = Enable;
	stcXtalCfg.enMode = ClkXtalModeOsc;
	stcXtalCfg.enDrv = ClkXtalLowDrv;
	CLK_XtalConfig(&stcXtalCfg);
	CLK_XtalCmd(Enable);

	/* Set UPLL out 240MHz. */
	stcUpllCfg.pllmDiv = 2u;
	/* upll = 8M(XTAL) / pllmDiv * plln */
	stcUpllCfg.plln = 60u;
	stcUpllCfg.PllpDiv = 16u;
	stcUpllCfg.PllqDiv = 16u;
	stcUpllCfg.PllrDiv = 16u;
	CLK_SetPllSource(ClkPllSrcXTAL);
	CLK_UpllConfig(&stcUpllCfg);
	CLK_UpllCmd(Enable);
	CLK_SetPeriClkSource(ClkPeriSrcUpllr);
}

static void AdcConfig(void)
{
	AdcClockConfig();
	AdcInitConfig();
	AdcChannelConfig();
}

char *naochuzhongdata(unsigned int *srcdata1, unsigned int *srcdata2, unsigned int *resultdata)
{
	int sum = 0;
#if 0
	for(int m=0;m<32;m++){
		for(int n=0;n<32;n++){
			printf("%04d-", srcdata1[32*m+n]);
		}
		printf("\r\n");
	}
    
	printf("==================================\r\n");
    
	for(int m=0;m<32;m++){
		for(int n=0;n<32;n++){
			printf("%04d-", srcdata2[32*m+n]);
		}
		printf("\n");
	}

	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\r\n");
#endif

	for (int m = 0; m < 32; m++)
	{
		sum = 0;
		for (int n = 0; n < 32; n++)
		{
			int a = srcdata1[32 * n + m] - srcdata2[32 * n + m];
			if (a < 0)
				a = -a;
			sum += a;
			// printf("<%d>-",sum);
		}
		resultdata[m] = sum;
		// printf("<%d>-",sum);
	}
#if 1
	for (int i = 0; i < 32; i++)
	{
		printf("<%04d>-", resultdata[i]);
	}
	printf("\n");
#endif

	// send_data((unsigned char *)resultdata,128);
	// printf("\n");
	return NULL;
}

int getdiff(int sum, int sumlast)
{
	printf("diff==<%d>\n", (sum >= sumlast) ? (sum - sumlast) : (sumlast - sum));
	return 0;
}

void senddata_4sleeprecord(uint32_t *srcdata, uint32_t *srcdatalast)
{

#if 0
    for(int i=0;i<1023;i++){
        srcdata[i] = 37;
        srcdatalast[i] = 30;
    }
    srcdata[1023] = 83;
    srcdatalast[1023] = 0;
#endif

	float sum = 0;

	for (int i = 0; i < 1024; i++)
	{
		if ((srcdata[i] != 0) && (srcdatalast[i] != 0))
		{
			sum += abs(srcdata[i] - srcdatalast[i]);
		}
		else
		{
			sum += abs(srcdata[i] - srcdatalast[i]) * 0.1;
		}
	}
#if 0
     sum = 1.25;
     uint8_t data[4];
     memcpy(data,(uint8_t *)&sum,4);
     for(int i=0;i<4;i++){
        printf("%02x-",data[i]);
     }
#endif
	// sum = 1234.5678;
	send_data((char *)&sum, 4);
}

void get32x32AdcValue()
{

	int adcsum = 0;
	static int adcsumlast = 0;
	memset(uarttxbuff, 0, sizeof(uarttxbuff));
	uint32_t debugdata = 0;
	for (int m = 0; m < 32; m++)
	{
		hc595Send4Byte(1 << m);
		for (int n = 0; n < 32; n++)
		{
			hc4067Chose1Channel32(31 - n);
			Ddl_Delay1us(20);
			ADC_PollingSa(M4_ADC1, m_au16Adc1Value, ADC1_CH_COUNT, TIMEOUT_MS);
			adc_value = m_au16Adc1Value[9];
			adcsum += adc_value;
			if (adc_value < 50)
			{
				debugdata = 1111;
				uarttxbuff[m * 32 + n] = adc_value / 6;
				if (uarttxbuff[m * 32 + n] < 5)
				{
					uarttxbuff[m * 32 + n] = 0;
				}

				adcsum += adc_value;
			}
			else
			{
				debugdata = 0000;
				uarttxbuff[m * 32 + n] = adc_value / 6;
				if (uarttxbuff[m * 32 + n] < 5)
				{
					uarttxbuff[m * 32 + n] = 0;
				}
				adcsum += adc_value;
			}
		}
	}

#if 0
	can_ctl_order[0]=1;

	can_ctl_order[1]=100;
	can_ctl_order[2]=100;
	can_ctl_order[3]=100;
	can_ctl_order[4]=100;
	can_ctl_order[5]=100;
	can_ctl_order[6]=100;
	can_ctl_order[7]=100;
	can_ctl_order[8]=100;
	
	can_ctl_order[9] = 0xAA;
    can_ctl_order[10] = 0x55;
    can_ctl_order[11] = 0x03;
    can_ctl_order[12] = 0x99;
    send_data((char *)can_ctl_order,sizeof(can_ctl_order));

#endif
#if 0 // 直接发160个数
	for (size_t i = 0; i < 160; i++)
	{
		res[i]=i%127;
	}
	
	res[160] = 0xAA;
    res[161] = 0x55;
    res[162] = 0x03;
    res[163] = 0x99;
	send_data((char *)res,sizeof(res));

#endif

#if 0
	int resIndex=0;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 10; j++) {
			res[resIndex]=uarttxbuff[i * 32 + j];
			//sleep_matrix[i+1]=res[i];
			resIndex++;
		}
	}
	for (int i = 8; i <12 ; i++) {
		for (int j = 0; j < 10; j++) {
			int tmp= res[i * 10 + j];
			res[i * 10 + j]=res[(15 - i + 8) * 10 + j];
			res[(15 - i + 8) * 10 + j]= tmp;
		}
	}
	res[160] = 0xAA;
    res[161] = 0x55;
    res[162] = 0x03;
    res[163] = 0x99;
	send_data((char *)res,sizeof(res));

#endif

#if 1 // 直接发送1024个数据
	uarttxbuff[1024] = 0xAA;
	uarttxbuff[1025] = 0x55;
	uarttxbuff[1026] = 0x03;
	uarttxbuff[1027] = 0x99;
	send_data((char *)uarttxbuff, sizeof(uarttxbuff));
#endif
}

int judgecloth(char *matrixdata)
{
	int index = 0;
	char mm[1024] = "";
	char nn[1024] = "";
	int num = 0;
	for (int m = 2; m < 30; m++)
	{
		for (int n = 2; n < 30; n++)
		{
#if 1
			if (matrixdata[28 * m + n] == 0)
			{
				mm[index] = m;
				nn[index] = n;
				index += 1;
			}
#endif
			// printf("<%d>-",uarttxbuff[32*m+n]);
		}
		// printf("\n");
	}
#if 0
	printf("index==<%d>\n",index);
	printf("mmmmmmmmmmmmmmmmmmmmmmmmmmmmm\n");
	for(int i=0;i<index;i++){
		printf("<%d>-",mm[i]);
	}
	printf("mmmmmmmmmmmmmmmmmmmmmmmmmmmmm\n");
#endif
#if 0
	printf("nnnnnnnnnnnnnnnnnnnnnnnnnnnnn\n");
	for(int i=0;i<index;i++){
		printf("<%d>-",nn[i]);
	}
	printf("nnnnnnnnnnnnnnnnnnnnnnnnnnnnn\n");
#endif
#if 1
	for (int i = 2; i < 30; i++)
	{ // m为竖直方向
		for (int m = 0; m < index; m++)
		{
			if (mm[m] == i)
			{
				// printf("find [%d]\r\n",i);
			}
			else
			{
				num++;
			}
		}
		if (num == index)
		{
			// printf("heng not find[%d]!!!!!!\r\n",i);
			return 0;
		}
		num = 0;
	}
#endif
#if 1
	for (int i = 2; i < 30; i++)
	{ // m为竖直方向
		for (int n = 0; n < index; n++)
		{
			if (nn[n] == i)
			{
				// printf("find [%d]\r\n",i);
			}
			else
			{
				num++;
			}
		}
		if (num == index)
		{
			// printf("shu not find[%d]!!!!!!\r\n",i);
			return 0;
		}
		num = 0;
	}
#endif
	return 1;
}

#define HC32TESTLED GpioPin06	  // PB7
#define HC32TESTLEDPORT GpioPortB // PB7

void adc_task(void *pvParameters)
{
	while (1u)
	{
		get32x32AdcValue();

		vTaskDelay(15);
	}
}

void init_adc(void)
{
	AdcConfig();
	xTaskCreate((TaskFunction_t)adc_task,
				(const char *)"adc_task",
				(uint16_t)1500,
				(void *)NULL,
				(UBaseType_t)1,
				NULL);
}
