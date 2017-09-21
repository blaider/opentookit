/*************************************************************************
 > File Name: sms_main.c
 > Author: suchao.wang
 > Mail: suchao.wang@advantech.com.cn
 > Created Time: Fri 13 Mar 2015 11:26:21 AM CST
 ************************************************************************/

#include<stdio.h>

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iconv.h>

struct message_info
{
	char cnnu[16];
	char phnu[16];
	char message[128];
};

struct pdu_info
{
	char cnswap[32];
	char phswap[32];
};
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

void swap(char number[], char swap[])
{
	char ch1[] = "86";
	char tmp[16];
	int i;

	memset(swap, 0, 32);
	memset(tmp, 0, 16);
	strcpy(swap, number);
	strcat(swap, "f");
	strcat(ch1, swap);
	strcpy(swap, ch1);

	for (i = 0; i <= strlen(swap) - 1; i += 2)
	{
		tmp[i + 1] = swap[i];
		tmp[i] = swap[i + 1];
	}
	strcpy(swap, tmp);
}

int showErr(int fd)
{
	int nread, nwrite;
	char buff[1024];
	char reply[1024];
	memset(buff, 0, sizeof(buff));
	strcpy(buff, "AT+CMEE=1\r");
	nwrite = write(fd, buff, strlen(buff));
	printf("nwrite=%d,%s\n", nwrite, buff);

	memset(reply, 0, sizeof(reply));
	sleep(1);
	nread = read(fd, reply, sizeof(reply));
	printf("nread=%d,%s\n", nread, reply);
	return 0;
}

int enableMulti(int fd)
{
	int nread, nwrite;
	char buff[1024];
	char reply[1024];
	memset(buff, 0, sizeof(buff));
	strcpy(buff, "AT+CMMS=2\r");
	nwrite = write(fd, buff, strlen(buff));
	printf("nwrite=%d,%s\n", nwrite, buff);

	memset(reply, 0, sizeof(reply));
	sleep(1);
	nread = read(fd, reply, sizeof(reply));
	printf("nread=%d,%s\n", nread, reply);
	return 0;
}
int disableMulti(int fd)
{
	int nread, nwrite;
	char buff[1024];
	char reply[1024];
	memset(buff, 0, sizeof(buff));
	strcpy(buff, "AT+CMMS=1\r");
	nwrite = write(fd, buff, strlen(buff));
	printf("nwrite=%d,%s\n", nwrite, buff);

	memset(reply, 0, sizeof(reply));
	sleep(1);
	nread = read(fd, reply, sizeof(reply));
	printf("nread=%d,%s\n", nread, reply);
	return 0;
}

int send(int fd, char *cmgf, char *cmgs, char *message)
{
	int nread, nwrite;
	char buff[1024];
	char reply[1024];

//	memset(buff, 0, sizeof(buff));
//	strcpy(buff, "at\r");
//	nwrite = write(fd, buff, strlen(buff));
//	printf("nwrite=%d,%s\n", nwrite, buff);
//
//	memset(reply, 0, sizeof(reply));
//	sleep(1);
//	nread = read(fd, reply, sizeof(reply));
//	printf("nread=%d,%s\n", nread, reply);
//
//	memset(buff, 0, sizeof(buff));
//	strcpy(buff, "AT+CSQ\r");
//	nwrite = write(fd, buff, strlen(buff));
//	printf("nwrite=%d,%s\n", nwrite, buff);
//
//	memset(reply, 0, sizeof(reply));
//	sleep(1);
//	nread = read(fd, reply, sizeof(reply));
//	int rssi=0,equ=0;
//	char *pos=(reply,"+CSQ: ");
//	sscanf(pos+6,"%d,%d",&rssi,&equ);
//	printf("rssi:%d,equ:%d\n");
//	printf("nread=%d,%s\n", nread, reply);


	memset(buff, 0, sizeof(buff));
	strcpy(buff, "AT+CMGF=");
	strcat(buff, cmgf);
	strcat(buff, "\r");
	nwrite = write(fd, buff, strlen(buff));
	printf("nwrite=%d,%s\n", nwrite, buff);

	memset(reply, 0, sizeof(reply));
	sleep(1);
	nread = read(fd, reply, sizeof(reply));
	printf("nread=%d,%s\n", nread, reply);

	memset(buff, 0, sizeof(buff));
	strcpy(buff, "AT+CMGS=");
	strcat(buff, cmgs);
	strcat(buff, "\r");
	nwrite = write(fd, buff, strlen(buff));
	printf("nwrite=%d,%s\n", nwrite, buff);

	memset(reply, 0, sizeof(reply));
	sleep(1);
	nread = read(fd, reply, sizeof(reply));
	printf("nread=%d,%s\n", nread, reply);

	memset(buff, 0, sizeof(buff));
	strcpy(buff, message);
	nwrite = write(fd, buff, strlen(buff));
	printf("nwrite=%d,%s\n", nwrite, buff);

	memset(reply, 0, sizeof(reply));
	sleep(1*(nwrite/150));
	nread = read(fd, reply, sizeof(reply));
	printf("nread=%d,%s\n", nread, reply);
}

int send_en_message(int fd, struct message_info info)
{
	getchar();
	char cmgf[] = "1";
	int conter = 0;
	char cmgs[16] =
	{ '\0' };
	info.phnu[0] ='\"';
	printf("enter recever phnumber :\n");
	gets(info.phnu+1);
	while (strlen(info.phnu) != 12)
	{
		if (conter >= 3)
		{
			printf("conter out !\n");
			return -1;
		}
		printf("number shuld be --11-- bits ! enter agin :\n");
		gets(info.phnu+1);
		conter++;
	}
	info.phnu[12] ='\"';

	printf("enter you message !\n");
	gets(info.message);
	strcat(info.message, "\x1a");
	strcat(cmgs, info.phnu);

	send(fd, cmgf, cmgs, info.message);
}


char iconv_dst_buf[ 1024 ];
char sms[ 1024 ];

int convtest(char *content)
{

	char * iconv_dst = NULL;
	iconv_t iconv_fd;
	ssize_t iconv_rc = 0;
	size_t iconv_src_len = 0;
	size_t iconv_dst_len = 0;

//	char *content = "123456789【优步】就在今天，优步青岛全城2.9折！在APP优惠一栏输入：为爱而烧，即刻获得2.9折优惠，周五出行从此无压力！微信关注公众号qd_you";
	memset( iconv_dst_buf, 0, sizeof( iconv_dst_buf ) );
	iconv_dst = iconv_dst_buf;
	iconv_src_len = strlen( content );
	iconv_dst_len = sizeof( iconv_dst_buf );
	iconv_fd = iconv_open( "UCS-2", "UTF-8" );

	printf("%d,%d,%d\n",iconv_rc,iconv_src_len,iconv_dst_len);
	iconv_rc = iconv(
			iconv_fd,
			&content,
			&iconv_src_len,
			&iconv_dst,
			&iconv_dst_len );
		if ( -1 == iconv_rc )
		{
			return 0;
		}
		printf("%d,%d,%d\n",iconv_rc,iconv_src_len,iconv_dst_len);
		char dst_keep_buf[10];
		int back_len = 0;
		int i = 0;
		int len = 0;
	for ( i = 1024; i >= 0; i-- )
			{
				if ( iconv_dst_buf[ i ] == 0 )
					back_len++;
				else
					break;
			}

			len = 1024 - back_len + 1;

			sprintf(sms,"%02x",len);
	for ( i = 0; i < len; i++ )
	{
		sprintf( dst_keep_buf, "%02x", iconv_dst_buf[ i ] );
		strcat( sms, dst_keep_buf );
	}
	iconv_close( iconv_fd );
	return 0;
}
int showPdu(char *content,int len)
{

	char * iconv_dst = NULL;
	iconv_t iconv_fd;
	ssize_t iconv_rc = 0;
	size_t iconv_src_len = 0;
	size_t iconv_dst_len = 0;
	unsigned char code[1024];
	char iconv_dst_buf[1024];

	memset( iconv_dst_buf, 0, sizeof( iconv_dst_buf ) );
	iconv_dst = iconv_dst_buf;
	iconv_src_len = len/2;
	iconv_dst_len = sizeof( iconv_dst_buf );
	iconv_fd = iconv_open(  "UTF-8","UCS-2BE");

	int i=0;
	memset(code,0,sizeof(code));

	for(i=0;i<iconv_src_len;i++)
	{
		char tmp[4];
		memset(tmp,0,sizeof(tmp));
		memcpy(tmp,content+i*2,2);
		code[i] = strtoul(tmp,NULL,16)&0xff;
	}
	char *p = code;

	iconv_rc = iconv(
			iconv_fd,
			&p,
			&iconv_src_len,
			&iconv_dst,
			&iconv_dst_len );
		if ( -1 == iconv_rc )
		{
			return 0;
		}
	printf("%s\n",iconv_dst_buf);
	iconv_close( iconv_fd );
	return 0;
}
int send_zh_message(int fd, struct message_info info)
{
	char cmgf[] = "0";
	char cmgs[4] =	{ '\0' };
	char ch2[] = "0891";
	char ch3[] = "1100";
	char ch3Multi[] = "5100";
	char ch4[] = "000800";
	char ch5[] = "0d91";
	char final[1024];
	char *message[4] =
	{ "0a5BB691CC7740706BFF01", "0a5BB691CC67098D3CFF01",
			"1a676866539E4FFF0C4F605988558A4F6056DE5BB65403996DFF01" ,""};
	struct pdu_info pdu;
	int conter = 0, flag, len;
	getchar();
	memset(final, 0, sizeof(final));

//	printf("cmnet:13800100500\n");
//	printf("unicom:13010112500\n");
//
//	printf("enter your centre phnumber :\n");
//	gets(info.cnnu);
//	while (strlen(info.cnnu) != 11)
//	{
//		if (conter >= 3)
//		{
//			printf("conter out !\n");
//			return -1;
//		}
//		printf("number shuld be --11-- bits ! enter agin :\n");
//		gets(info.cnnu);
//		conter++;
//	}

	printf("enter your recever phnumber :\n");
	gets(info.phnu);
	while (strlen(info.phnu) != 11)
	{
		if (conter >= 3)
		{
			printf("conter out !\n");
			return -1;
		}
		printf("number shuld be --11-- bits ! enter agin :\n");
		gets(info.phnu);
		conter++;
	}
	printf("choice message :\n");
	printf("1.fire.\n");
	printf("2.thief.\n");
	printf("3.mother@home.\n");
	scanf("%d", &flag);
	swap(info.phnu, pdu.phswap);
	swap(info.cnnu, pdu.cnswap);

	if(flag == 4)
	{
//		convtest();
		message[3] = sms;
	}

//	strcpy(final, ch2);
//	strcat(final, pdu.cnswap);
	char *mes = message[flag - 1];
	len = strlen(mes);
	printf("len:%d\n",len);
	if(len > 282)
	{
		int left = len -2;
		int order = 1;
		int max = 0;
		int offset = 2;
		while(left > 0)
		{
			max++;
			left = left-268;
		}
		left = len -2;
		unsigned char uniq =(unsigned char) (1+(int)(256.0*rand()/(RAND_MAX+1.0)));
//		printf("left:%d,max:%d,%d\n",left,max,uniq);
		enableMulti(fd);
		for(order=1;order < max+1;order++)
		{
			memset(final,0,sizeof(final));
			int submit = 0;
			if(left > 268)
			{
				submit = 268;
				left = left - 268;
			}else
			{
				submit = left;
				left = 0;
			}


			char lenBuf[128];
			sprintf(lenBuf,"%02x%02x%02x%02x%02x%02x%02x",submit/2 + 6,5,0,3,uniq,max,order);
			strcpy(final, "00");
			strcat(final, ch3Multi);
			strcat(final, ch5);
			strcat(final, pdu.phswap);
			strcat(final, ch4);
			strcat(final, lenBuf);
			int l = strlen(final);
//			printf("l:%d,offset:%d,submit:%d\n",l,offset,submit);
//			puts(final);
//			puts(mes);
//			puts(mes+offset);
			showPdu(mes+offset,submit);
			memcpy((char *)(final+l), mes+offset,submit);
			strcat(final, "\x1a");

			len = strlen(ch3) + strlen(ch4) + strlen(ch5) + strlen(pdu.phswap) + strlen(lenBuf)
					+ submit;
			printf("len:%d\n",len);
			puts(final);
			sprintf(cmgs, "%d", len / 2);
			send(fd, cmgf, cmgs, final);
			offset +=submit;
		}
		disableMulti(fd);
	}
	else
	{
		strcpy(final, "00");
		strcat(final, ch3);
		strcat(final, ch5);
		strcat(final, pdu.phswap);
		strcat(final, ch4);
		strcat(final, message[flag - 1]);
		strcat(final, "\x1a");

		len = strlen(ch3) + strlen(ch4) + strlen(ch5) + strlen(pdu.phswap)
				+ strlen(message[flag - 1]);
		puts(final);
		sprintf(cmgs, "%d", len / 2);
		showPdu(message[flag - 1]+2,strlen(message[flag - 1]+2));
		send(fd, cmgf, cmgs, final);
	}
}


unsigned long convert(const char *cp,int len)
{
	unsigned long result = 0;
	int base = 16;


	while (isxdigit(*cp) && len--) {
		unsigned int value;

		value = isdigit(*cp) ? *cp - '0' : tolower(*cp) - 'a' + 10;
		if (value >= base)
			break;
		result = result * base + value;
		cp++;
	}
	return result;
}

test()
{

	char str[] = "ab";
	printf("%02x\n",convert(str,2));
}

int hex2bin(char *dst,int *len,char *src,int lenIn)
{
	int i=0;
	int header = 0;
	*len = 0;
	if(lenIn%2)
	{
		dst[i++]=convert(src,1);
		lenIn--;
		header = 1;
	}
	for(;i<lenIn/2;i++)
	{
		dst[i++]=convert(src+i*2 -header,1);
	}
	printf("pas:%d\n",i-1);
	return i-1;
}
parsePDU(char *data)
{
	char *p = data;
	int len = convert(p,2);
	printf("[%02x]smsclen:%d\n",p-data,len);
	p = p + len*2 + 2;
	int first_oc = convert(p,2);
	printf("[%02x]first oc:%02x\n",p-data,first_oc);
	p = p +2;
	int oa_len = convert(p,2);
	printf("[%02x]oa_len:%02x\n",p-data,oa_len);
	p = p + oa_len + 5;
	int pid = convert(p,2);
	printf("[%02x]pid:%02x\n",p-data,pid);
	p = p +2;
	int dcs = convert(p,2);
	printf("[%02x]dcs:%02x\n",p-data,dcs);

	p = p + 2*7 + 2;
	int udl = convert(p,2);
	printf("[%02x]udl:%02x\n",p-data,udl);
	printf("len,%d\n",strlen(p+2));
	char dst[1024];
	int i=0;
//	hex2bin(dst,&i,p+2,udl*2);
	showPdu(p+2,udl*2);
	return;
}

parse(char *data,int len)
{
	char *p = data;
	char *pe = data;
	char str[1024];
	while(pe - data < len-1)
	{
		memset(str,0,sizeof(str));
		p=strstr(pe,"+CMGL:");
		if(NULL == p)
			return;
		pe = strstr(p,"\r");
		memcpy(str,p,pe-p);
		printf("%s\n",str);
		p = pe +1;
		pe = strstr(p,"\r");
		memcpy(str,p+1,pe-p);
		printf("PUD info:%s\n",str);
		parsePDU(str);

	}
}

int showMessage(int fd, struct message_info info)
{

	int nread, nwrite;
	char buff[1024];
	char reply[1024];
	memset(buff, 0, sizeof(buff));
	strcpy(buff, "AT+CMGL\r");
	nwrite = write(fd, buff, strlen(buff));
	printf("nwrite=%d,%s\n", nwrite, buff);

	memset(reply, 0, sizeof(reply));
	sleep(1);
	nread = read(fd, reply, sizeof(reply));
	printf("nread=%d,%s\n", nread, reply);
	parse(reply,nread);
	return 0;
}

int main()
{
	int fd;
	char choice;
	struct message_info info;
	fd = open("/dev/ttyUSB2", O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd)
	{
		perror("Can't Open Serial Port");
	}
	serial_init(fd);
	printf("\n============================================\n");
	printf("\tthis is a gprs test program !\n");
	printf("============================================\n");
	printf("enter your selete :\n");
	printf("1.send english message.\n");
	printf("2.send chinese message.\n");
	printf("3.list message.\n");
	printf("4.exit.\n");
	choice = getchar();
	switch (choice)
	{
	case '1':
		send_en_message(fd, info);
		break;
	case '2':
		send_zh_message(fd, info);
		break;
	case '3':
		showMessage(fd,info);
		break;
	case '0':
		test();
		break;
	default:
		break;
	}
	close(fd);
	return 0;

}
