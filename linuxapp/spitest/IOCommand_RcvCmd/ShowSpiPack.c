/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

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

//------------------------------------------------------------------------------
#define NSEC_PER_SEC    1000000000
#define MAX_SAFE_STACK (8*1024) /* The maximum stack size which is
                                   guaranteed safe to access without
                                   faulting */

static char gShow_Mdl_Idx = 11;

void WriteResultToLogfile(char* pfilebuf, int mdlidx);
                                   
/* using clock_nanosleep of librt */
extern int clock_nanosleep(clockid_t __clock_id, int __flags,
      __const struct timespec *__req,
      struct timespec *__rem);

/* the struct timespec consists of nanoseconds
 * and seconds. if the nanoseconds are getting
 * bigger than 1000000000 (= 1 second) the
 * variable containing seconds has to be
 * incremented and the nanoseconds decremented
 * by 1000000000.
 */
static inline void tsnorm(struct timespec *ts)
{
    while (ts->tv_nsec >= NSEC_PER_SEC) {
        ts->tv_nsec -= NSEC_PER_SEC;
        ts->tv_sec++;
    }
}

//------------------------------------------------------------------------------
#define ARRAY_SIZE(a) 	(sizeof(a) / sizeof((a)[0]))

static const char *device = "/dev/spidev1.0";
static const char *device_slave = "/dev/spidev2.0";
static uint8_t mode = 1;
static uint8_t bits = 8;
static uint32_t speed = 8000000; //12000000;
static uint32_t speed_slave = 9000000; //12000000;
static uint16_t delay = 500;
static uint16_t timedelay = 4; //4*10us

static int gfd, gfd_slave;

static int gShowRcvHexFlag = 0;
/*static int gShowRawDataFlg = 0;*/

//四个模块，每个模块一组数据
static float gfAIValue[4][4] = {0};
static float gfTemperature[4][2] = {0};
//第一个模块的索引值为11
#define MODULE_INDEX_BASE   11

#define BUFFER_LEN		256

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static uint8_t tx_search_msg[] = {
	0x41, 0x44, 0x41, 0x4d, //-->ADAM
	0x01, 0x00,	0xfe, 0xff,
	0x01, 0x00,	0x2a, 0x00,
	0x3c, 0x03,	0x01, 0x1c,
	0x20, 0x41,	0x00, 0x07,
	0x20, 0x40,	0x00, 0x07,
	0x20, 0x21, 0x00, 0x03,		
	0x20, 0x20,	0x00, 0x03,
	0x20, 0x11,	0x00, 0x07,
	0x20, 0x10,	0x00, 0x07,
	0xe2, 0x01
};

static uint8_t rx_search_msg[] = {
	0x41, 0x44, 0x41, 0x4D, 0x01, 0x00, 0xFE, 0xFF, 
	0x01, 0x00, 0x49, 0x00, 0x5B, 0x03, 0x01, 0x3B, 
	0x60, 0x41, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x60, 0x40, 0x00, 0x07, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x60, 0x21, 0x00, 0x03, 0x00, 0x60, 0x20, 0x00, 
	0x03, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x11, 0x00, 
	0x07, 0x01, 0xE0, 0x10, 0x00, 0x07, 0x01, 0x83, 
	0x04
};

/*static void spi_send(int fd)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx_search_msg,
		.rx_buf = 0, 
		.len = ARRAY_SIZE(tx_search_msg),
		.delay_usecs = 0, //delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	return;
}*/

void *rx_thread(void *arg)
{
    int i;
	int ret;
    int fd_slave;
	//long count = 0;
	//int cnt = ARRAY_SIZE(rx_search_msg); // search return len
    int write_file_cnt = 0;
	int err_cnt = 0;
    char pfilebuf[100] = {0};
    double fvol1 = 0, fvol2 = 0, fvol3 = 0, fvol4 = 0, fsnr1 = 0, fsnr2 = 0, 
        ftmp1 = 0, ftmp2 = 0, ftmp3 = 0, ftmp4 = 0;
	
    fd_slave = *((int *)arg);
    
	uint8_t rx_slave[BUFFER_LEN] = {0};
    //uint8_t
	uint8_t rx_msg[94] = {0};
    uint16_t rx_len = 0;

	speed_slave = 9000000;
	
	struct spi_ioc_transfer tr_slave = {
		.tx_buf = 0,
		.rx_buf = (unsigned long)rx_slave,
		.len = ARRAY_SIZE(rx_slave),
		.delay_usecs = 0, //delay,
		.speed_hz = speed_slave,
		.bits_per_word = bits,
	};

	printf("rx thread!\n");
	int j = 0, msg_size = 0;
    for (;;)
	{
		ret = ioctl(fd_slave, SPI_IOC_MESSAGE(1), &tr_slave);
		if (ret < 1)
			pabort("can't recv slave spi message\n");

		printf("Get a spi package\r\n");

// 		memcpy(&rx_msg[0], &rx_slave[14], 93);
// 		rx_msg[93] = '\0';

//         sscanf(rx_msg, "%7.2lf\t%7.2lf\t%7.2lf\t%7.2lf\t%l7.2f\t%7.2lf\t%10.6lf\t%10.6lf\t%10.6lf\t%10.6lf\r\n", &ftmp1, &ftmp2, &ftmp3, &ftmp4, 
//             &fsnr1, &fsnr2, &fvol1, &fvol2, &fvol3, &fvol4);
//        WriteResultToLogfile(rx_msg, rx_slave[10]);

        if (gShowRcvHexFlag == 1)
        {
            rx_len = rx_slave[7];
            for (i = 0; i < rx_len; ++i)
            {
                if ((i % 16 == 0) && (i != 0))
                {
                    printf("\n");
                }
                printf("%02x ", rx_slave[i]);                
            }
            printf("\n");
        }

        if(gShow_Mdl_Idx == rx_slave[10] && rx_slave[13] == 0x51)
        {
            msg_size = rx_slave[11] - 6;
            memcpy(&rx_msg[0], &rx_slave[14], msg_size);
		    rx_msg[msg_size] = '\0';
            printf("%s", rx_msg);
            msg_size = 0;
            //printf("%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%7.2f\t%10.6f\t%10.6f\t%10.6f\t%10.6f\r\n\r\n", ftmp1, ftmp2, ftmp3, ftmp4, 
            //fsnr1, fsnr2, fvol1, fvol2, fvol3, fvol4);
        }

//         if (gShowRawDataFlg == 1)
//         {
//             for (i = 0; i < rx_slave[7]; ++i)
//             {
//                 if ((i % 16) == 0)
//                 {
//                     printf("\n");
//                 }
//                 printf("02x ", rx_slave[i]);
//             }
//             printf("\n");
//         }
        
        /*if (rx_msg[0] == 'C')
        {
            if (rx_msg[1] == 'h')
            {
                char value[12] = {0};
                char* pvalue_head = value;
                memcpy(value, &rx_msg[4], 11);
                value[11] = '\0';
                while (*pvalue_head == ' ')
                {
                    pvalue_head++;
                }
                sscanf(pvalue_head, "%f", &gfAIValue[rx_slave[10] - MODULE_INDEX_BASE][(unsigned char)(rx_msg[2] - '0')]);
                //printf("%10.7lf\r\n", gfAIValue[(unsigned char)(rx_msg[2] - '0')]);
                //printf("%s\r\n", pvalue_head);
            }
            else
            {
                char value[12] = {0};
                char* pvalue_head = value;
                char pfilebuf[100] = {0};
                memcpy(value, &rx_msg[4], 11);
                value[11] = '\0';
                while (*pvalue_head == ' ')
                {
                    pvalue_head++;
                }
                sscanf(pvalue_head, "%f", &gfAIValue[rx_slave[10] - MODULE_INDEX_BASE][3]);
                //printf("%10.7lf\r\n", gfAIValue[3]);
                //printf("%s\r\n", pvalue_head);
            }
        }
        else if(rx_msg[0] == 's')
        {
            char value[12] = {0};
            char* pvalue_head = value;
            memcpy(value, &rx_msg[8], 11);
            value[11] = '\0';
            while (*pvalue_head == ' ')
            {
                pvalue_head++;
            }
            sscanf(pvalue_head, "%f", &gfTemperature[rx_slave[10] - MODULE_INDEX_BASE][(unsigned char)(rx_msg[6] - '1')]);
            //printf("%5.3lf\r\n", gfTemperature[(unsigned char)(rx_msg[6] - '1')]);
            //printf("%s\r\n", pvalue_head);

            static unsigned int sRecordCount11 = 0;
            static unsigned int sRecordCount12 = 0; 
            static unsigned int sRecordCount13 = 0; 
            static unsigned int sRecordCount14 = 0; 
            if (2 == (rx_msg[6] - '1' + 1))
            {
                int *pcount = 0;
                switch (rx_slave[10])
                {
                case 11:
                    sRecordCount11++;
                    pcount = &sRecordCount11;
                    break;
                case 12:
                    sRecordCount12++;
                    pcount = &sRecordCount12;
                    break;
                case 13:
                    sRecordCount13++;
                    pcount = &sRecordCount13;
                    break;
                case 14:
                    sRecordCount14++;
                    pcount = &sRecordCount14;
                    break;
                }
                //if ((*pcount % 15) == 0)
                {
                    *pcount = 0;
                }
                memset(pfilebuf, 0, 100);
                sprintf(pfilebuf, "%10.6f\t%10.6f\t%10.6f\t%10.6f\t%6.3f\t%6.3f\r\n", gfAIValue[rx_slave[10] - MODULE_INDEX_BASE][0], gfAIValue[rx_slave[10] - MODULE_INDEX_BASE][1], 
                        gfAIValue[rx_slave[10] - MODULE_INDEX_BASE][2], gfAIValue[rx_slave[10] - MODULE_INDEX_BASE][3], 
                        gfTemperature[rx_slave[10] - MODULE_INDEX_BASE][0], gfTemperature[rx_slave[10] - MODULE_INDEX_BASE][1]);

                if (0 == *pcount)
                {
                    //printf("write %dth data!\r\n", write_file_cnt);
                    write_file_cnt++;
                    WriteResultToLogfile(pfilebuf, rx_slave[10]);
//                     if (write_file_cnt == 75)
//                     {
//                         exit(0);
//                     }
                }           
            }

        }*/
        

		for (i = 0; i < BUFFER_LEN; i++) {
			rx_slave[i] = 0;
		}
		//count++;
	}
	
	printf("rx slave spi message end!\n");
}

void init_spidev(int fd)
{
	int ret = 0;
	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	speed = 8000000;
		
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
}

int main(int argc, char *argv[])
{
	int ret;
	int /*fd, */fd_slave;
    int interval;

    char* poption = NULL;
    char* pshowtype = NULL;
    
    struct timespec t;
    struct sched_param param;
	pthread_t rx_id;
	
	/*fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");
	gfd = fd;
	init_spidev(fd);*/

    if (argc > 1 )
    {
        gShow_Mdl_Idx = atoi(argv[1]);
        printf("show module index: %02d\r\n", gShow_Mdl_Idx);
    }
    if (argc > 2)
    {
        poption = argv[2];
        if (poption[1] == 'a')
        {
            printf("show raw\n");
            gShowRcvHexFlag = 1;
        }
    }

//     if (argc > 3)
//     {
//         pshowtype = argv[3];
//         if (0 == strcmp(pshowtype, "-raw"))
//         {
//             printf("show spi raw data\r\n");
//             gShowRawDataFlg = 1;
//         }
// 
//     }
    

	fd_slave = open(device_slave, O_RDWR);
	if (fd_slave < 0)
		pabort("can't open slave device");
	gfd_slave = fd_slave;
	init_spidev(fd_slave);

	/*param.sched_priority = 50;
    interval = 500000; //500000; // 500us
   	if (argc >= 2 && atoi(argv[1]) > 0)
   	{
      	param.sched_priority = atoi(argv[1]);
   		printf("priority: %d\n",atoi(argv[1]));
   	}
   	if (argc >= 3)
      	interval=atoi(argv[2]);
   	printf("interval: %dus\n",interval);*/
      
    ret = pthread_create(&rx_id, NULL, (void*)rx_thread, &fd_slave);
    if (ret)
    {	
       	printf("Create rx pthread error!\n");
       	return 1;
    }	

    // enable realtime fifo scheduling
    /*if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        perror("sched_setscheduler failed");
        exit(-1);
    }

    // get current time 
    clock_gettime(0,&t);
    // start after one second 
    t.tv_sec++;*/
    while(1){
	/*
        // wait untill next shot 
        clock_nanosleep(0, TIMER_ABSTIME, &t, NULL);
        // do the stuff 
        spi_send(fd);
        // calculate next shot 
        t.tv_nsec+=interval;
        tsnorm(&t);*/
    }
    
    pthread_join(rx_id, NULL);

	//close(fd);
	close(fd_slave);

	return ret;
}

void WriteResultToLogfile(char* pfilebuf, int mdlidx)
{
    FILE * fp_log;
    //char module_index = pfilebuf[10];
    char filename[50] = {0};
    sprintf(filename, "/home/ftp/data_%02d.txt", mdlidx);
    //open the file
    fp_log = fopen(filename, "ab+");
    //fp_log = fopen("/home/ftp/data.txt", "ab+");
    if (fp_log == NULL)
    {
        printf("open data.txt error\n");
        return ;
    }
    fseek(fp_log, 0, SEEK_END);
    fwrite(pfilebuf, strlen(pfilebuf), 1, fp_log);

    /*int len = ftell(fp_bin);
    rewind(fp_bin);
    char *fileBuffer = (char *) malloc(len);
    fread(fileBuffer, sizeof(unsigned char), len, fp_bin);*/
    fclose(fp_log);
    //gpioinfowrite(addr,fileBuffer,len);
	//free(fileBuffer);
}
