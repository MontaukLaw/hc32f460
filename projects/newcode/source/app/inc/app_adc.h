#ifndef APP_ADC_H
#define APP_ADC_H
extern void init_adc(void);
extern uint8_t uarttxbuff[1028];
extern int judgecloth(char *matrixdata);
extern void get30x30AdcValue(void);
extern void get30x30AdcValueempty(void);
#endif

