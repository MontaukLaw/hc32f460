
#if 0
#include "allinc.h"

#define LINKLENTH    200
#define SAMPLELENGTH 10


#define TIDONG         1024
#define LICHUANG       1025
#define BREATHRATE     1026
#define KEEPSTATUS     1027
#define STARTFLAG      2
#define STOPFLAG       3


#define MCU_VERSION "0017"
#define BLE_CONFIG "ble_config set 4918 "MCU_VERSION"\r\n"
//0004修改灯效
//ble_config set 4918 0005\r\n
int leftbedyuzhi = 0;
int systemmode = 0;

DATA *head;
void linker_inserttail(DATA *data)//add to tail
{
	
	if(head==NULL){
		data->next = NULL;
		head =  data;
	}
	DATA *temp = head;
	while(temp->next!=NULL){
		temp = temp->next;
	}
	temp->next=data;
	data->next=NULL;
	//return head;
}


int disp_link()
{
	if(head==NULL)return 0;
	DATA *temp = head;int num = 0;
	while(temp->next!=NULL){
		num++;
		//hc32debug("%d\n",temp->nozeronum);
		temp = temp->next;		
	}//hc32debug("%d\n",temp->nozeronum);
	return num+1;
}

void get_data(char *nozerodata,float *sum)
{
	if(head==NULL)return ;
	int i = 0;
	DATA *temp = head;
	while(temp->next!=NULL){
		//hc32debug("%d\n",temp->nozeronum);
		sum[i] = (float)temp->sum;
		i++;
		temp = temp->next;				
	}sum[i] = (float)temp->sum;	
}


void get_last10data(char *nozerodata)
{
	DATA *temp = head;
	for(int i=0;i<LINKLENTH-SAMPLELENGTH;i++){
		temp = temp->next;
	}
	for(int i=0;i<SAMPLELENGTH;i++){
		nozerodata[i] = temp->nozeronum;
		temp = temp->next;		
	}
}

void get_last10sum(float *latt10sum)
{
	DATA *temp = head;
	for(int i=0;i<LINKLENTH-SAMPLELENGTH;i++){
		temp = temp->next;
	}
	for(int i=0;i<SAMPLELENGTH;i++){
		latt10sum[i] = temp->sum;
		temp = temp->next;		
	}
}

void get_last50sum(char *latt10sum)
{
	DATA *temp = head;
	for(int i=0;i<LINKLENTH-(SAMPLELENGTH*5);i++){
		temp = temp->next;
	}
	for(int i=0;i<SAMPLELENGTH*5;i++){
		latt10sum[i] = temp->sum;
		temp = temp->next;		
	}
}




int getnum()
{
	if(head==NULL)return 0;
	DATA *temp = head;int num = 0;
	while(temp->next!=NULL){
		num++;
		temp = temp->next;		
	}
	return num+1;
}


void remove_first()
{
	DATA *temp = head;
	if(temp==NULL){
		head =  NULL;
		return;
	}
	if(temp->next == NULL){
		vPortFree(head);
		head =NULL;
		return;
	}
	DATA *newhead = temp->next;
	vPortFree(head);
	head = newhead;
	
}

void add_data(DATA *data)
{	
	if(getnum()==LINKLENTH)
		remove_first();
	linker_inserttail(data);	
}


float fangcha10(float *nozerodata)
{
	double sum = 0;double pow2_sum = 0;
	double __average; //临时保存均方差
	double ans; //均方差
	for(int i=0;i<SAMPLELENGTH;i++){
		sum+=nozerodata[i];
		//hc32debug("f==<%d>\r\n",nozerodata[i]);
	}
	for(int i=0;i<SAMPLELENGTH;i++){
    	pow2_sum += pow(((sum/SAMPLELENGTH)-nozerodata[i]), 2); //math.h平方函数求方差和
	}
	__average = pow2_sum/SAMPLELENGTH;
	ans = sqrt(__average);
	return ans;
}	


float fangcha50(char *nozerodata)
{
	double sum = 0;double pow2_sum = 0;
	double __average; //临时保存均方差
	double ans; //均方差
	for(int i=0;i<SAMPLELENGTH*5;i++){
		sum+=nozerodata[i];
	}
	for(int i=0;i<SAMPLELENGTH*5;i++){
    	pow2_sum += pow(((sum/(SAMPLELENGTH*5))-nozerodata[i]),2); //math.h平方函数求方差和
	}
	__average = pow2_sum/(SAMPLELENGTH*5);
	ans = sqrt(__average);
	return ans;
}	



float fangcha150(float *nozerodata)
{
	double sum = 0;double pow2_sum = 0;
	double __average; //临时保存均方差
	double ans; //均方差
	for(int i=0;i<LINKLENTH;i++){
		sum+=nozerodata[i];
	}
	for(int i=0;i<LINKLENTH;i++){
    	pow2_sum += pow(((sum/LINKLENTH)-nozerodata[i]), 2); //math.h平方函数求方差和
	}
	__average = pow2_sum/LINKLENTH;
	ans = sqrt(__average);
	return ans;
}	





#if 0
  static real32_T pDataValues1[] = { 2448.0, 2472.0, 2468.0, 2457.0, 2470.0,
      2464.0, 2463.0, 2476.0, 2470.0, 2477.0, 2475.0, 2471.0, 2473.0, 2470.0,
      2469.0, 2472.0, 2469.0, 2473.0, 2475.0, 2475.0, 2465.0, 2465.0, 2475.0,
      2466.0, 2440.0, 2441.0, 2433.0, 2428.0, 2433.0, 2419.0, 2414.0, 2418.0,
      2417.0, 2419.0, 2416.0, 2414.0, 2416.0, 2413.0, 2425.0, 2412.0, 2400.0,
      2416.0, 2425.0, 2425.0, 2430.0, 2438.0, 2447.0, 2456.0, 2454.0, 2475.0,
      2486.0, 2491.0, 2479.0, 2487.0, 2469.0, 2458.0, 2462.0, 2445.0, 2452.0,
      2457.0, 2463.0, 2453.0, 2467.0, 2466.0, 2467.0, 2476.0, 2481.0, 2478.0,
      2474.0, 2479.0, 2430.0, 2447.0, 2435.0, 2438.0, 2448.0, 2440.0, 2441.0,
      2432.0, 2441.0, 2432.0, 2422.0, 2424.0, 2435.0, 2422.0, 2411.0, 2423.0,
      2421.0, 2418.0, 2424.0, 2431.0, 2432.0, 2469.0, 2479.0, 2479.0, 2474.0,
      2471.0, 2463.0, 2450.0, 2444.0, 2451.0, 2452.0, 2454.0, 2456.0, 2458.0,
      2458.0, 2450.0, 2456.0, 2454.0, 2453.0, 2454.0, 2462.0, 2413.0, 2414.0,
      2421.0, 2421.0, 2434.0, 2429.0, 2405.0, 2411.0, 2416.0, 2398.0, 2404.0,
      2400.0, 2404.0, 2407.0, 2393.0, 2373.0, 2379.0, 2405.0, 2399.0, 2411.0,
      2405.0, 2423.0, 2433.0, 2427.0, 2418.0, 2428.0, 2437.0, 2433.0, 2461.0,
      2456.0, 2453.0, 2449.0, 2456.0, 2454.0, 2443.0, 2444.0, 2443.0, 2456.0,
      2434.0, 2455.0, 2453.0, 2455.0, 2412.0, 2422.0, 2434.0, 2428.0, 2436.0,
      2426.0, 2423.0, 2421.0, 2419.0, 2423.0, 2408.0, 2414.0, 2407.0, 2418.0,
      2424.0, 2424.0, 2440.0, 2441.0, 2436.0, 2415.0, 2411.0, 2424.0, 2415.0,
      2432.0, 2439.0, 2444.0, 2442.0, 2456.0, 2444.0, 2439.0, 2439.0, 2451.0,
      2472.0, 2443.0, 2442.0, 2438.0, 2441.0, 2443.0, 2459.0, 2468.0, 2451.0,
      2459.0, 2450.0, 2448.0, 2460.0, 2459.0 } ;    


static real32_T pDataValues0[] = { 2439.0, 2429.0, 2424.0, 2416.0, 2449.0,
      2451.0, 2442.0, 2412.0, 2397.0, 2407.0, 2390.0, 2369.0, 2375.0, 2360.0,
      2362.0, 2386.0, 2395.0, 2388.0, 2373.0, 2388.0, 2400.0, 2419.0, 2405.0,
      2413.0, 2412.0, 2397.0, 2421.0, 2421.0, 2412.0, 2412.0, 2408.0, 2410.0,
      2400.0, 2387.0, 2382.0, 2386.0, 2382.0, 2402.0, 2395.0, 2408.0, 2420.0,
      2403.0, 2397.0, 2424.0, 2440.0, 2426.0, 2421.0, 2433.0, 2428.0, 2425.0,
      2434.0, 2447.0, 2429.0, 2453.0, 2435.0, 2456.0, 2437.0, 2428.0, 2445.0,
      2462.0, 2460.0, 2444.0, 2430.0, 2437.0, 2440.0, 2430.0, 2438.0, 2431.0,
      2428.0, 2431.0, 2442.0, 2434.0, 2444.0, 2468.0, 2469.0, 2456.0, 2465.0,
      2477.0, 2467.0, 2435.0, 2431.0, 2416.0, 2409.0, 2392.0, 2382.0, 2397.0,
      2381.0, 2402.0, 2399.0, 2397.0, 2388.0, 2392.0, 2406.0, 2392.0, 2398.0,
      2390.0, 2407.0, 2453.0, 2446.0, 2448.0, 2472.0, 2468.0, 2457.0, 2470.0,
      2464.0, 2463.0, 2476.0, 2470.0, 2477.0, 2475.0, 2471.0, 2473.0, 2470.0,
      2469.0, 2472.0, 2469.0, 2473.0, 2475.0, 2475.0, 2465.0, 2465.0, 2475.0,
      2466.0, 2440.0, 2441.0, 2433.0, 2428.0, 2433.0, 2419.0, 2414.0, 2418.0,
      2417.0, 2419.0, 2416.0, 2414.0, 2416.0, 2413.0, 2425.0, 2412.0, 2400.0,
      2416.0, 2425.0, 2425.0, 2430.0, 2438.0, 2447.0, 2456.0, 2454.0, 2475.0,
      2486.0, 2491.0, 2479.0, 2487.0, 2469.0, 2458.0, 2462.0, 2445.0, 2452.0,
      2457.0, 2463.0, 2453.0, 2467.0, 2466.0, 2467.0, 2476.0, 2481.0, 2478.0,
      2474.0, 2479.0, 2430.0, 2447.0, 2435.0, 2438.0, 2448.0, 2440.0, 2441.0,
      2432.0, 2441.0, 2432.0, 2422.0, 2424.0, 2435.0, 2422.0, 2411.0, 2423.0,
      2421.0, 2418.0, 2424.0, 2431.0, 2432.0, 2469.0, 2479.0, 2479.0, 2474.0,
      2471.0, 2463.0, 2450.0, 2444.0, 2451.0 
} ;
#endif


int data_process(char *bodymove,char *leavebedflag,char *breathpermin)
{
	
	char nozerodata[SAMPLELENGTH]="";float sum[LINKLENTH];memset(sum,0,sizeof(sum));
	float last10sum[SAMPLELENGTH];
	int outflag = 1;
	if(getnum()==LINKLENTH){
		get_last10data(nozerodata);
		get_last10sum(last10sum);
		get_data(NULL,sum);
		for(int i=0;i<SAMPLELENGTH;i++){
			//printf("sum==<%f>\n",last10sum[i]);
			if(last10sum[i]>9000)outflag = 0;
		}
		*leavebedflag = (char)outflag;
		//printf("<%d>\n",*leavebedflag);
		//bodymovemont = fangcha10(nozerodata);
		
		
		uvtest_initialize();
		memcpy(uvtest_U.input_data,sum,sizeof(sum));
		uvtest_step();
		*breathpermin = (char) (uvtest_Y.brate_data);
		*bodymove = (char)(fangcha10(last10sum)/10);

		#if 0
		hc32debug("==================\n");
		for(int i=0;i<200;i++){
			hc32debug("%f\r\n",sum[i]);
		}
		hc32debug("breath==<%d>bodymove==<%d>\n",*breathpermin,*bodymove);
		#endif
		//if((double)(fangcha10(last10sum)/10)<0.5)uvtest_Y.brate_data = 0;
		
		//hc32debug("<%f> fangcha==<%f>\n",uvtest_Y.brate_data,fangcha10(last10sum)/10);
		
		//hc32debug("bodymovement==%f,outflag==%d   breathrate==%d\n",bodymovemont,outflag,*breathpermin);			
	}else{
		//hc32debug("not enough data!!!\n");
		return 0;
	}	
	return 1;
}


#define SIID                     3

#define MATRIXONEID              1
#define MATRIXTWOID              2
#define MATRIXTHREEID            3
#define MATRIXFOURID             4
#define COMBINEID                5
#define COMBINETONGJIID          6
#define SENDDATACMD              "properties_changed"
#define LICHUANGWARNING          7
#define LICHUANGYUZHI            8
#define CLEANHISTORY             9
#define ZHINENGCHANGJINGID       10
#define ZHINENGCHANGFALLASLEEP   12

int tongjikeepinbedflag = 0;



void sendzhinengchangjing(int inbedstate)
{
	vTaskDelay(2000);
	char combinedata[100] = "";
	snprintf(combinedata,sizeof(combinedata),"%s %d %d %d\r\n",SENDDATACMD,SIID,ZHINENGCHANGJINGID,inbedstate);
	send_data(combinedata,strlen(combinedata));
	vTaskDelay(2000);
	
}


void sendzhinengchangjingfallasleep(int inbedstate)
{
	vTaskDelay(2000);
	char combinedata[100] = "";
	snprintf(combinedata,sizeof(combinedata),"%s %d %d %d\r\n",SENDDATACMD,SIID,ZHINENGCHANGFALLASLEEP,inbedstate);
	send_data(combinedata,strlen(combinedata));
	vTaskDelay(2000);
	
}



void sendcmd_recvreply(char *cmd,char *reply)
{
	xQueueReset(uartqueue);
	send_data(cmd,strlen(cmd));
	
	char uartrecv[100]="";
	if(xQueueReceive(uartqueue, uartrecv, 1000)) {
		//send_data(test,1024);
		memcpy(reply,uartrecv,strlen(uartrecv));
		//printf("[%s]\r\n",uartrecv);
    }else{
		
	}
}

int uapflag = 0;
void send_cleanhistory()
{
	char combinedata[100] = "";
	snprintf(combinedata,sizeof(combinedata),"%s %d %d \"cleanhistory\"\r\n",SENDDATACMD,SIID,CLEANHISTORY);
	send_data(combinedata,strlen(combinedata));
	vTaskDelay(1000);
	
}

int check_net()
{
	char reply[20]="";
	sendcmd_recvreply("net\r\n",reply);
	if(strcmp(reply,"cloud\r")==0){
		led_status = LEDON;
		if(uapflag==1){
			uapflag = 0;
			send_cleanhistory();
		}
		return 1;
	}else if((strcmp(reply,"uap\r")==0)){
		led_status = QUICKBLINK;
		uapflag = 1;
		return 3;
	}else if(strcmp(reply,"unprov\r")==0){
		led_status = QUICKBLINK;
		return 2;
	}
	else if(strcmp(reply,"updating\r")==0){
		led_status = SLOWBLINK;
		return 4;
	}else if((strcmp(reply,"local\r")==0)&&(systemmode==FACTORYMODE)){
		//printf("wifi test ok!!!!\n");
		led_status = LEDON;
		return 5;
	}else{
		led_status = LEDOFF;
		return 6;
	}

}


static int  process_data(char *test)
{

	//这四个数据按照顺序是  体动  离床标志  呼吸 和持续在床离床标志
	
	if(data_process(test+1024,test+1025,test+1026)==0){
		return 0;
	}//
	else 
		return 1;	
}



static void realtimesenddata_2_miot(char *test)
{
	////////////send xx-xx-xx-xx 体动 离床  呼吸
	char combinedata[100] = "";
	snprintf(combinedata,sizeof(combinedata),"%s %d %d \"%02x-%02x-%02x-%02x\"\r\n",SENDDATACMD,SIID,COMBINEID,test[TIDONG],test[LICHUANG],test[BREATHRATE],test[KEEPSTATUS]);
	send_data(combinedata,strlen(combinedata));
	
}


static void send_leftbed_warn(int type)
{
	char combinedata[100] = "";
	snprintf(combinedata,sizeof(combinedata),"%s %d %d %d\r\n",SENDDATACMD,SIID,LICHUANGWARNING,type);
	send_data(combinedata,strlen(combinedata));
	vTaskDelay(1000);
}
uint8_t yuzhi = 0;
char zhinenginbed = 0;


int leftbedstatusrealtime = 0;
int breathrealtime = 16;
static void get_down()
{
	char reply[100]="";
	sendcmd_recvreply("get_down\r",reply);
	#if 1
	if(strcmp(reply,"down update_fw\r")==0){
		memset(reply,0,100);
		sendcmd_recvreply("result \"ready\"\r",reply);
		if(strcmp(reply,"ok\r")==0)
			NVIC_SystemReset();
	}else if(strcmp(reply,"down get_properties 3 8\r")==0){
		//printf("getdown==<%s>\n",reply);
		char combinedata[100] = "";
		snprintf(combinedata,sizeof(combinedata),"result 3 8 0 %d\r",yuzhi);

		send_data(combinedata,strlen(combinedata));	
		vTaskDelay(100);
	}else if(strcmp(reply,"down get_properties 2 1\r")==0){
		char combinedata[100] = "";
		snprintf(combinedata,sizeof(combinedata),"result 2 1 0 %d\r",0);
		send_data(combinedata,strlen(combinedata));	
		vTaskDelay(100);
	}else if(strcmp(reply,"down get_properties 3 11\r")==0){
		char combinedata[100] = "";
		if(leftbedstatusrealtime==1)breathrealtime = 0;
		snprintf(combinedata,sizeof(combinedata),"result 3 11 0 \"%02x-%02x\"\r",leftbedstatusrealtime,breathrealtime);
		send_data(combinedata,strlen(combinedata));	
		vTaskDelay(100);
	}else if(strcmp(reply,"down none\r")==0){
		//printf("getdown!!!!!==<%s>\n",reply);
	}else if(strstr(reply,"down set_properties 3 8")!=0){
		#if 1
		sscanf(reply,"down set_properties 3 8 %hhd",&yuzhi);
		leftbedyuzhi = yuzhi*10;
		char combinedata[100] = "";
		snprintf(combinedata,sizeof(combinedata),"result 3 8 0 \r",yuzhi);
		send_data(combinedata,strlen(combinedata));	
		vTaskDelay(100);
		memset(combinedata,0,sizeof(combinedata));
		snprintf(combinedata,sizeof(combinedata),"properties_changed 3 8 %d\r",yuzhi);
		send_data(combinedata,strlen(combinedata));	

		#endif
		//printf("yes!!!!!\n");
	}else if(strcmp(reply,"down get_properties 3 10\r")==0){
		//printf("getdown==<%s>\n",reply);
		char combinedata[100] = "";
		snprintf(combinedata,sizeof(combinedata),"result 3 10 0 %d\r",zhinenginbed);
		send_data(combinedata,strlen(combinedata)); 
		vTaskDelay(100);
	}else{
		memset(reply,0,100);
		sendcmd_recvreply("result \"none\"\r",reply);
	}
	#endif
}


static void tongjisenddata_2_miot(char *test,int averbreath,int tidongcishu,int id)
{

	char combinedata[100] = "";char data[3] = "";
	sprintf(data,"%02x",id);
	if(test[LICHUANG]==1)averbreath = 0;
	if(tongjikeepinbedflag>1){
		//hc32debug("<%d><%d><%d><%d>\n",averbreath,tidongcishu,test[LICHUANG],tongjikeepinbedflag);
		snprintf(combinedata,sizeof(combinedata),"%s %d %d \"%02x-%02x-%02x-%02x\"\r\n",SENDDATACMD,SIID,COMBINETONGJIID,tidongcishu*15,test[LICHUANG],averbreath,tongjikeepinbedflag);
		send_data(combinedata,strlen(combinedata));
		tongjikeepinbedflag = test[LICHUANG];
	}		
	else{
		snprintf(combinedata,sizeof(combinedata),"%s %d %d \"%02x-%02x-%02x-%02x\"\r\n",SENDDATACMD,SIID,COMBINETONGJIID,tidongcishu*15,test[LICHUANG],averbreath,test[LICHUANG]);
		send_data(combinedata,strlen(combinedata));
		//hc32debug("<%d><%d><%d><%d>\n",averbreath,tidongcishu,test[LICHUANG],test[LICHUANG]);
	}	
	vTaskDelay(100);
	get_down();
	vTaskDelay(100);


	#if 1
	/////send maritx
	char cmdmatrix[550]="";
	char cmd[515]="";
	for(int i=0;i<256;i++){
	     char temp[3] = "";
		 sprintf(temp,"%02x",test[i]);
		 strncat(cmd,temp,100);
	}
	//strncat(cmd,data,100);
	
	//hc32debug("cmd==<%s>\r\n",cmd);
	sprintf(cmdmatrix,"%s %d %d \"%s\"\r\n",SENDDATACMD,SIID,MATRIXONEID,cmd);
	send_data(cmdmatrix,strlen(cmdmatrix));	
	vTaskDelay(100);
	get_down();
	vTaskDelay(100);


	//send matrix data two
	memset(cmdmatrix,0,sizeof(cmdmatrix));
	memset(cmd,0,sizeof(cmd));
	for(int i=256;i<512;i++){
		 char temp[3] = "";
		 sprintf(temp,"%02x",test[i]);
		 strncat(cmd,temp,100);
	}
	//strncat(cmd,data,100);
	sprintf(cmdmatrix,"%s %d %d \"%s\"\r\n",SENDDATACMD,SIID,MATRIXTWOID,cmd);
	send_data(cmdmatrix,strlen(cmdmatrix));
	vTaskDelay(100);
	get_down();
	vTaskDelay(100);
	//send matrix data three
	memset(cmdmatrix,0,sizeof(cmdmatrix));
	memset(cmd,0,sizeof(cmd));
	for(int i=512;i<768;i++){
		 char temp[3] = "";
		 sprintf(temp,"%02x",test[i]);
		 strncat(cmd,temp,100);
	}
	//strncat(cmd,data,100);
	sprintf(cmdmatrix,"%s %d %d \"%s\"\r\n",SENDDATACMD,SIID,MATRIXTHREEID,cmd);
	send_data(cmdmatrix,strlen(cmdmatrix));
	vTaskDelay(100);
	get_down();
	vTaskDelay(100);
	//send matrix data four
	memset(cmdmatrix,0,sizeof(cmdmatrix));
	memset(cmd,0,sizeof(cmd));
	for(int i=768;i<1024;i++){
		 char temp[3] = "";
		 sprintf(temp,"%02x",test[i]);
		 strncat(cmd,temp,100);
	}
	//strncat(cmd,data,100);
	sprintf(cmdmatrix,"%s %d %d \"%s\"\r\n",SENDDATACMD,SIID,MATRIXFOURID,cmd);
	send_data(cmdmatrix,strlen(cmdmatrix));
	vTaskDelay(100);
	get_down();
	vTaskDelay(100);
	/////send maritx
	#endif
		
}






static int setmodel()
{
	char reply[20]="";
	sendcmd_recvreply("model qushui.bed.8hai1\r\n",reply);
	if(strcmp(reply,"ok\r")==0){
		//led_status = LEDON;
		return 1;
	}else if(strcmp(reply,"unprov\r")==0){
		//led_status = LEDOFF;
		return 2;
	}else{
		//led_status = LEDOFF;
		return 3;
	}
}


static int setmcuversion()
{
	char reply[20]="";
	sendcmd_recvreply("mcu_version "MCU_VERSION"\r",reply);
	if(strcmp(reply,"ok\r")==0){
		//led_status = LEDON;
		return 1;
	}else if(strcmp(reply,"unprov\r")==0){
		//led_status = LEDOFF;
		return 2;
	}else{
		//led_status = LEDOFF;
		return 3;
	}
}


static int disablehco()
{
	char reply[20]="";
	sendcmd_recvreply("echo off\r\n",reply);
	if(strcmp(reply,"ok\r")==0){
		//led_status = LEDON;
		return 1;
	}else if(strcmp(reply,"error\r")==0){
		//led_status = LEDOFF;
		return 2;
	}else{
		//led_status = LEDOFF;
		return 3;
	}
}

static int setbleconfig()
{
	char reply[20]="";
	sendcmd_recvreply(BLE_CONFIG,reply);
	if(strcmp(reply,"ok\r")==0){
		//led_status = LEDON;
		return 1;
	}else if(strcmp(reply,"unprov\r")==0){
		//led_status = LEDOFF;
		return 2;
	}else{
		//led_status = LEDOFF;
		return 3;
	}
}

static void init_miotesp32()
{
	vTaskDelay(1000);
	setmcuversion();
	vTaskDelay(1000);
	setmodel();
	vTaskDelay(1000);
	setbleconfig();
	vTaskDelay(1000);
	disablehco();
	vTaskDelay(1000);	
	return;
}


#if 0
int main()
{
	char recvdata[] = "down set_properties 3 11 33";//yuzhi
	char yuzhi = 0;
	if(strstr(recvdata,"down set_properties 3 11")!=NULL){
		sscanf(recvdata,"down set_properties 3 11 %hhd",&yuzhi);
		printf("yuzhi===%d\n",yuzhi);
		char combinedata[100] = "";
		snprintf(combinedata,sizeof(combinedata),"%s %d %d %hhd\r\n",SENDDATACMD,SIID,LEFTBEDYUZHI,yuzhi);
		printf("<%s>\n",combinedata);		
	}else{
		printf("not correct cmd!!!!\n");
	}
	return 0;
}
#endif


#define  HC32TESTLED     Pin06 //PB7
#define  HC32TESTLEDPORT PortB //PB7
#define  KEYIN_PORT      (PortB)
#define  KEYIN_PIN       (Pin07)


int getmax(char *data,int len)
{
	int max = 0;
	for(int i=0;i<len;i++){
		if(data[i]>max)
			max = data[i];
	}
	return max;
}





void autotestmode()
{
	char uartrecv[100]="";
	if(xQueueReceive(uartqueue, uartrecv, 1000)) {
		if(strcmp(uartrecv,"ledon")==0){//led on
			gpio_set_level(HC32TESTLEDPORT,HC32TESTLED,1);
			printf("led on\n");
		}else if(strcmp(uartrecv,"ledoff")==0){//led off
			gpio_set_level(HC32TESTLEDPORT,HC32TESTLED,0);
			printf("led off\n");
		}else if(strcmp(uartrecv,"getkey")==0){
			if(PORT_GetBit(KEYIN_PORT,KEYIN_PIN))//0 press
				printf("key up\n");
			else
				printf("key down\n");
		}else if(strcmp(uartrecv,"getfwversion")==0){//ok
			printf(MCU_VERSION"\n");
		}else if(strcmp(uartrecv,"getuuid")==0){//ok
			stc_efm_unique_id_t id = EFM_ReadUID();
			printf("%04X-%04X-%04X\n",id.uniqueID1,id.uniqueID2,id.uniqueID3);
		}else if(strcmp(uartrecv,"getclothempty")==0){
			get30x30AdcValueempty();
			int max = getmax(uarttxbuff,sizeof(uarttxbuff));
			if(max>5)
				printf("fail!\n");
			else
				printf("success!\n");
			
		}else if(strcmp(uartrecv,"getclothpressed")==0){
			get30x30AdcValue();
			int sum = 0;
			for(int i=0;i<1500;i++){
				sum+=uarttxbuff[i];			
			}
			printf("sum==%d\n",sum);
			//int ret = judgecloth(uarttxbuff);
			//if(sum<1000)
				//printf("fail!\n");
			//else
				//printf("success!\n");
			
		}else if(strcmp(uartrecv,"gettestversion")==0){//0001
			//printf("0001\n");
		}
	}
	
}


void factorytest()
{
	led_status = LEDBLINK4TIMES;	int ret = 0;
	char reply[100]="";
	sendcmd_recvreply("factory\r",reply);
	//test cloth
	for(int i=0;i<2000;i++){
		get30x30AdcValue();
		ret = judgecloth(uarttxbuff);
		if(ret==1){
			printf("test cloth ok!\n");
			break;
		}
	}
	if(ret==1){
		led_status = SLOWBLINK;
		while(1){
			char reply[20]="";
			sendcmd_recvreply("net\r\n",reply);
			if(strcmp(reply,"local\r")==0)
				led_status = LEDON;
			vTaskDelay(1000);
		}		
	}else{
		while(1);
	}	
	//vTaskDelay(1000);
}

void app_task(void *pvParameters)
{
	char test[1028]="";int timeflag = 0;static int lichuangtemp = 0;static int bianhuaflag = 0;static int chixucount = 0;
	static int breathratetotal = 0;static int tidongcishu = 0;static int id = 0;static int breathratenotzerotime = 0;
	int warn_leftflag = 0;int warn_leftflagtemp = 0;int warnbianhuaflag = 0;static int  warnchixucount = 0;static int inbedchixutime = 0;
	int tidongtemp = 0;static int jingzhichixutime = 0;static int fallinsleepflag = 0;
	init_miotesp32();
	static int sendmodelflag = 0;static int inbedoneminflag = 0;
	while(1){
		if(systemmode==FACTORYMODE){
			factorytest();
			continue;
		}
		if(systemmode==AUTOTESTMODE){
			//printf("auto test mode\n");
			autotestmode();
			continue;
		}
		get_down();
		vTaskDelay(200);	
		#if 1//net
		if(check_net()!=1){
			vTaskDelay(500);//
			continue;			
		}
		#endif
		#if 1
		if(sendmodelflag==0){
			init_miotesp32();
			sendmodelflag = 1;
		}
		#endif

		#if 1//send miotdata
		memset(test,0,1028);
		if(process_data(test)==0){
			hc32debug("not enough data!!!!\n");
			vTaskDelay(1000);//
			continue;
		}else{			
			if(xQueueReceive(adcqueue, test, portMAX_DELAY)) {
				#if 0  //debug adc data
				for(int i=0;i<32;i++){
					for(int m=0;m<32;m++){
						#if 0
						if(test[32*i+m]>5)
							printf("0000-");
						else
							printf("1111-");
						#endif
						printf("%04d-",test[32*i+m]);
					}
					printf("\r\n");
				}
				#endif					
			}
			timeflag+=1;
			if(test[BREATHRATE]<13)test[BREATHRATE]=16;//暂时狗一手
			if(test[BREATHRATE]>21)test[BREATHRATE]=17;//暂时狗一手
			//hc32debug("<%d><%d><%d>\n",test[1024],test[1025],test[1026]);//tidong  lichuang huxi
			if(test[LICHUANG]==1){//离床时呼吸体动归零
				test[BREATHRATE] = 0;
				test[TIDONG] = 0;
				fallinsleepflag = 0;
			}
			////////////////////////////////			
			warn_leftflag = test[LICHUANG];
			if(inbedoneminflag==0){
				if(warn_leftflag==0)
					inbedchixutime += 1;
				else if((warn_leftflag==1)&&(inbedoneminflag==0))
					inbedchixutime = 0;
				if(inbedchixutime>220)
					inbedoneminflag = 1;
			}
			
			
			
			if(inbedoneminflag==1){
				if(warn_leftflag==1)
					warnchixucount++;
				else{
					warnchixucount = 0;
				}
					
				if(warnchixucount>yuzhi*220*10){//lichuang baojing
					if(yuzhi!=0)
						send_leftbed_warn(yuzhi);
					inbedchixutime = 0;
					inbedoneminflag = 0;
					warnchixucount=0;
				}
					
			}	//离床警报定义 在床30s后  离床持续10分钟 20分钟 30分钟发送警报
			////////////////////////////////xiaomianxuyaode 报告
			////////////////////////////////
			if(lichuangtemp!=test[LICHUANG]){
				lichuangtemp = test[LICHUANG];
				bianhuaflag = 1;
				chixucount = 0;
			}else{
				chixucount++;
				if(chixucount>70){//chixu 40s
					if(bianhuaflag==1){
						bianhuaflag = 0;
						if(lichuangtemp==1){
							test[KEEPSTATUS] = STOPFLAG;
							tongjikeepinbedflag = STOPFLAG;
							zhinenginbed = 1;
							sendzhinengchangjing(zhinenginbed);
						}							
						else{
							test[KEEPSTATUS] = STARTFLAG;
							tongjikeepinbedflag = STARTFLAG;
							zhinenginbed = 0;
							sendzhinengchangjing(zhinenginbed);
						}								
					}					
				}
			}//智能场景定义   离床在床状态改变持续210s后发送智能场景0 1状态
			/////////////////////////////////
			breathratetotal+=test[BREATHRATE];
			if(test[BREATHRATE]>0)breathratenotzerotime+=1;
			//if(test[TIDONG]>=20)tidongcishu+=1;
			if((test[TIDONG]>20)&&(tidongtemp<20)){
				tidongcishu+=1;
			}
			tidongtemp = test[TIDONG];
			//realtimesenddata_2_miot(test);
			//printf("<%d>\n",tidongcishu);
			leftbedstatusrealtime = test[LICHUANG];
			breathrealtime = test[BREATHRATE];
			//printf("timeflag==<%d>\n",timeflag);
			///////////////////////////////send tongji	
			#if 1 //send to xiaomian			
			if(timeflag>=238){//tongjitime1
				//check_net();
				if(test[LICHUANG]==1)
					jingzhichixutime=0;
				if(tidongcishu==0){
					jingzhichixutime+=1;
					if((jingzhichixutime>10)&&(fallinsleepflag==0)){//15min static send fallaslepp
						//sendtoxiaomi
						fallinsleepflag = 1;
						jingzhichixutime = 0;
						sendzhinengchangjingfallasleep(1);
					}
				}else
					jingzhichixutime = 0;
				if(id<=254)id+=1;
				else id = 0;
				timeflag=0;
				tongjisenddata_2_miot(test,breathratetotal/breathratenotzerotime,tidongcishu,id);
 				breathratenotzerotime = 0;
				breathratetotal = 0;tidongcishu = 0;			
			}
			#endif
		}	
		#endif
	}
	
}





void app_start(void)
{	

	hc32debug("config net\n");
	Ddl_Delay1us(200000);
	xTaskCreate((TaskFunction_t )app_task,(const char*)"app_task",(uint16_t)10000, (void*)NULL,(UBaseType_t)1,NULL); 	
}

#endif







