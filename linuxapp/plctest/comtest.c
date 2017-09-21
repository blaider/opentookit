#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <stdint.h>
#include <sys/time.h>


#define TX_DATA							"\x05\x30\x31\x46\x46\x57\x52\x30\x59\x30\x30\x30\x30\x30\x31\x34\x30\x0d\x0a"
#define RX_DATA							""

struct baud_rate
{
	speed_t speed;
	int bitmap;
};

static struct baud_rate baud_list[] =
{
	{ 50, B50 },
	{ 75, B75 },
	{ 110, B110 },
	{ 134, B134 },
	{ 150, B150 },
	{ 200, B200 },
	{ 300, B300 },
	{ 600, B600 },
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
	{ 1000000, B1000000 },
	{ 1152000, B1152000 },
	{ 1500000, B1500000 },
	{ 2000000, B2000000 },
	{ 2500000, B2500000 },
	{ 3000000, B3000000 },
	{ 3500000, B3500000 },
	{ 4000000, B4000000 },
};

static int set_baud_rate(struct termios *opt, int speed)
{
	int ret = -1;
	int i = 0;
	for (i = 0; i < sizeof(baud_list) / sizeof(baud_list[0]); i++)
	{
		struct baud_rate *rate = &baud_list[i];
		if (speed == rate->speed)
		{
			cfsetispeed(opt, rate->bitmap);
			cfsetospeed(opt, rate->bitmap);
			ret = 0;
			break;
		}
	}

	return ret;
}

static int set_databits(struct termios *opt,int databits)
{
	opt->c_cflag &= ~CSIZE;
	switch ( databits )
	{
	case 5:
		opt->c_cflag |= CS5;
		break;
	case 6:
		opt->c_cflag |= CS6;
		break;
	case 7:
		opt->c_cflag |= CS7;
		break;
	case 8:
		opt->c_cflag |= CS8;
		break;
	default:
		printf( "Unsupported data size\n" );
		return -1;
	}

	return 0;
}

static int set_parity(struct termios *opt, int parity)
{
	switch (parity)
	{
	case 'n':
	case 'N':
		opt->c_cflag &= ~PARENB;
		opt->c_iflag &= ~INPCK;
		break;

	case 'o':
	case 'O':
		opt->c_cflag |= (PARODD | PARENB);
		opt->c_iflag |= INPCK;
		break;

	case 'e':
	case 'E':
		opt->c_cflag |= PARENB;
		opt->c_cflag &= ~PARODD;
		opt->c_iflag |= INPCK;
		break;

	default:
		printf("Unsupported parity\n");
		return -1;
	}
	return 0;
}

static int set_stopbits(struct termios *opt, int stopbits)
{
	switch (stopbits)
	{
	case 1:
		opt->c_cflag &= ~CSTOPB;
		break;

	case 2:
		opt->c_cflag |= CSTOPB;
		break;

	default:
		printf("Unsupported stop bits\n");
		return -1;
	}
	return 0;
}

static int set_com(int fd, int speed, int databits, int stopbits, int parity)
{
	struct termios opt;

	if (tcgetattr(fd, &opt) != 0)
	{
		perror("get attr failed!\n");
		return -1;
	}

	if (set_baud_rate(&opt, speed) != 0)
		return -1;

	if (set_databits(&opt, databits) != 0)
		return -1;

	if (set_parity(&opt, parity) != 0)
		return -1;

	if (set_stopbits(&opt, stopbits) != 0)
		return -1;

//	opt.c_iflag &= ~(IXON | IXOFF | IXANY | BRKINT | ICRNL | INPCK | ISTRIP);
//	opt.c_lflag &= ~(ICANON | ECHO | ECHOE | IEXTEN | ISIG);
//	opt.c_oflag &= ~OPOST;
//	opt.c_cc[VTIME] = 5;
//	opt.c_cc[VMIN] = 5;


	opt.c_iflag &= ~(IGNBRK | IGNCR | INLCR | ICRNL | IUCLC |
	        IXANY | IXON | IXOFF | INPCK | ISTRIP);
	opt.c_iflag |= (BRKINT | IGNPAR);
	opt.c_oflag &= ~OPOST;
	opt.c_lflag &= ~(XCASE|ECHONL|NOFLSH);
	opt.c_lflag &= ~(ICANON | ISIG | ECHO);
	opt.c_cflag |= CREAD;
	opt.c_cc[VTIME] = 5;
	opt.c_cc[VMIN] = 1;
	opt.c_iflag &= ~(IXON|IXOFF|IXANY);

	tcflush(fd, TCIOFLUSH);
	if (tcsetattr(fd, TCSANOW, &opt) != 0)
	{
		perror("set attr failed!\n");
		return -1;
	}

	return 0;
}

int open_device(const char* Dev, int speed, int databits, int stopbits,
		int parity)
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
		close(fd);
		return -1;
	}

	return fd;
}

void showHex(char *buf,int len)
{
	int i = 0;
	printf("[%3x]",0);
	for(i=0;i<len;i++)
	{
		printf("%02x ",buf[i]);
		if((i+1)%16 == 0)
			printf("\n[%3x]",(i+1)/16*16);
	}
	printf("\n");
}
uint64_t GetTickCount()

{
    uint64_t data;
    struct timeval tv;
	gettimeofday(&tv, NULL);
	data = tv.tv_sec;
	data *= 1000;
	data += tv.tv_usec/1000;
	return data;
}

uint64_t last_tick = 0;//GetTickCount();
uint64_t rwbytes = 0;
uint64_t tick_count()
{
	uint64_t new_tick = GetTickCount();
	uint64_t diff = new_tick-last_tick;
	printf("dealy %llu ms,%f B/s\n",diff,1000.0*rwbytes/diff);
	last_tick = new_tick;
	rwbytes = 0;
	return 0;
}

static inline setdata(char *buff,int len,char data)
{
	memset(buff,data,len);
	buff[len-2] = '\xd';
	buff[len-1] = '\xa';
	return;
}

int main(int argc, char *argv[])
{
	int fd = -1;
	char *buff;
	char *buffread;
	int nread = 0;
	int data_len = 64;
	uint8_t data_index = 1;
	int brate = 9600;
	int showCount = 0;

	if(argc == 1)
	{
		printf("run as:comtest /dev/ttyAP0 data_len  baudrate\n");
		return -1;
	}

	if(argc >= 3)
		data_len = atoi(argv[2]);

	if(argc >= 4)
		brate = atoi(argv[3]);

	showCount = 8/(2.0*data_len*10/brate);
	if(showCount < 1 )
		showCount = 1;

	printf("Port:%s, len:%d,Bau:%d,ShowCount:%d\n",argv[1],data_len,brate,showCount);

	fd = open_device(argv[1], brate, 8, 1, 'o');
	if (fd < 0)
	{
		printf("open %s failed\n",argv[1]);
		return -1;
	}

	buff = malloc(data_len);
	buffread = malloc(data_len+100);

	setdata(buff,data_len,data_index);
	data_index+=1;
	int rc = 0;
	last_tick = GetTickCount();
	rc = write( fd, buff, data_len );
	if ( rc <= 0 )
	{
		printf( "write port failed!\n" );
		return -1;
	}
	showHex(buff,rc);

	int count = 0;
	while(1)
	{
		memset(buffread,0,data_len+100);
		int left = data_len;
		int findend = 0;
		int j = 0;
		while(1)
		{
			nread = read(fd, buffread+j, left);
			if (nread > 0)
			{
				j +=nread;
				left -=nread;
				rwbytes+=nread;
				int i =0;
				for(i=0;i<nread;i++)
				{
					if(findend == 1 && buffread[j+i-nread] == '\xa')
					{
						findend =2;
						count++;
						if(memcmp(buff,buffread,data_len))
						{
							showHex(buffread,data_len);
							printf("%d,error\n",count);
						}
						setdata(buff,data_len,data_index);
						data_index+=1;
						rc = write( fd, buff, data_len );
						if(rc > 0)
							rwbytes+=rc;
						break;
					}
					else
					if(buff[j+i-nread] == '\xd')
						findend =1;
					else{
						findend = 0;
					}
				}
			}
			if(findend == 2)
			{
				if((count % showCount) == 0)
				{
					printf("%d ",count);
					tick_count();
				}
				break;
			}

		}

	}
	close(fd);
	return 0;
}
