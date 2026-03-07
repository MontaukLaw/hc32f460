#ifndef APP_H
#define APP_H
extern void app_start(void);
extern xQueueHandle adcqueue;
extern xQueueHandle uartqueue;
extern int systemmode;

extern int leftbedstatusrealtime ;
extern int breathrealtime ;
typedef struct data
{
	struct data *next;
	int nozeronum;
	int sum;
	int num;
}DATA;

extern void app_task(void *pvParameters);
extern void add_data(DATA *data);
extern void linker_inserttail(DATA *data);//add to tail
extern void sendcmd_recvreply(char *cmd,char *reply);



#endif
