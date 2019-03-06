#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sched.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <string.h>
#include <semaphore.h>

//#include <unistd.h>

//------------------------------------------------------------------------------
#define ARRAY_SIZE(a) 	(sizeof(a) / sizeof((a)[0]))

static const char *device = "/dev/spidev1.0";
static const char *device_slave = "/dev/spidev2.0";
static uint8_t mode = 1;
static uint8_t bits = 8;
static uint32_t speed = 8000000; //12000000;
static uint32_t speed_slave = 9000000;//9000000; //12000000;
static uint16_t delay = 500;

static int gfd, gfd_slave;

static int recive_data = 1;

sem_t sem_tick;

sem_t sem_cmd;

unsigned long last_tick = 0;

static shuld_exit = 0;

static  unsigned char seq = 0;

#define BUFFER_LEN		256

#include <time.h>
#include <sys/time.h>


// 返回自系统开机以来的毫秒数（tick）
unsigned long GetTickCount()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (ts.tv_sec);
}

static void spi_send(int fd, unsigned char* pbuf, int buf_len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)pbuf,
		.rx_buf = 0,
		.len = buf_len,
		.delay_usecs = 0, //delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		perror("can't send spi message");

	return;
}

static int find_header(char *package,int len)
{
   int header_pos = 0;
   for(header_pos = 0; header_pos < len - 4; header_pos++)
   {
      if(package[header_pos] == 'A' &&             //'A'
         package[header_pos+1] == 'D' &&           //'D'
         package[header_pos+2] == 'A' &&
         package[header_pos+3] == 'M')
      {
         return header_pos;
      }
   }
   return -1; //there is no header string
}

static int spi_rcv(int fd, unsigned char* pbuf, int *buf_len)
{
	int ret;
	*buf_len = 0;
	uint8_t rx_slave[BUFFER_LEN] = {0};
	struct spi_ioc_transfer tr_slave = {
		.tx_buf = 0,
		.rx_buf = (unsigned long)rx_slave,
		.len = ARRAY_SIZE(rx_slave),
		.delay_usecs = 0, //delay,
		.speed_hz = speed_slave,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr_slave);
	if (ret < 1)
		perror("can't send spi message");
	else{
		int pos = find_header(rx_slave,sizeof(rx_slave));
		if(pos < 0)
			return 0;
		int rx_len = rx_slave[pos + 7];
		*buf_len = rx_len;
		memcpy(pbuf,&rx_slave[pos],rx_len);
	}

	return *buf_len;
}

void init_spidev(int fd)
{
	int ret = 0;
	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		perror("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		perror("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		perror("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		perror("can't get bits per word");

	/*
	 * max speed hz
	 */

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		perror("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		perror("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
}

unsigned char ConstructSPIPackage(unsigned char seq_id,unsigned char module_id, unsigned char direction, unsigned char command_id
                            , unsigned char* pbufsrc, unsigned char buflen, unsigned char* pbufdst)
{
    unsigned char header_flag[7] = {0x41, 0x44, 0x41, 0x4d, 0x01, 0x00, 0x01};
    unsigned char package_len = 0x10 + buflen;
    unsigned short header_checksum = 0x0;
    unsigned char module_datalen = 0x6 + buflen;
    unsigned short module_checksum = 0x0;
    int i = 0;
    unsigned short checksum = 0;


    memcpy(&pbufdst[0], &header_flag[0], 7);
    pbufdst[4] = seq_id;
    pbufdst[7] = package_len;
    for (i = 0; i < 8; ++i)
    {
        checksum += pbufdst[i];
    }
    pbufdst[8] = checksum & 0xff;
    pbufdst[9] = (checksum >> 8) & 0xff;
    pbufdst[10] = module_id;
    pbufdst[11] = module_datalen;
    pbufdst[12] = direction;
    pbufdst[13] = command_id;
    memcpy(&pbufdst[14], &pbufsrc[0], buflen);
    checksum = 0;
    for (i = 0; i < module_datalen - 2; ++i)
    {
        checksum += pbufdst[10 + i];
    }

    pbufdst[10 + module_datalen - 2] = checksum & 0xff;
    pbufdst[10 + module_datalen - 2 + 1] = (checksum >> 8) & 0xff;

    return package_len;
}

int SendSpiCmdInFile()
{
    int ret = 0;
    unsigned char spi_cmd_buf[256] = {0};
    unsigned char spi_cmd_len = 0;


    do
    {
    	int i = 0;

        //41 44 41 4d 01 00 01 12 27 01* 00 08 20 40 00 07 6f 00
        //41 44 41 4d 01 00 01 13 28 01* 00 09 20 25 00 07 00 55 00
        unsigned char cmd_id = 0x40;
        char cmd_data[] = {0x00,0x07};
        unsigned char dir = 0x20;
        unsigned char module = 0;
        while (1)
        {
        	int wlen=ConstructSPIPackage(++seq,module,dir,cmd_id,cmd_data,sizeof(cmd_data),spi_cmd_buf);

        	int rlen = 0;
        	sem_wait(&sem_tick);
        	last_tick = GetTickCount();
        	sem_post(&sem_tick);
        	sem_wait(&sem_cmd);
            spi_send(gfd, spi_cmd_buf, wlen);


//            printf("[S][%02x]\n",seq);
//            if(seq != spi_cmd_buf[4] || (wlen > 0 && wlen != 34))
//            {
//				for (i = 0; i < wlen; ++i)
//				{
//					printf("%02x ", spi_cmd_buf[i]);
//				}
//				printf("\n");
//            }

//            sleep(1);
            usleep(5*1000);
        }

    } while (0);

    return ret;

}

void *rx_tnd(void *arg)
{
	int i = 0;
	 unsigned char spi_cmd_buf_rcv[256] = {0};
	int spi_cmd_len_rcv = 0;
	int rlen = 0;
	while(1)
	{
		int diff_tick = 0;
		memset(spi_cmd_buf_rcv,0,sizeof(spi_cmd_buf_rcv));
		rlen = spi_rcv(gfd_slave, spi_cmd_buf_rcv, &spi_cmd_len_rcv);
		sem_wait(&sem_tick);
		diff_tick = GetTickCount() - last_tick;
		sem_post(&sem_tick);

//		 printf("[R][%02x][%d]\n",seq,rlen);
		 if(seq != spi_cmd_buf_rcv[4] || (rlen > 0 && rlen !=34))
		 {
			 for (i = 0; i < rlen; ++i)
			{
				printf("%02x ", spi_cmd_buf_rcv[i]);
			}
			printf("\n**********************\n");
			printf("spend tick:%ld,read = %d[0x%02x],seq=0x%02x\n",diff_tick,rlen,rlen,seq);
		 }
		 sem_post(&sem_cmd);
		if(shuld_exit)
		{
			 sem_post(&sem_cmd);
			break;
		}
	}
	return;
}


void sigroutine(int dunno)
{
	if(shuld_exit == 0)
	{
		shuld_exit = 1;
	}else{
		exit(0);
	}

}

int main(int argc, char *argv[])
{
	int ret = 0;
	signal(SIGINT, sigroutine);

	sem_init(&sem_tick,1,1);
	sem_init(&sem_cmd,1,1);

	gfd = open(device, O_RDWR);
	if (gfd < 0)
	{
		perror("can't open device");
		return -1;
	}
	init_spidev(gfd);
	gfd_slave = open(device_slave, O_RDWR);
	if (gfd_slave < 0)
	{
		perror("can't open device");
		return -1;
	}
	init_spidev(gfd_slave);

	pthread_t mh;
	pthread_create(&mh,NULL,rx_tnd,NULL);

	sleep(2);

	SendSpiCmdInFile();

	close(gfd);
	close(gfd_slave);
	return ret;
}
