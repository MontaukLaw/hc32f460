#include "allinc.h"

#define HC32595DATA Pin00 // PA0
#define HC32595STCP Pin01 // PA1
#define HC32595SHCP Pin13 // PC13
#define HC324067D0 Pin05  // PB5
#define HC324067D1 Pin12  // PA12
#define HC324067D2 Pin10  // PA10
#define HC324067D3 Pin11  // PA11
#define HC324067D4 Pin08  // PA8
#define HC324067D5 Pin15  // PB15
#define HC324067D6 Pin13  // PB13
#define HC324067D7 Pin14  // PB14
#define HC324067EN1 Pin09 // PA09
#define HC324067EN2 Pin12 // PB12

#define HC32595DATAPORT PortA // PA0
#define HC32595STCPPORT PortA // PA1
#define HC32595SHCPPORT PortC // PC13
#define HC324067D0PORT PortB  // PB5
#define HC324067D1PORT PortA  // PA12
#define HC324067D2PORT PortA  // PA10
#define HC324067D3PORT PortA  // PA11
#define HC324067D4PORT PortA  // PA8
#define HC324067D5PORT PortB  // PB15
#define HC324067D6PORT PortB  // PB13
#define HC324067D7PORT PortB  // PB14
#define HC324067EN1PORT PortA // PA09
#define HC324067EN2PORT PortB // PB12

#define HC32TESTLED Pin06	  // PB7
#define HC32TESTLEDPORT PortB // PB7

#define KEYIN_PORT (PortB)
#define KEYIN_PIN (Pin07)

void gpio_set_level(en_port_t port, uint16_t gpio, int num)
{
	if (1 == num)
	{
		PORT_SetBits(port, gpio);
	}
	else if (0 == num)
	{
		PORT_ResetBits(port, gpio);
	}
}

void hc595Send4Byte(uint32_t byte)
{
	// 1 SHCP 移位信号
	// 2 STCP 寄存器锁存信号
	// 3 raise date trans
	gpio_set_level(HC32595STCPPORT, HC32595STCP, 0); // gpio_set_level(ESP32_595STCP,0);
	for (int i = 0; i < 32; i++)
	{
		gpio_set_level(HC32595SHCPPORT, HC32595SHCP, 0); // gpio_set_level(ESP32_595SHCP,0);
		uint32_t data = (byte << i) & 0x80000000;
		if (data)
		{
			gpio_set_level(HC32595DATAPORT, HC32595DATA, 1); // gpio_set_level(ESP32_595DATA,1);
		}
		else
		{
			gpio_set_level(HC32595DATAPORT, HC32595DATA, 0); // gpio_set_level(ESP32_595DATA,0);
		}
		gpio_set_level(HC32595SHCPPORT, HC32595SHCP, 1); // gpio_set_level(ESP32_595SHCP,1);
	}
	gpio_set_level(HC32595STCPPORT, HC32595STCP, 1); // gpio_set_level(ESP32_595STCP,1);
}

void hc4067Chose1Channel32(uint8_t channel) // 21 choose 1  channel(0~31)
{
	// if(channel>31)assert(0);
	if (channel < 16)
	{
		channel = channel;
		gpio_set_level(HC324067EN1PORT, HC324067EN1, 0);				// choose 4067 en  gpio_set_level(ESP32_4067EN1,0);
		gpio_set_level(HC324067EN2PORT, HC324067EN2, 1);				// gpio_set_level(ESP32_4067EN2,1);
		gpio_set_level(HC324067D0PORT, HC324067D0, GETBIT(channel, 0)); // gpio_set_level(ESP32_4067D0,GETBIT(channel, 0));
		gpio_set_level(HC324067D1PORT, HC324067D1, GETBIT(channel, 1)); // gpio_set_level(ESP32_4067D1,GETBIT(channel, 1));
		gpio_set_level(HC324067D2PORT, HC324067D2, GETBIT(channel, 2)); // gpio_set_level(ESP32_4067D2,GETBIT(channel, 2));
		gpio_set_level(HC324067D3PORT, HC324067D3, GETBIT(channel, 3)); // gpio_set_level(ESP32_4067D3,GETBIT(channel, 3));
		// hc32debug("<%d><%d><%d><%d>\n",GETBIT(channel, 0),GETBIT(channel, 1),GETBIT(channel, 2),GETBIT(channel, 3));
	}
	else
	{
		channel = channel - 16;
		gpio_set_level(HC324067EN1PORT, HC324067EN1, 1);				// choose 4067 en  gpio_set_level(ESP32_4067EN1,1);
		gpio_set_level(HC324067EN2PORT, HC324067EN2, 0);				// gpio_set_level(ESP32_4067EN2,0);
		gpio_set_level(HC324067D4PORT, HC324067D4, GETBIT(channel, 0)); // gpio_set_level(ESP32_4067D4,GETBIT(channel, 0));
		gpio_set_level(HC324067D5PORT, HC324067D5, GETBIT(channel, 1)); // gpio_set_level(ESP32_4067D5,GETBIT(channel, 1));
		gpio_set_level(HC324067D6PORT, HC324067D6, GETBIT(channel, 2)); // gpio_set_level(ESP32_4067D6,GETBIT(channel, 2));
		gpio_set_level(HC324067D7PORT, HC324067D7, GETBIT(channel, 3)); // gpio_set_level(ESP32_4067D7,GETBIT(channel, 3));
	}
}

void gpio_task(void *pvParameters)
{
	while (1)
	{
		gpio_set_level(HC32595DATAPORT, HC32595DATA, 1);
		gpio_set_level(HC32595STCPPORT, HC32595STCP, 1);
		gpio_set_level(HC32595SHCPPORT, HC32595SHCP, 1);
		gpio_set_level(HC324067D0PORT, HC324067D0, 1);
		gpio_set_level(HC324067D1PORT, HC324067D1, 1);
		gpio_set_level(HC324067D2PORT, HC324067D2, 1);
		gpio_set_level(HC324067D3PORT, HC324067D3, 1);
		gpio_set_level(HC324067D4PORT, HC324067D4, 1);
		gpio_set_level(HC324067D5PORT, HC324067D5, 1);
		gpio_set_level(HC324067D6PORT, HC324067D6, 1);
		gpio_set_level(HC324067D7PORT, HC324067D7, 1);
		gpio_set_level(HC324067EN1PORT, HC324067EN1, 1);
		gpio_set_level(HC324067EN2PORT, HC324067EN2, 1);
		gpio_set_level(HC32TESTLEDPORT, HC32TESTLED, 1);
		vTaskDelay(1000);
		gpio_set_level(HC32595DATAPORT, HC32595DATA, 0);
		gpio_set_level(HC32595STCPPORT, HC32595STCP, 0);
		gpio_set_level(HC32595SHCPPORT, HC32595SHCP, 0);
		gpio_set_level(HC324067D0PORT, HC324067D0, 0);
		gpio_set_level(HC324067D1PORT, HC324067D1, 0);
		gpio_set_level(HC324067D2PORT, HC324067D2, 0);
		gpio_set_level(HC324067D3PORT, HC324067D3, 0);
		gpio_set_level(HC324067D4PORT, HC324067D4, 0);
		gpio_set_level(HC324067D5PORT, HC324067D5, 0);
		gpio_set_level(HC324067D6PORT, HC324067D6, 0);
		gpio_set_level(HC324067D7PORT, HC324067D7, 0);
		gpio_set_level(HC324067EN1PORT, HC324067EN1, 0);
		gpio_set_level(HC324067EN2PORT, HC324067EN2, 0);
		gpio_set_level(HC32TESTLEDPORT, HC32TESTLED, 0);
		vTaskDelay(1000);
	}
}
#if 0
float test[]  = { 6724.0, 6736.0, 6737.0, 6710.0, 6723.0,
   6715.0, 6733.0, 6732.0, 6727.0, 6735.0, 6750.0, 6751.0, 6728.0, 6734.0,
   6711.0, 6720.0, 6717.0, 6704.0, 6702.0, 6716.0, 6712.0, 6700.0, 6722.0,
   6728.0, 6738.0, 6726.0, 6711.0, 6708.0, 6711.0, 6719.0, 6726.0, 6734.0,
   6728.0, 6735.0, 6721.0, 6737.0, 6732.0, 6735.0, 6719.0, 6731.0, 6725.0,
   6738.0, 6728.0, 6735.0, 6736.0, 6731.0, 6741.0, 6729.0, 6743.0, 6749.0,
   6744.0, 6751.0, 6744.0, 6754.0, 6759.0, 6741.0, 6742.0, 6747.0, 6731.0,
   6744.0, 6759.0, 6752.0, 6757.0, 6753.0, 6748.0, 6759.0, 6754.0, 6754.0,
   6757.0, 6748.0, 6744.0, 6755.0, 6741.0, 6737.0, 6747.0, 6760.0, 6755.0,
   6757.0, 6751.0, 6751.0, 6729.0, 6741.0, 6737.0, 6741.0, 6733.0, 6737.0,
   6732.0, 6732.0, 6750.0, 6737.0, 6743.0, 6747.0, 6740.0, 6736.0, 6720.0,
   6721.0, 6733.0, 6736.0, 6735.0, 6730.0, 6737.0, 6737.0, 6741.0, 6743.0,
   6739.0, 6738.0, 6742.0, 6736.0, 6734.0, 6730.0, 6728.0, 6731.0, 6745.0,
   6739.0, 6741.0, 6736.0, 6744.0, 6729.0, 6741.0, 6741.0, 6763.0, 6751.0,
   6743.0, 6741.0, 6754.0, 6754.0, 6752.0, 6752.0, 6760.0, 6750.0, 6754.0,
   6761.0, 6756.0, 6765.0, 6760.0, 6752.0, 6739.0, 6742.0, 6754.0, 6753.0,
   6758.0, 6762.0, 6747.0, 6759.0, 6758.0, 6756.0, 6768.0, 6757.0, 6744.0,
   6747.0 } ;

static float a,b;
void gpio_task2(void *pvParameters)
{
	while(1){
			breathrate_initialize();
			memcpy(breathrate_U.In1,test,sizeof(breathrate_U.In1));

			breathrate_step();
			a = (float)breathrate_Y.len_data;
			b = (float)breathrate_Y.brate_data;
			breathrate_terminate();
			vTaskDelay(100);
	}
}
#endif

void init_gpio(void)
{

	stc_port_init_t stcPortInit;
	MEM_ZERO_STRUCT(stcPortInit);

	stcPortInit.enPinMode = Pin_Mode_Out;
	stcPortInit.enExInt = Enable;
	stcPortInit.enPullUp = Enable;

	// PORT_Init(PortA, Pin07, &stcPortInit);
	// PORT_Init(PortA, Pin06, &stcPortInit);

	PORT_Init(HC32595DATAPORT, HC32595DATA, &stcPortInit);
	PORT_Init(HC32595SHCPPORT, HC32595SHCP, &stcPortInit);
	PORT_Init(HC32595STCPPORT, HC32595STCP, &stcPortInit);

	PORT_Init(HC324067D0PORT, HC324067D0, &stcPortInit);
	PORT_Init(HC324067D1PORT, HC324067D1, &stcPortInit);
	PORT_Init(HC324067D2PORT, HC324067D2, &stcPortInit);
	PORT_Init(HC324067D3PORT, HC324067D3, &stcPortInit);
	PORT_Init(HC324067D4PORT, HC324067D4, &stcPortInit);
	PORT_Init(HC324067D5PORT, HC324067D5, &stcPortInit);
	PORT_Init(HC324067D6PORT, HC324067D6, &stcPortInit);
	PORT_Init(HC324067D7PORT, HC324067D7, &stcPortInit);
	PORT_Init(HC324067EN1PORT, HC324067EN1, &stcPortInit);
	PORT_Init(HC324067EN2PORT, HC324067EN2, &stcPortInit);

	PORT_Init(HC32TESTLEDPORT, HC32TESTLED, &stcPortInit);

#if 0
	xTaskCreate((TaskFunction_t )gpio_task2,     
                (const char*    )"gpio_task2",   
                (uint16_t       )50, 
                (void*          )NULL,
                (UBaseType_t    )2,
                NULL);
#endif
}
