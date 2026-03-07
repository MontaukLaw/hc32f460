#include "allinc.h"


void wdt_task(void *pvParameters)
{
	while (1u)
	{
		WDT_RefreshCounter();
		//vTaskDelay(500);
		//Ddl_Delay1ms(100u);
	}

}



static void Wdt_Config(void)
{
    stc_wdt_init_t stcWdtInit;
    MEM_ZERO_STRUCT(stcWdtInit);
    stcWdtInit.enClkDiv = WdtPclk3Div512;
    stcWdtInit.enCountCycle = WdtCountCycle16384;
    stcWdtInit.enRefreshRange = WdtRefresh0To100Pct;
    stcWdtInit.enSleepModeCountEn = Disable;
    stcWdtInit.enRequestType = WdtTriggerResetRequest;
    WDT_Init(&stcWdtInit);
}



void init_wdt()
{

	Wdt_Config();
	xTaskCreate((TaskFunction_t )wdt_task,     
	(const char*    )"wdt_task",   
	(uint16_t       )1500, 
	(void*          )NULL,
	(UBaseType_t    )1,
	NULL); 		
	
}




