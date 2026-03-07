#ifndef ALLINC_H
#define ALLINC_H
#include "hc32_ddl.h"
#include "stdint.h"
#include "stdlib.h"
#include "math.h"
#include "FreeRTOS.h"						  
#include "task.h"
#include "queue.h"
#include "math.h"
#include "hc32f46x_gpio.h"
#include "hc32f46x_clk.h"
#include "hc32f46x_efm.h"
#include "hc32f46x_usart.h"
#include "hc32f46x_interrupts.h"
#include "hc32f46x_wdt.h"



#include "app_gpio.h"
#include "app_adc.h"
#include "app_uart.h"
#include "app.h"
#include "app_wdt.h"


#include "uvtest.h"


#define QUENEDATAMAXLEN  30  //uart rx max len

#define FACTORYMODE 999
#define AUTOTESTMODE 998

#define  GETBIT(data, bit)   ((data>>bit)&1)

#define  DEBUGENABLE
#ifdef   DEBUGENABLE
#define  COLOR_NONE                    "\033[0m"
#define  DEBUGCOLOR                    "\033[1m\033[45;33m"
#define  hc32debug(...) do{printf(DEBUGCOLOR"%35s:%04d->>>>",__FILE__,__LINE__);printf(__VA_ARGS__);printf(COLOR_NONE"\r");} while(0)
#else
#define  hc32debug(...)
#endif




enum
{
	QUICKBLINK=0,//on 0.1s off 0.2s
	SLOWBLINK,//on 0.2s off 0.8s
	LEDOFF,
	LEDON,
	LEDBLINK4TIMES,
};



#endif

