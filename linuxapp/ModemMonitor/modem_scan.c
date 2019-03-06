/*************************************************************************
	> File Name: modem_scan.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Mon 05 Nov 2018 03:06:53 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>

#include "modem_scan.h"
#include "led.h"
#include "Log.h"

static char serialName[256];

#define QLTY_DATA_ERROR -1
#define QLTY_DEVICE_ERROR -1

void serial_init(int fd)
{
	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag |= ( CLOCAL | CREAD);
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~PARENB;
	options.c_iflag &= ~INPCK;
	options.c_cflag &= ~CSTOPB;
	options.c_oflag = 0;
	options.c_lflag = 0;

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	tcsetattr(fd, TCSANOW, &options);
}

static int readMobile(char *cmd,int cmdLen,char *res,int resLen)
{
	if(cmd == NULL || res == NULL )
		return -1;
	char buf[128];
	if(cmdLen > 128)
		return -1;
	memcpy(buf,cmd,cmdLen);
	if(buf[cmdLen-1] != '\r')
	{
		buf[cmdLen] = '\r';
		cmdLen ++;
	}
	memset(res, 0, resLen);
	int fd;
	int nwrite,nread;

	if(strlen(serialName) < 5 || access(serialName,F_OK))
		return -1;

	fd = open(serialName, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd)
	{
		perror(serialName);
		memset(serialName,0,sizeof(serialName));
		return -2;
	}
	serial_init(fd);

	nwrite = write(fd, buf, cmdLen);
	if(cmdLen != nwrite )
	{
		close(fd);
		LOGEX("close %s when write %d/%d",serialName,nwrite,cmdLen);
		perror("serial write");
		return 0;
	}

	nread = 0;
	int doNum = 10;
	int offset=0;
	do{
		usleep(200*1000);
		nread = read(fd, res+offset, resLen-offset);
		if(nread != -1)
			offset += nread;
		doNum --;
	}
	while(doNum > 0 && NULL ==strstr(res,"ERROR") && NULL == strstr(res,"OK"));

	close(fd);
	if(strcmp(res,cmd) == 0)
		memmove(res,res+strlen(cmd),offset - strlen(cmd)+1);
//	printf("%s:return,%d\n",__func__,offset);
	return offset;
}


int cmee_enable()
{
	int nread;
	char reply[128];
	char cmd[] = "AT+CMEE=2\r";

	if(access(serialName,F_OK))
	{
		return 0;
	}

	nread = readMobile(cmd,strlen(cmd),reply,sizeof(reply));
	if(nread >= 0)
	{
		printf("%s\n",reply);
	}

	return 0;
}


//Mobile network type
int get_system_mobile_mnt(double *count)
{
	int nread;
	char reply[128];
	int bufLen = sizeof(reply);
	char cmd[] = "AT+COPS?";

	if (count == NULL)
		return QLTY_DATA_ERROR;
	*count = -1;

	nread = readMobile(cmd,strlen(cmd),reply,sizeof(reply));
	if(  nread == -2){
		return QLTY_DEVICE_ERROR;
	}else if(nread < 1){
		return QLTY_DATA_ERROR;
	}
	if(NULL == strstr(reply,"OK"))
	{
		return QLTY_DATA_ERROR;
	}
	char *p = reply;
//	printf("%s\n",p);
	while( p - reply < bufLen && (*p++)!='"');
	while(p - reply < bufLen &&(*p++)!='"');

	p++;
	char *pnull = p+1;
	while(pnull - reply < bufLen &&(*++pnull)!='\n');
	*pnull = 0;
	int type = strtoul(p,NULL,0);
	switch(type)
	{
	case 0: //GSM
	case 1: //Enhanced GSM
	case 3: //GSM w/EGPRS
		*count = 2;
		break;
	case 2: //UTRAN
	case 4: //UTRAN w/HSDPA
	case 5: //UTRAN w/HSUPA
	case 6: //UTRAN w/HSDPA and HSUPA
		*count = 3;
		break;
	case 7: //E-UTRAN
		*count = 4;
		break;
	case 100: //CDMA for EC20
		*count = 3;
		break;
	default:
		*count = -type;
	}

//	printf("%s:%lf\n",__func__,*count);
	return 0;

}


int get_system_mobile_mpn(double *count)
{
	int nread;
	char cmd[] = "AT+CNUM";
	char reply[128];
	int bufLen = sizeof(reply);

	if (count == NULL)
	{
		return QLTY_DATA_ERROR;
	}
	*count = 0;

	nread = readMobile(cmd,strlen(cmd),reply,sizeof(reply));
	if(  nread == -2){
		return QLTY_DEVICE_ERROR;
	}else if(nread < 1){
		return QLTY_DATA_ERROR;
	}
	if(NULL == strstr(reply,"OK"))
	{
		return QLTY_DATA_ERROR;
	}
	char *p = strstr(reply,"+CNUM:");
	if(NULL == p)
	{
		return QLTY_DATA_ERROR;
	}

	while(p - reply < bufLen && (*++p)!=',');
	if(p+2 > reply )
		p += 2;
	char *pnull = p;
	while((pnull - reply < bufLen)  && (*++pnull)!='"');
	*pnull = 0;
	*count=atof(p);;
//	printf("%s:%lf\n",__func__,*count);
	return 0;
}


//Mobile sim card status
int get_gprs_sim_state(char * ttyName)
{
	int nread;
	char reply[128];
	char cmd[] = "AT+CPIN?";

	nread = readMobile(cmd,strlen(cmd),reply,sizeof(reply));
	if(  nread == -2){
		return 0;
	}else if(nread < 1){
		return 0;
	}
	if(strstr(reply,"ERROR"))
	{
		return 0;
	}
	return 1;
}

//Mobile network operator
int get_mobile_ismi(char * ttyName,char *ismi)
{
	int nread;
	char reply[128];
	char cmd[] = "AT+CIMI";

	memset(reply,0,sizeof(reply));

	nread = readMobile("ATE1",strlen("ATE1"),reply,sizeof(reply));
	memset(reply,0,sizeof(reply));

	nread = readMobile(cmd,strlen(cmd),reply,sizeof(reply));
	if(  nread == -2){
		return 0;
	}else if(nread < 1){
		return 0;
	}
//	printf("%s:%s\n",__func__,reply);
	if(strstr(reply,"ERROR"))
	{
		return 0;
	}
	if(NULL == strstr(reply,"OK"))
	{
		return 0;
	}

	char *p =  strstr(reply,"AT+CIMI");
	if(p == NULL)
		return 0;

	while(p-reply < sizeof(reply) && !isdigit(*++p));

	if ( sscanf(p,"%[0-9]",ismi) != 1 )
	{
		return 0;
	}else{
		return 1;
	}

	return 0;
}

int get_mobile_csq(char * ttyName,int *csq)
{
	int nread;
	char reply[128];
	char cmd[] = "AT+CSQ";

	memset(reply,0,sizeof(reply));

	nread = readMobile(cmd,strlen(cmd),reply,sizeof(reply));
	if(  nread == -2){
		return -1;
	}else if(nread < 1){
		return -1;
	}
	if(NULL == strstr(reply,"OK"))
	{
		return -1;
	}
	int rssi=0,equ=0;
	char *pos=strchr(reply,':');
	if(NULL == pos)
	{
		return -1;
	}
	sscanf(pos+2,"%d,%d",&rssi,&equ);
//	printf("rssi:%d,equ:%d\n",rssi,equ);
//	if(rssi >31 || rssi < 1)
//	{
//		return 0;
//	}
	*csq = rssi;
	return 0;
}

static int get_lte_service(char * mcc,int *network)
{
	int nread;
	char reply[128];
	char cmd[] = "AT+COPS?";
	char op_name[ 64 ];
	memset(reply,0,sizeof(reply));

	nread = readMobile(cmd,strlen(cmd),reply,sizeof(reply));
	if(nread < 1)
	{
		LOGEX( "%s: readMobile return %d\n", __func__, nread );
		return -1;
	}

	if(NULL == strstr(reply,"OK"))
	{
		LOGEX( "%s: AT command result not OK, buf = %s", __func__, reply );
		return -1;
	}
	char *p = strstr(reply,"+COPS:");
	if(p == NULL)
	{
		LOGEX( "%s: reply buffer format error! buf = %s", __func__, reply );
		return -1;
	}

	memset( op_name, 0, sizeof( op_name ) );
	if ( sscanf(p,"+COPS: %*[0-9],%*[0-9],\"%[^\"]\",%d",op_name, network) == 2 )
	{
		if(strlen(op_name) > 1)
		{
			strcpy(mcc,op_name);
			return 0;
		}
	}

	return 1;
}

static int get_modem_version(char * version)
{
	int nread;
	char reply[128];
	char cmd[] = "AT+GMR";
	memset(reply,0,sizeof(reply));
	char op_name[64];

	nread = readMobile(cmd,strlen(cmd),reply,sizeof(reply));
	if(nread < 1)
	{
		LOGEX( "%s: readMobile return %d\n", __func__, nread );
		return -1;
	}

	if(NULL == strstr(reply,"OK"))
	{
		LOGEX( "%s: AT command result not OK, buf = %s", __func__, reply );
		return -1;
	}
	char *p = strstr(reply, "AT+GMR");
	if (p)
	{
		if (sscanf(p, "AT+GMR\r%s\r", op_name) == 1)
		{
			if (strlen(op_name) > 1)
			{
				strcpy(version, op_name);
				return 1;
			}
		}
	}

	return 0;
}

static void* check_modem_status(void* arg)
{
	int sim_state = 0;
	int network = 0;
	char op_name[64];
	int led_status = LED_NONE;
	memset(op_name,0,sizeof(op_name));
	while(1)
	{
		set_led_status(led_status);
		sleep(1);

		sim_state = get_gprs_sim_state(NULL);
		if(!sim_state)
		{
			printf("%s:sim state=%d\n",__func__,sim_state);
			led_status = LED_SIM_ERROR;
			continue;
		}

		memset(op_name,0,sizeof(op_name));
		network = -1;
		int ret = get_lte_service(op_name, &network);
		if(ret != 0)
		{
			printf("%s:op_name=%s,network=%d\n",__func__, op_name, network);
			led_status = LED_LTE_ERROR;
			continue;
		}
		int csq = 0;
		get_mobile_csq(NULL, &csq);
		printf("%s:csq=%d\n",__func__, csq);
		if(csq < 8)
		{
			led_status = LED_CSQ1;
			continue;
		}
		if(csq < 16)
		{
			led_status = LED_CSQ2;
			continue;
		}
		if(csq < 24)
		{
			led_status = LED_CSQ3;
			continue;
		}
		led_status = LED_CSQ4;


	}
	return NULL;
}

void start_modem_scan(char *pname)
{

	int sim_found = 0;
	char modem_version[64];

	memset(modem_version,0,sizeof(modem_version));
	memset(serialName, 0, sizeof(serialName));
	strcpy(serialName, pname);

	int fd = open(serialName, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd)
	{
		perror(serialName);
		memset(serialName,0,sizeof(serialName));
		return ;
	}


	get_modem_version(modem_version);
	LOGEX("Modem Version:%s\n",modem_version);
	sim_found = get_gprs_sim_state(NULL);
	if (sim_found)
	{
		char ismi[32];
		int csq = 0;
		double mpn = 0;
		char op_name[64];
		int network = -1;

		memset(ismi, 0, sizeof(ismi));
		memset(op_name, 0, sizeof(op_name));
		get_mobile_ismi(NULL, ismi);
		get_mobile_csq(NULL, &csq);
		get_system_mobile_mpn(&mpn);
		get_lte_service(op_name, &network);

		LOGEX("CIMI:%s\n", ismi);
		LOGEX("Lte Provider:%s\nnetwork=%d\n", op_name, network);
		LOGEX("CSQ=%d\n", csq);
		LOGEX("Phone Number:%.0lf\n",mpn);

	}else{
		LOGEX("sim card not found\n");
	}

	pthread_t h_led;
	pthread_create(&h_led, NULL, check_led_status, NULL);
	check_modem_status(NULL);

	return;
}
