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
#include "Log.h"
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

#include "xmlparse.h"

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

void skipline(FILE *f)
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
	usleep(100 * 1000);
	gpio_set(viraddr, pinfo->regOffset);

	close(mapfd);
	return 0;

}
int usb_detect (const char *usbName)
{
	int ret = 0;
	FILE * fp;
	char line_str[ 1024 * 64 ];

	fp = popen( "lsusb", "r" );
	if ( NULL == fp )
	{
		return 0;
	}
	memset( line_str, 0,sizeof( line_str ) );
//	printf("find device:%s\n",conn->usbName);
	while ( fgets( line_str, sizeof( line_str ), fp ) != NULL )
	{

		if ( strstr( line_str, usbName ) != NULL )
		{
			ret = 1;
			break;
		}
	}
	pclose( fp );

	return ret;
}

int find_usb_bus_no (const char *usbName,int *usbBusNo)
{
	int ret = 0;
	FILE * fp;
	char line_str[ 1024 * 64 ];

	fp = popen( "lsusb | sort", "r" );
	if ( NULL == fp )
	{
		return 0;
	}
	memset( line_str, 0,sizeof( line_str ) );
//	printf("find device:%s\n",conn->usbName);
	while ( fgets( line_str, sizeof( line_str ), fp ) != NULL )
	{

		if ( strstr( line_str, usbName ) != NULL )
		{
			sscanf(line_str,"%*s %d",usbBusNo);
			ret = 1;
			break;
		}
	}
	pclose( fp );

	return ret;
}

#define MOBILE_OPERATOR "/home/root/project/MobileOperator.acr"
int loadOperator(int code,char *buf)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr root = NULL;

	sprintf(buf,"wan.sh %s ","other");

	doc = xmlParseFile(MOBILE_OPERATOR);
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

#define SERVICE_PROVIDER_FILE "/home/root/bin/serviceproviders.xml"

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
	char const * file_name = SERVICE_PROVIDER_FILE;
	int done;
	FILE * fp;

	memset( &apn, 0, sizeof( apn ) );
	apn.mcc = mcc;
	apn.mnc = mnc;

	fp = fopen( file_name, "r" );
	if ( NULL == fp )
	{
		LOGEX( "%s: Couldn't open file %s", __func__, file_name );
		return false;
	}

	parser = XML_ParserCreate( NULL );
	if ( NULL == parser )
	{
		LOGEX( "%s: Couldn't allocate memory for parser", __func__ );
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
			LOGEX( "%s: Read error", __func__ );
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
			LOGEX(
				"%s: Parse error at line %d:\n%s",
				__func__,
				XML_GetCurrentLineNumber( parser ),
				XML_ErrorString( XML_GetErrorCode( parser ) ) );
			done = false;
			break;
		}
	} while ( !done && !apn.found );

	fclose( fp );
	XML_ParserFree( parser );

	if ( !done && !apn.found )
	{
		LOGEX( "%s: load xml file %s failed!", __func__, file_name );
	}

	if ( apn.found )
	{
		LOGEX(
			"%s: found apn = %s for mcc = %s, mnc = %s.",
			__func__,
			apn.value,
			apn.mcc,
			apn.mnc );
		strncpy( apnvalue, apn.value, len );
	}
	else
	{
		LOGEX(
			"%s: fail to found apn for mcc = %s, mnc = %s.",
			__func__,
			apn.mcc,
			apn.mnc );
	}

	return apn.found;
}

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
//Mobile network operator
int get_system_mobile_mno(char * ttyName, char * mcc, char * mnc )
{
	int nread;
	char reply[128];
	char cmd[] = "AT+COPS?";
	char cmdSetMode[] = "AT+COPS=0,2";
	char op_name[ 64 ];

	LOGEX( "%s: tty = %s", __func__, ttyName );

	memset(reply,0,sizeof(reply));

	nread = readMobile(ttyName,cmdSetMode,strlen(cmdSetMode),reply,sizeof(reply));
	if(nread < 1)
	{
		LOGEX( "%s: readMobile return %d\n", __func__, nread );
		return -1;
	}

	if(NULL == strstr(reply,"OK"))
	{
		LOGEX( "%s: AT command [AT+COPS=0,2] result not OK, buf = %s", __func__, reply );
		return -1;
	}

	memset(reply,0,sizeof(reply));

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
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
	if ( sscanf(p,"+COPS: %*[0-9],%*[0-9],\"%[0-9]\",%*[0-9]",op_name) != 1 )
	{
		LOGEX( "%s: scan operator name failed! buf = %s", __func__, p );
		return -1;
	}

	LOGEX( "%s: operator code = %s", __func__, op_name );

	memcpy( mcc, op_name, 3 );
	mcc[ 3 ] = 0;

	strcpy( mnc, &op_name[ 3 ] );

	LOGEX( "%s: tty = %s,mcc = %s,mnc = %s", __func__, ttyName , mcc, mnc );
	return 0;
}

//Mobile network operator
int check_cdma(char * ttyName)
{
	int nread;
	char reply[128];
	char cmd[] = "AT+GCAP";
//	char op_name[ 64 ];

	LOGEX( "%s: tty = %s", __func__, ttyName );

	memset(reply,0,sizeof(reply));

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
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
	char *p = strstr(reply,"+GCAP:");
	if(p == NULL)
	{
		LOGEX( "%s: reply buffer format error! buf = %s", __func__, reply );
		return -1;
	}

	p = strstr(reply,"+CGSM");
	if(p == NULL)
	{
		LOGEX( "%s: not foud +CGSM! buf = %s", __func__, reply );
		return 1;
	}
	return 0;
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

int checkProvider ( char *ttyPort, char *command, int netmode,char *usbLabel)
{
	char mcc[ 8 ];
	char mnc[ 8 ];
	char apnname[ 32 ];
	int ret = 0;

	ret = get_system_mobile_mno( ttyPort, mcc, mnc );
	if ( ret != 0 )
	{
		LOGEX( "%s: get_system_mobile_mno failed! return = %d", __func__, ret );
		return ret;
	}

	ret = check_cdma( ttyPort );
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
			if(netmode == 1)
				find = generate_apn_script( apnname );
			else
				find =  generate_apn_netmode_script( apnname , netmode ,usbLabel );
			if ( !find )
				generate_default_script();
		}
		else
		{
			generate_default_script();
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


int system_ping(const char *ifname,PINGURL * urls)
{
	if(!urls->lastPingTime)
		urls->lastPingTime = time(NULL);

	int ret = 0;
	unsigned int i=0;
	for(i=0;i<sizeof(urls->url)/sizeof(urls->url[0]);i++)
	{
		if(strlen(urls->url[i]) < 1)
			continue;
		char cmd[2056];
		sprintf(cmd,"ping %s -c 1 -W 10 -I %s",urls->url[i],ifname);
		printf("%s:%s\n",__func__,cmd);
		ret = system(cmd);
		printf("%s:ret=%d\n",__func__,ret);
		if(!ret)
		{
			urls->lastPingTime = time(NULL);
			break;
		}
		printf("%s:%s failed:%d\n",__func__,cmd,i);
	}
	return !ret;
}



int traffic_monitor(const char *ifName,TRAFFICDETAIL *tdetail)
{
//	printf("%s:mtime:%ld,interval:%d,left=%ld\n",__func__,tdetail.mtime,gprsSilenceMinutes*60,gprsSilenceMinutes*60+tdetail.mtime-time(NULL));
//	TRAFFICDETAIL *tdetail = &conn->tdetail;
	if(!tdetail->trafficMinutes)
		return 1;

	if(!tdetail->mtime)
		tdetail->mtime = time(NULL);
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
			tdetail->mtime = time(NULL);
			tdetail->rxbytes = ifc.r_bytes;
			tdetail->rxbytes = ifc.x_bytes;
			ret = 1;
		}else if(time(NULL) - tdetail->mtime  > tdetail->trafficMinutes*60)
		{
			ret = 0;
		}else{
			printf("%s:%s,left %ld seconds\n",__func__,ifName,tdetail->trafficMinutes*60 + tdetail->mtime - time(NULL) );
		}
	}else{
		ret = 0;
	}
  return ret;
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
		"\'\' \"K\"\n"\
		"\'\' \"+++ATH\"\n"\
		"SAY \"\\nGoodbay\\n\"";

#define CHAT_DIR "/etc/ppp/peers/"
#define CHAT_NAME "default"
#define CHAT_CONNECT "default-chat-connect"
#define CHAT_DISCONNECT "default-chat-disconnect"
int generate_default_script()
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
		write(fd,default_apn_connect,strlen(default_apn_connect));
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

#define NETMODE_FILE	"/home/root/project/"
static int getModeScript(char **modbuff,char *usbLabel)
{
	printf("%s+++\n",__func__);
	char filename[PATH_MAX];
	char buff[1024];
	long filesize = 0;
	int buffOffset = 0;
	sprintf(filename,"%s%s",NETMODE_FILE,usbLabel);

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

	 printf("%s,filelen:%ld\n",__func__,filesize);

	 memset(buff,0,sizeof(buff));
	 while(fgets(buff,sizeof(buff),fp))
	 {
		 printf("%s:buff:%s",__func__,buff);
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
		 strcpy((*modbuff)+buffOffset,"\\n");
		 buffOffset = strlen(*modbuff);
		 printf("%s:*modbuff:%s",__func__,*modbuff);

	 }
	 fclose(fp);

	return 0;
}


int generate_apn_netmode_script(char *apnname , int mode,char *usbLabel)
{

	char *modebuff = NULL;

//	if(mode != 1)
//		getModeScript(&modebuff,usbLabel);


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
		sprintf(buff,apn_netmode_connect2,mode,apnname);
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

