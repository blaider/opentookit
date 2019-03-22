// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2014, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 NetdeviceInfo.cpp
// Author:  suchao.wang
// Created: Jul 18, 2014
//
// Description: get or set info for netdevice.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#include <ctype.h>
#include "AwcCommon.h"
//#include "Log.h"
#include <errno.h>
#include <sys/types.h>

#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <net/if_arp.h>		/* For ARPHRD_ETHER */
#include <sys/socket.h>		/* For AF_INET & struct sockaddr */
#include <netinet/in.h>         /* For struct sockaddr_in */
#include <netinet/if_ether.h>
#include "xmlparse.h"

#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <sys/stat.h>

#ifndef DEFFILEMODE
#define DEFFILEMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)/* 0666*/
#endif

struct in_addr GetIP( char const * if_name )
{
	int sockfd;
	struct ifreq ifr;
	int rc;
	struct in_addr addr;
	in_addr_t addrt;
	bzero(&addr,sizeof(struct in_addr));

	sockfd = socket( AF_INET, SOCK_STREAM, 0);
	if (0 == sockfd)
	{
		return addr;
	}

	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name));

	rc = ioctl(sockfd, SIOCGIFADDR, &ifr);

	close(sockfd);

	if (rc != 0)
	{
		return addr;
	}

	if (ifr.ifr_addr.sa_family != AF_INET)
	{
		return addr;
	}

//	LOGEX("NetdeviceInfo::GetIP,%08x",((struct sockaddr_in*) &ifr.ifr_addr)->sin_addr.s_addr);

	addrt = ((struct sockaddr_in*) &ifr.ifr_addr)->sin_addr.s_addr;
	memcpy(&addr,&addrt,sizeof(addrt));
	return addr;

}

static void skipline(FILE *f)
{
  int ch;
  do {
    ch = getc(f);
  } while ( ch != '\n' && ch != EOF );
}
int NetIsExist(const char *ifname)
{
	int ret = 0;
	FILE *pnd;
	char devName[8];
	pnd = fopen("/proc/net/dev", "r");
	if (!pnd)
	{
		return 0;
	}

	/* Skip header */
	skipline(pnd);
	skipline(pnd);

	/* Get interface info */
	 while (!feof(pnd))
	{
		memset(devName, 0, sizeof(devName));
		fscanf(pnd, " %6[^:]:%*[^\n]", devName);
//		printf("find:%s\n",devName);
		if(!strcmp(devName, ifname))
		{
			ret = 1;
			break;
		}
	}
	fclose(pnd);
	return ret;

}

void gpio_set(void * addr,unsigned int offset)
{
	volatile unsigned int *gpio_dout = (volatile unsigned int *)((volatile unsigned char *)addr + 0x13c);
    unsigned int reg = *((volatile unsigned int *)gpio_dout);
    reg |= (1 << offset);
	*((volatile unsigned int *)gpio_dout) = reg;

//	 LOGEX("%s:%x\n",__func__,*(volatile unsigned int *) addr);
	 return;
}
void gpio_clear(void * addr,unsigned int offset)
{
	volatile unsigned int *gpio_dout = (volatile unsigned int *)((volatile unsigned char *)addr + 0x13c);
    unsigned int reg = *((volatile unsigned int *)gpio_dout);
    reg &= ~(1 << offset);
	*((volatile unsigned int *)gpio_dout) = reg;

//	 LOGEX("%s:%x\n",__func__,*(volatile unsigned int *) addr);
	 return;
}
struct gpioinfo
{
	off_t phyAddress;
	int regOffset;
};
#define GPIO0_BASE 0x44E07000
#define GPIO3_BASE 0x481AE000
int restartUSB(int usbBusNo)
{
	struct gpioinfo sinfo[] = {{GPIO0_BASE,4},{GPIO3_BASE,15}};

	if(!usbBusNo || usbBusNo > sizeof(sinfo)/sizeof(sinfo[0]))
		return 0;

	struct gpioinfo *pinfo = &sinfo[usbBusNo-1];
	int mapfd = -1;
	void * viraddr = NULL;
	if (mapfd < 0)
	{
		mapfd = open("/dev/mem", O_RDWR | O_SYNC); //鎵撳紑mem鏂囦欢锛岃闂洿鎺ュ湴鍧€
		if (mapfd == -1)
		{
			return -1;
		}
	}
	viraddr = mmap(0, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, mapfd,
			pinfo->phyAddress);

	volatile unsigned int *gpio_oe =
			(volatile unsigned int *) ((volatile unsigned char *) viraddr
					+ 0x134);
	unsigned int reg = *((volatile unsigned int *) gpio_oe);
	reg &= ~(1 << pinfo->regOffset);
	*((volatile unsigned int *) gpio_oe) = reg;

	gpio_clear(viraddr, pinfo->regOffset);
//	usleep(100 * 1000);
	sleep(1);
	gpio_set(viraddr, pinfo->regOffset);

	close(mapfd);
	return 0;

}
int find_usb_bus_no (const char * usbID,int *usbBusNo)
{
	int ret = 0;
	FILE * fp;
	char line_str[ 1024 * 64 ];

	fp = popen( "/bin/busybox lsusb", "r" );
	if ( NULL == fp )
	{
		return 0;
	}
	memset( line_str, 0,sizeof( line_str ) );
//	printf("find device:%s\n",conn->usbName);
	while ( fgets( line_str, sizeof( line_str ), fp ) != NULL )
	{

		if ( strstr( line_str, usbID ) != NULL )
		{
			if(strlen(usbID) > 4){
				if ( strstr( line_str, usbID ) != NULL ){
					sscanf(line_str,"%*s %d",usbBusNo);
					ret = 1;
					break;
				}
			}
			else{
				sscanf(line_str,"%*s %d",usbBusNo);
				ret = 1;
				break;
			}

		}
	}
	pclose( fp );

	return ret;
}

//#define MOBILE_OPERATOR "/home/root/project/MobileOperator.acr"
int loadOperator(int code,char *buf)
{
	char config_file[PATH_MAX];
	char *value = getenv("TAGLINK_PATH");
	if(value != NULL)
	{
//		LOGEX("TAGLINK_PATH:%s\n",value);
		sprintf(config_file,"%s/project/MobileOperator.acr",value);
	}else{
		sprintf(config_file,"/home/root/project/MobileOperator.acr");
	}
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;

	sprintf(buf,"wan.sh %s ","other");

	doc = xmlParseFile(config_file);
	if (doc == NULL)
	{
		return -1;
	}
	root = xmlDocGetRootElement(doc);
	if (root == NULL)
	{
		xmlFreeDoc(doc);
		return -1;
	}
	xmlNodePtr Config = NULL;
	Config = root->xmlChildrenNode;
	while (Config != NULL)
	{
		if (strcmp((const char*) Config->name, "OPERATOR") == 0)
		{
			xmlChar * name;
			int ocode = 0;
			name = xmlGetProp(Config, BAD_CAST "name");
			ocode = strtoul((char *)xmlGetProp(Config,	BAD_CAST "code"),NULL,10);
			if(ocode == code)
			{
				sprintf(buf,"wan.sh %s ",name);
				printf("%s:%s\n",__func__,buf);
				if( name != NULL)
					 xmlFree(name);
				break;
			}
			if( name != NULL)
				 xmlFree(name);

		}
		Config = Config->next;
	}
	xmlFreeDoc(doc);
	return 0;
}

//
//<serviceproviders format="2.0">
//<!-- Andorra -->
//<country code="ad">
//	<provider>
//		<name>Andorra Telecom (Mobiland)</name>
//		<gsm>
//			<network-id mcc="213" mnc="03"/>
//			<apn value="internetand">
//				<plan type="postpaid"/>
//				<usage type="internet"/>
//				<name>Mobiland</name>
//			</apn>
//			<apn value="internetclic">
//				<plan type="prepaid"/>
//				<usage type="internet"/>
//				<name>Mobiland Click!</name>
//			</apn>
//		</gsm>
//	</provider>
//</country>
//<!-- China -->
//<country code="cn">
//	<provider>
//		<name>China Mobile</name>
//		<gsm>
//			<network-id mcc="460" mnc="00"/>
//			<network-id mcc="460" mnc="02"/>
//			<apn value="cmwap">
//				<plan type="postpaid"/>
//				<usage type="wap"/>
//				<name>WAP</name>
//				<username>guest</username>
//				<password>guest</password>
//			</apn>
//			<apn value="cmnet">
//				<plan type="postpaid"/>
//				<usage type="internet"/>
//				<name>Internet</name>
//				<username>guest</username>
//				<password>guest</password>
//				<dns>211.136.20.203</dns>
//			</apn>
//			<apn value="cmwap">
//				<usage type="mms"/>
//				<name>移动彩信</name>
//				<mmsc>http://mmsc.monternet.com</mmsc>
//				<mmsproxy>10.0.0.172:80</mmsproxy>
//			</apn>
//		</gsm>
//	</provider>
//	<provider>
//		<name>China Unicom</name>
//		<gsm>
//			<network-id mcc="460" mnc="01"/>
//			<apn value="3gnet">
//				<plan type="postpaid"/>
//				<usage type="internet"/>
//				<username>uninet</username>
//			</apn>
//			<apn value="3gwap">
//				<usage type="mms"/>
//				<name>联通彩信</name>
//				<mmsc>http://mmsc.myuni.com.cn</mmsc>
//				<mmsproxy>10.0.0.172:80</mmsproxy>
//			</apn>
//		</gsm>
//	</provider>
//	<provider>
//		<name>China Telecom</name>
//		<cdma>
//			<username>ctnet@mycdma.cn</username>
//			<password>vnet.mobi</password>
//			<sid value="11296"/>
//			<sid value="11298"/>
//		</cdma>
//	</provider>
//</country>
//</serviceproviders>

//#define SERVICE_PROVIDER_FILE "/home/root/bin/serviceproviders.xml"

struct apn_info
{
	char const * mcc;
	char const * mnc;
	char value[ 32 ];
	int found;
	bool in_provider;
	bool in_gsm;
	bool in_apn;
	bool sp_match;
};
typedef struct apn_info					APN_INFO;

static void on_xml_element_begin ( void * data, const char * el, const char ** attr )
{
	APN_INFO * apn = (APN_INFO*) data;
	int i;

	if ( apn->in_apn )
	{
		if ( apn->sp_match )
		{
			if ( strcmp( el, "usage" ) == 0 )
			{
				for ( i = 0; attr[ i ] != NULL; i += 2 )
				{
					if ( strcmp( attr[ i ], "type" ) == 0 )
					{
						if ( strcmp( attr[ i + 1 ], "internet" ) == 0 )
						{
							apn->found = true;
							apn->sp_match = false; // forbid the following apn to overwrite the apn value
						}
					}
				}
			}
		}
	}
	else
	{
		if ( apn->in_gsm )
		{
			if ( !apn->sp_match )
			{
				if ( strcmp( el, "network-id" ) == 0 )
				{
					bool mcc_match = false;
					bool mnc_match = false;

					for ( i = 0; attr[ i ] != NULL; i += 2 )
					{
						if ( strcmp( attr[ i ], "mcc" ) == 0 )
						{
							if ( strcmp( attr[ i + 1 ], apn->mcc ) == 0 )
							{
								mcc_match = true;
							}
						}
						else if ( strcmp( attr[ i ], "mnc" ) == 0 )
						{
							if ( strcmp( attr[ i + 1 ], apn->mnc ) == 0 )
							{
								mnc_match = true;
							}
						}
					}

					if ( mcc_match && mnc_match )
					{
						apn->sp_match = true;
					}
				}
			}
			else
			{
				if ( strcmp( el, "apn" ) == 0 )
				{
					apn->in_apn = true;
					for ( i = 0; attr[ i ] != NULL; i += 2 )
					{
						if ( strcmp( attr[ i ], "value" ) == 0 )
						{
							strncpy( apn->value, attr[ i + 1 ], sizeof( apn->value ) );
						}
					}
				}
			}
		}
		else
		{
			if ( apn->in_provider )
			{
				if ( strcmp( el, "gsm" ) == 0 )
				{
					apn->in_gsm = true;
				}
			}
			else
			{
				if ( strcmp( el, "provider" ) == 0 )
				{
					apn->in_provider = true;
				}
			}
		}
	}
}

static void on_xml_element_end ( void * data, const char * el )
{
	APN_INFO * apn = (APN_INFO*) data;

	if ( apn->in_apn )
	{
		if ( strcmp( el, "apn" ) == 0 )
		{
			apn->in_apn = false;
		}
	}
	else if ( apn->in_gsm )
	{
		if ( strcmp( el, "gsm" ) == 0 )
		{
			apn->in_gsm = false;
		}
	}
	else if ( apn->in_provider )
	{
		if ( strcmp( el, "provider" ) == 0 )
		{
			apn->in_provider = false;
		}
	}
}

bool find_provider_apn (
	char *apnvalue,
	size_t len,
	char const * mcc,
	char const * mnc )
{
	XML_Parser parser;
	APN_INFO apn;
	char file_name[PATH_MAX];// = SERVICE_PROVIDER_FILE;
	int done;
	FILE * fp;

	char *value = getenv("TAGLINK_PATH");
	if(value != NULL)
	{
		sprintf(file_name,"%s/bin/serviceproviders.xml",value);
	}else{
		sprintf(file_name,"/home/root/bin/serviceproviders.xml");
	}
	memset( &apn, 0, sizeof( apn ) );
	apn.mcc = mcc;
	apn.mnc = mnc;

	fp = fopen( file_name, "r" );
	if ( NULL == fp )
	{
//		LOGEX( "%s: Couldn't open file %s", __func__, file_name );
		return false;
	}

	parser = XML_ParserCreate( NULL );
	if ( NULL == parser )
	{
//		LOGEX( "%s: Couldn't allocate memory for parser", __func__ );
		fclose( fp );
		return false;
	}

	XML_SetUserData( parser, &apn );
	XML_SetElementHandler( parser, on_xml_element_begin, on_xml_element_end );

	do
	{
		char buf [ 8192 ];
		int len;
		bool bom_detect = false;
		int offset;

		len = fread( buf, 1, sizeof( buf ), fp );
		if ( ferror( fp ) )
		{
//			LOGEX( "%s: Read error", __func__ );
			break;
		}
		done = feof( fp );

		offset = 0;
		if ( !bom_detect && len > 3 )
		{
			if ( memcmp( buf, "\xef\xbb\xbf", 3 ) == 0 )
			{
				offset = 3;
				len -= 3;
			}
			bom_detect = true;
		}

		if ( !XML_Parse( parser, buf + offset, len, done ) )
		{
//			LOGEX(
//				"%s: Parse error at line %d:\n%s",
//				__func__,
//				XML_GetCurrentLineNumber( parser ),
//				XML_ErrorString( XML_GetErrorCode( parser ) ) );
			done = false;
			break;
		}
	} while ( !done && !apn.found );

	fclose( fp );
	XML_ParserFree( parser );

	if ( !done && !apn.found )
	{
//		LOGEX( "%s: load xml file %s failed!", __func__, file_name );
	}

	if ( apn.found )
	{
//		LOGEX(
//			"%s: found apn = %s for mcc = %s, mnc = %s.",
//			__func__,
//			apn.value,
//			apn.mcc,
//			apn.mnc );
		strncpy( apnvalue, apn.value, len );
	}
	else
	{
//		LOGEX(
//			"%s: fail to found apn for mcc = %s, mnc = %s.",
//			__func__,
//			apn.mcc,
//			apn.mnc );
	}

	return apn.found;
}

static void serial_init(int fd)
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


int readMobile(char *ttyName,char *cmd,int cmdLen,char *res,int resLen)
{
	if(cmd == NULL || res == NULL)
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
//	printf("%s\n",serialName);
	fd = open(ttyName, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd)
	{
		perror(ttyName);
		return -1;
	}
	serial_init(fd);

	nwrite = write(fd, buf, cmdLen);
	if(cmdLen != nwrite )
	{
		close(fd);
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

static int set_cops_mode(char * ttyName )
{
	int nread;
	char reply[128];
	char cmdSetMode[] = "AT+COPS=0,2";

//	LOGEX( "%s: tty = %s", __func__, ttyName );
	memset(reply,0,sizeof(reply));

	nread = readMobile(ttyName,cmdSetMode,strlen(cmdSetMode),reply,sizeof(reply));
	if(nread < 1)
	{
//		LOGEX( "%s: readMobile return %d\n", __func__, nread );
		return -1;
	}

	if(NULL == strstr(reply,"OK"))
	{
//		LOGEX( "%s: AT command [AT+COPS=0,2] result not OK, buf = %s", __func__, reply );
		return -1;
	}

	return 0;
}

static int get_mno_info(char * ttyName, char * mcc, char * mnc )
{
	int nread;
	char reply[128];
	char cmd[] = "AT+COPS?";
	char op_name[ 64 ];
	memset(reply,0,sizeof(reply));

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
	if(nread < 1)
	{
//		LOGEX( "%s: readMobile return %d\n", __func__, nread );
		return -1;
	}

	if(NULL == strstr(reply,"OK"))
	{
//		LOGEX( "%s: AT command result not OK, buf = %s", __func__, reply );
		return -1;
	}
	char *p = strstr(reply,"+COPS:");
	if(p == NULL)
	{
//		LOGEX( "%s: reply buffer format error! buf = %s", __func__, reply );
		return -1;
	}

	memset( op_name, 0, sizeof( op_name ) );
	if ( sscanf(p,"+COPS: %*[0-9],%*[0-9],\"%[0-9]\",%*[0-9]",op_name) != 1 )
	{
//		LOGEX( "%s: scan operator name failed! buf = %s", __func__, p );
		if ( sscanf(p,"+COPS: %*[0-9],%*[0-9],\"%s\",%*[0-9]",op_name) == 1 )
		{
			return 1;
		}
		return -1;
	}

//	LOGEX( "%s: operator code = %s", __func__, op_name );

	memcpy( mcc, op_name, 3 );
	mcc[ 3 ] = 0;

	strcpy( mnc, &op_name[ 3 ] );

//	LOGEX( "%s: tty = %s,mcc = %s,mnc = %s", __func__, ttyName , mcc, mnc );
	return 0;
}

//Mobile network operator
static int get_system_mobile_mno(char * ttyName, char * mcc, char * mnc )
{
	int ret = -1;
	ret = get_mno_info(ttyName,mcc,mnc);
	if(ret == 0)
		return 0;
	//set cops type
	if(ret == 1)
	{
		int wait_sleep = 36;
		set_cops_mode(ttyName);
		while(wait_sleep--)
		{
			int csq;
			int iret = get_mobile_csq(ttyName,&csq);
			if(iret == 0)
//				LOGEX( "%s: csq = %d", __func__, csq );
			ret = get_mno_info(ttyName,mcc,mnc);
			if(ret == 0)
				break;

			sleep(5);
		}
	}else{
		return ret;
	}

	return ret;
}

#if 0
int checkOperator(char *ttyPort,char *command)
{
	int ret = 0;
	int oper=get_system_mobile_mno(ttyPort);
	if(oper <= 0 )
		return ret;
	loadOperator(oper,command);
	printf("%s:%d,%s,%s\n",__func__,oper,command,command);
	if(strlen(command) > 3)
	{
		strcat(command,ttyPort);
		ret = 1;
	}
	return ret;
}
#endif


//Mobile sim card status
int get_gprs_sim_state(char * ttyName)
{
	int nread;
	char reply[128];
	char cmd[] = "AT+CPIN?";

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
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

	nread = readMobile(ttyName,"ATE1",strlen("ATE1"),reply,sizeof(reply));
	memset(reply,0,sizeof(reply));

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
	if(  nread == -2){
		return 0;
	}else if(nread < 1){
		return 0;
	}
	printf("%s:%s\n",__func__,reply);
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

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
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


int checkProvider ( char *ttyPort, char *command, int netmode,char *usbLabel)
{
	char mcc[ 8 ];
	char mnc[ 8 ];
	char apnname[ 32 ];
	int ret = 0;

	ret = get_system_mobile_mno( ttyPort, mcc, mnc );
	if ( ret != 0 )
	{
		int csq;
//		LOGEX( "%s: get_system_mobile_mno failed! return = %d", __func__, ret );
		int iret = get_mobile_csq(ttyPort,&csq);
//		if(iret == 0)
//			LOGEX( "%s: get_mobile_csq = %d", __func__, csq );
		return ret;
	}

	if(strcmp(mcc,"460") == 0 &&( strcmp(mnc,"03") == 0 || strcmp(mnc,"11") == 0))
	{
		ret = 1;
//		LOGEX( "%s: find operator is telecom ", __func__);
	}
	if ( ret == -1 )
	{
		return ret;
	}

	if( ret == 1 )
	{
		generate_cdma_script();
	}else
	{
		memset( apnname, 0, sizeof( apnname ) );
		find_provider_apn( apnname, sizeof( apnname ), mcc, mnc );
		if ( strlen( apnname ) > 0 )
		{
			int find = 0;
//			LOGEX( "%s: find apn = %s, netmode = %d", __func__, apnname ,netmode);
			find =  generate_apn_netmode_script( apnname , netmode ,usbLabel );
			if ( !find )
				generate_default_script( netmode ,usbLabel );
		}
		else
		{
//			LOGEX( "%s: no apn , netmode = %d", __func__, netmode );
			generate_default_script( netmode ,usbLabel );
		}
	}

//	loadOperator(oper,command);

	sprintf( command, "wan.sh default " );
	printf( "%s:mcc = %s, mnc = %s,ret = %d, %s\n", __func__, mcc, mnc, ret, command);
	if ( strlen( command ) > 3 )
	{
		strcat( command, ttyPort );
		ret = 1;
	}
	return ret;
}

#define DIRS "/dev/serial/by-id/"
#define PRE_LINK "../../"

int usbLable_detect(char *usbLable)
{
	FILE * fd;
	char line[4096];
	char name[4096];
	sprintf(name,"ls %susb-%s*",DIRS,usbLable);
	fd = popen(name, "r");
	if(fd == NULL)
	{
		perror(DIRS);
		return -1;
	}

	int inx = 0;

	while (fgets(line, sizeof(line), fd))
	{
//		printf("read:%s",line);
		inx++;
	}
	pclose(fd);
	return inx;
}



int get_gprs_config_node_name(char *usbLable,int ttyPortNo,char *ttyPort)
{
	int ret = 0;
	FILE * fd;
	char line[4096];
	char name[4096],linkName[4096];
	sprintf(name,"ls %susb-%s*",DIRS,usbLable);
	fd = popen(name, "r");
	if(fd == NULL)
	{
		perror(DIRS);
		return -1;
	}

	int inx = 0;

	while (fgets(line, sizeof(line), fd))
	{
//		printf("read:%s",line);
		sprintf(name,"%s",line);
		if(name[strlen(name)-1] == 0xa || name[strlen(name)-1] == 0xd)
			name[strlen(name)-1] = 0x0;
		memset(linkName,0,sizeof(linkName));
		int linkSize = readlink(name,linkName,sizeof(linkName));
		if(-1 == linkSize)
		{
//			perror("readlink");
			ret = -1;
			continue;
		}
//		LOGEX("%s %s %s\n",name,(linkSize > 0)?" ->":"",linkName);
		if(ttyPortNo == inx )
		{
			if (strncmp(linkName, PRE_LINK, strlen(PRE_LINK)) == 0)
			{
				sprintf(ttyPort, "/dev/%s", linkName + strlen(PRE_LINK));
				ret = 1;
			}
		}
		inx++;
	}
	pclose(fd);
	if(access(ttyPort,F_OK))
		return -1;
	return ret;
}

int checkProcess(const char *name)
{
	char buff[80];
	pid_t pid = 0;
	char *ret;

	sprintf(buff, "pidof %s", name);
	FILE *pidreader = popen(buff, "r");
	memset(buff, 0, 80);

	ret = fgets(buff, 79, pidreader);
	pclose(pidreader);

	ret = strtok(buff, " ");
	while (ret != NULL)
	{
//		int status;
		pid = atoi(ret);
		ret =strtok(NULL," ");
		break;
	}
	return pid;
}

//0 failed,1 success,2 not enough time
int system_ping(const char *ifname,PINGURL * urls)
{
	if(!urls->lastPingTime)
//		urls->lastPingTime = time(NULL);
		urls->lastPingTime = GetTickCount();

	//won't check this time
	if(GetTickCount() - urls->lastPingTime < urls->pingInterval)
		return 2;

	int ret = -1;
	unsigned int i=0;
	for(i=0;i<sizeof(urls->url)/sizeof(urls->url[0]);i++)
	{
		if(strlen(urls->url[i]) < 1)
			continue;
		char cmd[2056];
		sprintf(cmd,"ping %s -c 1 -W 10 -s 8  -I %s",urls->url[i],ifname);
		printf("%s:%s\n",__func__,cmd);
		ret = system(cmd);
		printf("%s:ret=%d\n",__func__,ret);
		if(!WEXITSTATUS(ret))
		{
//			urls->lastPingTime = time(NULL);
			urls->lastPingTime = GetTickCount();
			break;
		}
		printf("%s:%s failed:%d\n",__func__,cmd,i);
	}
	return !ret;
}

//0 failed,1 success,2 not enough time
int traffic_monitor(const char *ifName,TRAFFICDETAIL *tdetail)
{
//	printf("%s:mtime:%ld,interval:%d,left=%ld\n",__func__,tdetail.mtime,gprsSilenceMinutes*60,gprsSilenceMinutes*60+tdetail.mtime-time(NULL));
//	TRAFFICDETAIL *tdetail = &conn->tdetail;
	if(!tdetail->trafficMinutes)
		return 1;

	if(!tdetail->mtime)
		tdetail->mtime = GetTickCount();

	//don't check this time;
	if(GetTickCount() - tdetail->mtime  < tdetail->trafficMinutes*60)
	{
		return 2;
	}
	int ret = 1;
	FILE *pnd;
	struct ifinfo
	{
		char name[8];
		unsigned int r_bytes, r_pkt, r_err, r_drop, r_fifo, r_frame;
		unsigned int r_compr, r_mcast;
		unsigned int x_bytes, x_pkt, x_err, x_drop, x_fifo, x_coll;
		unsigned int x_carrier, x_compr;
	} ifc;

	pnd = fopen("/proc/net/dev", "r");
	if (!pnd)
	{
		fprintf(stderr, "%s: /proc/net/dev: %s", "aa", strerror(errno));
		exit(1);
	}

	/* Skip header */
	skipline(pnd);
	skipline(pnd);

	/* Get interface info */
	do
	{
		memset(ifc.name, 0, sizeof(ifc.name));
		int ret = fscanf(pnd,
				" %6[^:]:%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u ",
				ifc.name, &ifc.r_bytes, &ifc.r_pkt, &ifc.r_err, &ifc.r_drop,
				&ifc.r_fifo, &ifc.r_frame, &ifc.r_compr, &ifc.r_mcast,
				&ifc.x_bytes, &ifc.x_pkt, &ifc.x_err, &ifc.x_drop,
				&ifc.x_fifo, &ifc.x_coll, &ifc.x_carrier, &ifc.x_compr);
		if (ret != 17)
		{
			break;
		}
	} while (strcmp(ifc.name, ifName));
	fclose(pnd);
	if (!strcmp(ifc.name, ifName))
	{
		if((ifc.r_bytes != tdetail->rxbytes) && (ifc.x_bytes != tdetail->rxbytes))
		{
			tdetail->mtime = GetTickCount();
			tdetail->rxbytes = ifc.r_bytes;
			tdetail->rxbytes = ifc.x_bytes;
			ret = 1;
		}else
		{
			ret = 0;
		}
	}else{
		ret = 0;
	}
  return ret;
}



//#define NETMODE_FILE	"/home/root/project/"
static int getModeScript(char **modbuff,char *usbLabel)
{
	printf("%s+++\n",__func__);
//	char filename[PATH_MAX];
	char buff[1024];
	long filesize = 0;
	int buffOffset = 0;
//	sprintf(filename,"%s%s",NETMODE_FILE,usbLabel);
//	const char * filename = "/home/root/project/urat";

	char filename[PATH_MAX];
	char *value = getenv("TAGLINK_PATH");
	if(value != NULL)
	{
		sprintf(filename,"%s/project/urat",value);
	}else{
		sprintf(filename,"/home/root/project/urat");
	}

	if(access(filename,F_OK))
	{
		return -1;
	}

	FILE *fp;
	fp = fopen(filename,"r");
	 if(NULL==fp)
	{
		return -1;//要返回错误代码
	}

	 //get file size
	 fseek(fp,0,SEEK_END);
	 filesize = ftell(fp);
	 fseek(fp,0,SEEK_SET);

	 *modbuff = (char *)malloc(filesize+256);
	 memset(*modbuff,0,filesize+256);

//	 printf("%s,filelen:%ld\n",__func__,filesize);

	 memset(buff,0,sizeof(buff));
	 while(fgets(buff,sizeof(buff),fp))
	 {
//		 printf("%s:buff:%s\n",__func__,buff);
		 char *p = NULL;
		 char *pbuff = buff;
		 if(memcmp(pbuff,"OK",2))
		 {
			 strcpy((*modbuff)+buffOffset,"OK ");
			 buffOffset = strlen(*modbuff);
		 }
//		 while(*pbuff != 0){
//
//			 p = strstr(pbuff,"^");
//			 if(p){
//				 memcpy((*modbuff)+buffOffset,pbuff,p-pbuff);
//				 if(*(p-1) != '\\')
//				 {
//					*((*modbuff)+buffOffset) = '\\';
//					 buffOffset++;
//				 }
//				 pbuff = p;
//			 }else
//			 {
//				 strcpy((*modbuff)+buffOffset,pbuff);
//				 buffOffset = strlen(*modbuff);
//			 }
//
//		 }
		 do{
			 p = strstr(pbuff,"^");
			 if(p){
				 memcpy((*modbuff)+buffOffset,pbuff,p-pbuff);
				 buffOffset += p-pbuff;
				 if(*(p-1) != '\\')
				 {
					*((*modbuff)+buffOffset) = '\\';
					 buffOffset++;
				 }
				 *((*modbuff)+buffOffset) = *p++;
				 buffOffset++;
				 pbuff = p;
//				 printf("%s:*modbuff:%s,buffOffset= %d\n",__func__,*modbuff,buffOffset);
			 }else
			 {
				 strcpy((*modbuff)+buffOffset,pbuff);
				 buffOffset = strlen(*modbuff);
				 pbuff += strlen(pbuff);
			 }
		 }while(*(pbuff) != 0);
//		 strcpy((*modbuff)+buffOffset,pbuff);
//		 buffOffset = strlen(*modbuff);
		 if(*((*modbuff)+buffOffset -1) != '\n')
		 {
			 *((*modbuff)+buffOffset) = '\n';
			 buffOffset = strlen(*modbuff);
		 }

//		 printf("%s:*modbuff:%s,buffOffset= %d\n",__func__,*modbuff,buffOffset);

	 }
	 fclose(fp);

	return 0;
}


char *default_apn =
"debug\n"\
"nodetach\n"\
"lock\n"\
"/dev/ttyUSB1\n"\
"115200\n"\
"nocrtscts\n"\
"modem\n"\
"usepeerdns\n"\
"noauth\n"\
"noipdefault\n"\
"novj\n"\
"novjccomp\n"\
"noccp\n"\
"defaultroute\n"\
"#lcp-echo-failure 5\n"\
"#lcp-echo-interval 30\n"\
"persist\n"\
"ipcp-accept-local\n"\
"persist\n"\
"connect \'/usr/sbin/chat -s -v -f /etc/ppp/peers/default-chat-connect\'\n"\
"disconnect \'/usr/sbin/chat -s -v -f/etc/ppp/peers/default-chat-disconnect\'";

char *default_apn_connect =
		"TIMEOUT 30\n"\
		"ABORT \"NO CARRIER\"\n"\
		"ABORT \"ERROR\"\n"\
		"ABORT \"NO DIALTONE\"\n"\
		"ABORT \"BUSY\"\n"\
		"ABORT \"NO ANSWER\"\n"\
		"\"\" AT\n"\
		"OK ATZ\n"\
		"OK AT+CGDCONT=1,\"IP\",\"apn\"\n"\
		"OK ATDT*99#\n"\
		"CONNECT \"\"";

char *default_apn_connect2 =
		"OK AT+CGDCONT=1,\"IP\",\"apn\"\n"\
		"OK ATDT*99#\n"\
		"CONNECT \"\"";

char *apn_connect =
		"TIMEOUT 30\n"\
		"ABORT \"NO CARRIER\"\n"\
		"ABORT \"ERROR\"\n"\
		"ABORT \"NO DIALTONE\"\n"\
		"ABORT \"BUSY\"\n"\
		"ABORT \"NO ANSWER\"\n"\
		"\"\" AT\n"\
		"OK ATZ\n"\
		"OK AT+CGDCONT=1,\"IP\",\"%s\"\n"\
		"OK ATDT*99#\n"\
		"CONNECT \"\"";


char *apn_netmode_connect1 =
		"TIMEOUT 30\n"\
		"ABORT \"NO CARRIER\"\n"\
		"ABORT \"ERROR\"\n"\
		"ABORT \"NO DIALTONE\"\n"\
		"ABORT \"BUSY\"\n"\
		"ABORT \"NO ANSWER\"\n"\
		"\"\" AT\n"\
		"OK ATZ\n";

char *apn_netmode_connect2 =
		"OK AT+CGDCONT=1,\"IP\",\"%s\"\n"\
		"OK ATDT*99#\n"\
		"CONNECT \"\"";

char *cdma_connect =
		"TIMEOUT 30\n"\
		"ABORT \"NO CARRIER\"\n"\
		"ABORT \"ERROR\"\n"\
		"ABORT \"NO DIALTONE\"\n"\
		"ABORT \"BUSY\"\n"\
		"ABORT \"NO ANSWER\"\n"\
		"\"\" AT\n"\
		"OK ATZ\n"\
		"OK ATDT#777\n"\
		"CONNECT \"\"";

char *default_apn_disconnect =
		"ABORT \"ERROR\"\n"\
		"ABORT \"NO DIALTONE\"\n"\
		"SAY \"\\nSending break to the modem\\n\"\n"\
		"\'\' \"\\K\"\n"\
		"\'\' \"+++ATH\"\n"\
		"SAY \"\\nGoodbay\\n\"";

#define CHAT_DIR "/etc/ppp/peers/"
#define CHAT_NAME "default"
#define CHAT_CONNECT "default-chat-connect"
#define CHAT_DISCONNECT "default-chat-disconnect"
int generate_default_script(int mode,char *usbLabel)
{
	char *modebuff = NULL;

	if(mode != 1)
		getModeScript(&modebuff,usbLabel);

	if(access(CHAT_DIR CHAT_NAME,F_OK))
	{
		int fd = open(CHAT_DIR CHAT_NAME,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,default_apn,strlen(default_apn));
		close(fd);

	}

//	if(access(CHAT_DIR CHAT_CONNECT,F_OK))
	{
		int fd = open(CHAT_DIR CHAT_CONNECT,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
//		write(fd,default_apn_connect,strlen(default_apn_connect));
		write(fd,apn_netmode_connect1,strlen(apn_netmode_connect1));
		if(modebuff)
			write(fd,modebuff,strlen(modebuff));
		write(fd,default_apn_connect2,strlen(default_apn_connect2));
		close(fd);

	}

	if(access(CHAT_DIR CHAT_DISCONNECT,F_OK))
	{
		int fd = open(CHAT_DIR CHAT_DISCONNECT,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,default_apn_disconnect,strlen(default_apn_disconnect));
		close(fd);

	}

	if(modebuff)
		free(modebuff);
	return 1;
}

int generate_cdma_script()
{

	if(access(CHAT_DIR CHAT_NAME,F_OK))
	{
		int fd = open(CHAT_DIR CHAT_NAME,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,default_apn,strlen(default_apn));
		close(fd);

	}

//	if(access(CHAT_DIR CHAT_CONNECT,F_OK))
	{
		int fd = open(CHAT_DIR CHAT_CONNECT,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,cdma_connect,strlen(cdma_connect));
		close(fd);

	}

	if(access(CHAT_DIR CHAT_DISCONNECT,F_OK))
	{
		int fd = open(CHAT_DIR CHAT_DISCONNECT,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,default_apn_disconnect,strlen(default_apn_disconnect));
		close(fd);

	}

	return 1;
}

#if 0
int generate_apn_script(char *apnname)
{

	if(access(CHAT_DIR CHAT_NAME,F_OK))
	{
		int fd = open(CHAT_DIR CHAT_NAME,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,default_apn,strlen(default_apn));
		close(fd);

	}

//	if(access(CHAT_DIR CHAT_CONNECT,F_OK))
	{
		char *buff = (char *)calloc(1,strlen(apn_connect)+strlen(apnname)+20);
		sprintf(buff,apn_connect,apnname);
		int fd = open(CHAT_DIR CHAT_CONNECT,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,buff,strlen(buff));
		close(fd);
		free(buff);

	}

	if(access(CHAT_DIR CHAT_DISCONNECT,F_OK))
	{
		int fd = open(CHAT_DIR CHAT_DISCONNECT,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,default_apn_disconnect,strlen(default_apn_disconnect));
		close(fd);

	}

	return 1;
}
#endif



int generate_apn_netmode_script(char *apnname , int mode,char *usbLabel)
{

	char *modebuff = NULL;

	if(mode != 1)
		getModeScript(&modebuff,usbLabel);


	if(access(CHAT_DIR CHAT_NAME,F_OK))
	{
		int fd = open(CHAT_DIR CHAT_NAME,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,default_apn,strlen(default_apn));
		close(fd);

	}

//	if(access(CHAT_DIR CHAT_CONNECT,F_OK))
	{
		char *buff = (char *)calloc(1,strlen(apn_netmode_connect2)+strlen(apnname)+20);
		sprintf(buff,apn_netmode_connect2,apnname);
		int fd = open(CHAT_DIR CHAT_CONNECT,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,apn_netmode_connect1,strlen(apn_netmode_connect1));
		if(modebuff)
			write(fd,modebuff,strlen(modebuff));
		write(fd,buff,strlen(buff));
		close(fd);
		free(buff);

	}

	if(access(CHAT_DIR CHAT_DISCONNECT,F_OK))
	{
		int fd = open(CHAT_DIR CHAT_DISCONNECT,O_RDWR|O_CREAT|O_TRUNC,DEFFILEMODE);
		if( fd < 0)
			return 0;
		write(fd,default_apn_disconnect,strlen(default_apn_disconnect));
		close(fd);

	}

	if(modebuff)
		free(modebuff);
	return 1;
}

// 返回自系统开机以来的毫秒数（tick）
unsigned long GetTickCount()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec);
}


char *strlwr(char *str){
    char *orign=str;
    for (; *str != '\0'; str++)
        *str = tolower(*str);
    return orign;
}

int IsExist(const char *ifname)
{
	int ret = 0;
	FILE *pnd;
	char devName[8];
	pnd = fopen("/proc/net/dev", "r");
	if (!pnd)
	{
		return 0;
	}

	/* Skip header */
	skipline(pnd);
	skipline(pnd);

	/* Get interface info */
	 while (!feof(pnd))
	{
		memset(devName, 0, sizeof(devName));
		fscanf(pnd, " %6[^:]:%*[^\n]", devName);
//		printf("find:%s\n",devName);
		if(!strcmp(devName, ifname))
		{
			ret = 1;
			break;
		}
	}
	fclose(pnd);
	return ret;

}
int get_system_ip(const char *ifname,in_addr_t *add)
{
	if(!IsExist(ifname))
		return 0;
	int sockfd;
	struct ifreq ifr;
	int rc;
	struct in_addr addr = {0};
	in_addr_t addrt = {0};
	bzero(&addr,sizeof(struct in_addr));
	bzero(&addrt,sizeof(addrt));

	sockfd = socket( AF_INET, SOCK_STREAM, 0);
	if (0 == sockfd)
	{
		return 0;
	}

	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

	rc = ioctl(sockfd, SIOCGIFADDR, &ifr);

	close(sockfd);

	if (rc != 0)
	{
		return 0;
	}

	if (ifr.ifr_addr.sa_family != AF_INET)
	{
		return 0;
	}

//	LOGEX("NetdeviceInfo::GetIP,%08x",((struct sockaddr_in*) &ifr.ifr_addr)->sin_addr.s_addr);

	addrt = ((struct sockaddr_in*) &ifr.ifr_addr)->sin_addr.s_addr;
	memcpy(&addr,&addrt,sizeof(addrt));
	*add = addrt;
	return 1;
}


int net_is_up(char* net_name)
{
        int skfd = 0;
        struct ifreq ifr;

        skfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(skfd < 0) {
                printf("%s:%d Open socket error!\n", __FILE__, __LINE__);
                return 0;
        }

        strcpy(ifr.ifr_name, net_name);

        if(ioctl(skfd, SIOCGIFFLAGS, &ifr) <0 ) {
                printf("%s:%d IOCTL error!\n", __FILE__, __LINE__);
                printf("Maybe ethernet inferface %s is not valid!", ifr.ifr_name);
                close(skfd);
                return 0;
        }

        if(ifr.ifr_flags & IFF_UP) {
                return 1;
        } else {
                return 0;
        }
        return 0;
}

int find_usb_bus_by_list (WIFI_DEV_LIST *list,char *usbid)
{
	if(list == NULL)
		return 0;
	FILE * fp;
	char line_str[ 1024 * 64 ];
	int usbBusNo = 0;
	char usbDeviceID[32];
	int findbus = 0;

	fp = popen( "/bin/busybox lsusb", "r" );
	if ( NULL == fp )
	{
		return 0;
	}
	memset( line_str, 0,sizeof( line_str ) );

//	printf("find device:%s\n",conn->usbName);
	while ( fgets( line_str, sizeof( line_str ), fp ) != NULL && !findbus)
	{
		sscanf(line_str,"%*s %d %*s %*s %*s %s",&usbBusNo,usbDeviceID);
		if(strcmp(usbDeviceID,"1d6b:0002"))
		{
			WIFI_DEV_LIST *l = list;
			while(l != NULL)
			{
				if(!strcmp(usbDeviceID,l->device_id))
				{
					strcpy(usbid,usbDeviceID);
					findbus = 1;
					break;
				}
				l=l->next;
			}

		}
		if(findbus)
			break;
	}
	pclose( fp );

	if(findbus)
		return usbBusNo;
	return 0;
}

int load_usb_device_list (char *configfile,WIFI_DEV_LIST **list)
{

	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;

	doc = xmlParseFile(configfile);
	if (doc == NULL)
	{
		return -1;
	}
	root = xmlDocGetRootElement(doc);
	if (root == NULL)
	{
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return -1;
	}
	xmlNodePtr Config = NULL;
	Config = root->xmlChildrenNode;
	while (Config != NULL)
	{
		if (strcmp((const char*) Config->name, "Device") == 0)
		{
			xmlChar *devname,*device_id;
			devname = xmlGetProp(Config, BAD_CAST "deviceName");
			device_id = xmlGetProp(Config, BAD_CAST "deviceID");
			WIFI_DEV_LIST *node = malloc(sizeof(WIFI_DEV_LIST));
			if(node == NULL)
			{
				xmlFreeDoc(doc);
				return -1;
			}
			printf("Support Device:%s[%s]\n",devname,device_id);
			strcpy(node->display_name,(char *)devname);
			strcpy(node->device_id,(char*)device_id);
			node->next = *list;
			*list = node;
		}
		Config = Config->next;
	}
	xmlFreeDoc(doc);
	return 0;
}

int find_usbdevice_by_label(const char *lable)
{
	FILE * fd;
	char line[4096];
	char name[4096];
	sprintf(name,"ls %susb-%s*",DIRS,lable);
	fd = popen(name, "r");
	if(fd == NULL)
	{
		perror(DIRS);
		return 0;
	}

	int inx = 0;

	while (fgets(line, sizeof(line), fd))
	{
		inx++;
	}
	pclose(fd);
	return inx;
}
int find_usbdevice_by_id (const char *usbid)
{
	FILE * fp;
	char line_str[ 1024 * 64 ];
	int usbBusNo = 0;
	char usbDeviceID[32];
	int findbus = 0;

	fp = popen( "/bin/busybox lsusb", "r" );
	if ( NULL == fp )
	{
		return 0;
	}
	memset( line_str, 0,sizeof( line_str ) );

//	printf("find device:%s\n",conn->usbName);
	while ( fgets( line_str, sizeof( line_str ), fp ) != NULL && !findbus)
	{
		sscanf(line_str,"%*s %d %*s %*s %*s %s",&usbBusNo,usbDeviceID);
		if(strcmp(usbDeviceID,usbid) == 0)
		{
			findbus = 1;
			break;
		}
	}
	pclose( fp );
	return findbus;
}

#define MEM_KEY  0x1112
/*共享内存大小*/
#define MEM_SIZE 32

static int g_mid;
/*共享内存初始化*/
SHMINFO * ShareMemInit()
{
	gpio_init(1);
	g_mid = shmget(MEM_KEY,MEM_SIZE,IPC_CREAT | 0666);
	if(g_mid < 0 ){
		g_mid = shmget(MEM_KEY, 0, 0);
	}
	if(g_mid < 0 )
		return NULL;

	return (SHMINFO *)shmat(g_mid,0,0);
}
