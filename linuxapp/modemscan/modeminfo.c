#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <dirent.h>


struct BaudRate{
	int speed;
	int bitmap;
};
struct BaudRate baudlist[] =
{
//{ 50, B50 },
//{ 75, B75 },
//{ 110, B110 },
//{ 134, B134 },
//{ 150, B150 },
//{ 200, B200 },
//{ 300, B300 },
//{ 600, B600 },
{ 1200, B1200 },
{ 1800, B1800 },
{ 2400, B2400 },
{ 4800, B4800 },
{ 9600, B9600 },
{ 19200, B19200 },
{ 38400, B38400 },
{ 57600, B57600 },
{ 115200, B115200 },
{ 230400, B230400 },
{ 460800, B460800 },
{ 500000, B500000 },
{ 576000, B576000 },
{ 921600, B921600 },
//{ 1000000, B1000000 },
//{ 1152000, B1152000 },
//{ 1500000, B1500000 },
//{ 2000000, B2000000 },
//{ 2500000, B2500000 },
//{ 3000000, B3000000 },
//{ 3500000, B3500000 },
//{ 4000000, B4000000 },
};
int comDatabits[] =
{ 5, 6, 7, 8 };
int comStopbits[] =
{ 1, 2 };
int comParity[] =
{ 'n', 'o', 'e' };


int set_com(int fd,int speed,int databits,int stopbits,int parity)
{
	int i;
	struct termios opt;

	if( tcgetattr(fd ,&opt) != 0)
	{
		perror("get attr failed!\n");
		return -1;
	}

	for (i = 0; i < sizeof(baudlist) / sizeof(baudlist[0]); i++)
	{
		struct BaudRate *rate = &baudlist[i];
		if (speed == rate->speed)
		{
			cfsetispeed(&opt, rate->bitmap);
			cfsetospeed(&opt, rate->bitmap);
			break;
		}
	}
//	//修改控制模式，保证程序不会占用串口
//	opt.c_cflag |= CLOCAL;
//	//修改控制模式，使得能够从串口中读取输入数据
//	opt.c_cflag |= CREAD;

	opt.c_cflag &= ~CSIZE;
	switch (databits)
	{
	case 5:
		opt.c_cflag |= CS5;
		break;
	case 6:
		opt.c_cflag |= CS6;
		break;
	case 7:
		opt.c_cflag |= CS7;
		break;
	case 8:
		opt.c_cflag |= CS8;
		break;
	default:
		printf("Unsupported data size\n");
		return -1;
	}

	switch(parity)
	{
		case 'n':
		case 'N':
			opt.c_cflag &= ~PARENB;
			opt.c_iflag &= ~INPCK;
			break;
		case 'o':
		case 'O':
			opt.c_cflag |= (PARODD|PARENB);
			opt.c_iflag |= INPCK;
			break;
		case 'e':
		case 'E':
			opt.c_cflag |= PARENB;
			opt.c_cflag &= ~PARODD;
			opt.c_iflag |= INPCK;
			break;
		default:
			printf("Unsupported parity\n");
			return -1;
	}

	switch(stopbits)
	{
		case 1:
			opt.c_cflag &= ~CSTOPB;
			break;
		case 2:
			opt.c_cflag |=  CSTOPB;
			break;
		default:
			printf("Unsupported stop bits\n");
			return -1;
	}

	opt.c_iflag &= ~(IXON | IXOFF | IXANY | BRKINT | ICRNL | INPCK | ISTRIP);
	opt.c_lflag &=  ~(ICANON | ECHO | ECHOE | IEXTEN | ISIG);
	opt.c_oflag &= ~OPOST;
	opt.c_cc[VTIME] = 100;
	opt.c_cc[VMIN] = 0;

	tcflush(fd, TCIOFLUSH);
	if (tcsetattr(fd, TCSANOW, &opt) != 0)
	{
		perror("set attr failed!\n");
		return -1;
	}
	return 0;
}

int openDevice(const char* Dev,int speed,int databits,int stopbits,int parity)
{
	int fd;
	fd = open(Dev, O_RDWR | O_NOCTTY);
	if (-1 == fd)
	{
		perror("open failed!\n");
		return -1;
	}

	if (set_com(fd, speed, databits, stopbits, parity) != 0)
	{
		printf("Set Com Error\n");
		return -1;
	}

	return fd;
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

int select_read(int fd, char *buff,int readlen)
{

   struct timeval tm;
    tm.tv_sec = 0;
    tm.tv_usec = 500*1000;
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    int res;
    res = select(fd + 1, &rset, NULL, NULL, &tm);
    if (res < 0)
    {
        //printf("network error in connect[%d]\n",1);
        return -1;
    }
    else if (res == 0)
    {
      //  printf("connect time out[%d]\n", 1);
       return -1;
    }
    else if (1 == res)
    {
        if (FD_ISSET(fd, &rset))
        {
            int n = read(fd, buff, readlen);
            return n;
        }       
    }
    return 0;
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
	fd = open(ttyName, O_RDWR | O_NOCTTY | O_NDELAY );
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
		nread = select_read(fd, res+offset, resLen-offset);
        if(nread == -1)
            break;
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

//Mobile sim card status
int get_modem_sim_state(char * ttyName)
{
	int nread;
	char reply[1024];
	char cmd[] = "AT+CPIN?";

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
//	printf("%s:[%s]%s\n", __func__,ttyName, reply);
	if(  nread == -2){
		return 0;
	}else if(nread < 1){
		return 0;
	}
	if(strstr(reply,"ERROR"))
	{
         printf("\t[sim status:%s]\n","ERROR");
		return 0;
	}
    char *p = strstr(reply,"+CPIN:");
    if(p != NULL)
    {
        char status[32];
        memset(status,0,sizeof(status));
        //+CPIN: READY
        int n = sscanf(p,"+CPIN:%[^\n]",status);
        if(n == 1)
        {
            printf("\t[sim status:%s]\n",status);

        }
    }
	return 1;
}

//Mobile sim card status
int get_modem_version(char * ttyName)
{
	int nread;
	char reply[1024];
	char cmd[] = "ATI";

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
	//printf("%s:[%s]%s\n", __func__,ttyName, reply);
	if(  nread == -2){
		return 0;
	}else if(nread < 1){
		return 0;
	}
	if(strstr(reply,"ERROR"))
	{
         printf("\t[ATI version:%s]\n","ERROR");
		return 0;
	}
    char *p = strstr(reply,"ATI");
    if(p != NULL)
    {
        char status[128];
        memset(status,0,sizeof(status));
        //+CPIN: READY
     //   printf("[%s]",p);
        int n = sscanf(p,"ATI%[^OK]OK",status);
        if(n == 1)
        {   
            p = status;
            while(p!= NULL)
            {
               p = strchr(p,'\n');
               if(p != NULL)
                *p=' ';
            }        

            printf("\t[version:%s]\n",status);

        }
    }
	return 1;
}

//Mobile sim card status
int get_modem_sim_number(char * ttyName)
{
	int nread;
	char reply[1024];
	char cmd[] = "AT+CNUM";

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
//	printf("%s:[%s]%s\n", __func__,ttyName, reply);
	if(  nread == -2){
		return 0;
	}else if(nread < 1){
		return 0;
	}
	if(strstr(reply,"ERROR"))
	{
         printf("\t[sim number:%s]\n","ERROR");
		return 0;
	}
    char *p = strstr(reply,"+CNUM:");
    if(p != NULL)
    {
        char status[32],alpha[32],type[32];
        memset(status,0,sizeof(status));
         memset(alpha,0,sizeof(alpha));
          memset(type,0,sizeof(type));
        //+CPIN: READY
        int n = sscanf(p,"+CNUM:%[^,],%[^,],%[^\n]\n",alpha,status,type);
        if(n == 3)
        {
            printf("\t[sim number:%s,type=%s]\n",status,type);

        }
    }
	return 1;
}

//Mobile sim card status
int get_modem_sim_csq(char * ttyName)
{
	int nread;
	char reply[1024];
	char cmd[] = "AT+CSQ";

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
//	printf("%s:[%s]%s\n", __func__,ttyName, reply);
	if(  nread == -2){
		return 0;
	}else if(nread < 1){
		return 0;
	}
	if(strstr(reply,"ERROR"))
	{
         printf("\t[csq:%s]\n","ERROR");
		return 0;
	}
    char *p = strstr(reply,"+CSQ:");
    if(p != NULL)
    {
        char rssi[32],ber[32];
        memset(rssi,0,sizeof(rssi));
         memset(ber,0,sizeof(ber));
        //+CPIN: READY
        int n = sscanf(p,"+CSQ:%[^,],%[^\n]\n",rssi,ber);
        if(n == 2)
        {
            printf("\t[csq=%s,ber=%s]\n",rssi,ber);

        }
    }
	return 1;
}

//Mobile sim card status
int get_modem_sim_cops(char * ttyName)
{
	int nread;
	char reply[1024];
	char cmd[] = "AT+COPS?";

	nread = readMobile(ttyName,cmd,strlen(cmd),reply,sizeof(reply));
//	printf("%s:[%s]%s\n", __func__,ttyName, reply);
	if(  nread == -2){
		return 0;
	}else if(nread < 1){
		return 0;
	}
	if(strstr(reply,"ERROR"))
	{
         printf("modem csq:%s\n","ERROR");
		return 0;
	}
    char *p = strstr(reply,"+COPS:");
    if(p != NULL)
    {
        char mode[32],format[32],oper[32],act[32];
        memset(mode,0,sizeof(mode));
        memset(format,0,sizeof(format));
        memset(oper,0,sizeof(oper));
        memset(act,0,sizeof(act));
        //+CPIN: READY
        int n = sscanf(p,"+COPS:%[^,],%[^,],%[^,],%[^\n]\n",mode,format,oper,act);
        if(n == 1)
        {
            printf("\t[modem operator: no]\n");
        }else if(n == 4)
        {
             printf("\t[operator: mode=%s,format=%s,oper=%s,act=%s]\n",mode,format,oper,act);
        }
    }
	return 1;
}

int is_modem(char *devname)
{
    int nread;
	char reply[1024];
	char cmd[] = "AT";

	nread = readMobile(devname,cmd,strlen(cmd),reply,sizeof(reply));
	//printf("%s:[%s]%s\n", __func__,devname, reply);
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

int show_modem(char *devname)
{
    int ret =  get_modem_sim_state(devname);
    if(ret != 1)
        return 0;

    get_modem_sim_number(devname);
    get_modem_sim_csq(devname);
     get_modem_sim_cops(devname);
    get_modem_version(devname);
   
    
    return 0;
}