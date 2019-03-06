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
//#include <unistd.h>


//------------------------------------------------------------------------------
#define NSEC_PER_SEC    1000000000
#define MAX_SAFE_STACK (8*1024) /* The maximum stack size which is
                                   guaranteed safe to access without
                                   faulting */
#pragma pack(1)
typedef struct tagDL{
    unsigned char ucSubCommand;
    unsigned short DownloadIdx;
    unsigned char ucDownloadDataLen;
    unsigned char pData[68];
}strDlObj,*pstrDLObj;
#pragma pack()

#define DL_START_AI 0x08
#define DL_PROCESS_AI 0x09
#define DL_END_AI 0x0a
#define CMD_MODULE_FW_DOWNLOAD 0x02
                                   
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

static int gfd, gfd_slave;

#define BUFFER_LEN		ARRAY_SIZE(rx_search_msg)

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static uint8_t tx_search_msg[256] = {0};

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

static void spi_send(int fd, int buf_len)
{
	int ret;

	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx_search_msg,
		.rx_buf = 0, 
		.len = buf_len,
		.delay_usecs = 0, //delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if (ret < 1)
		pabort("can't send spi message");

	return;
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

	//printf("spi mode: %d\n", mode);
	//printf("bits per word: %d\n", bits);
	//printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
}

unsigned char ConstructSPIPackage(unsigned char id, unsigned char direction, unsigned char command_id
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
    pbufdst[7] = package_len;
    for (i = 0; i < 8; ++i)
    {
        checksum += pbufdst[i];
    }
    pbufdst[8] = checksum & 0xff;
    pbufdst[9] = (checksum >> 8) & 0xff;
    pbufdst[10] = id;
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

int main(int argc, char *argv[])
{
	int ret;
	int fd, fd_slave, fd_update = 0;
    int interval;
    //int direction = 0;
    //int spicmdid = 0x40;
	

	unsigned int slot_id = 0;
    unsigned int cmd_id = 0;
	unsigned int cmd_value = 0;
    unsigned int spicmdid = 0x40;
    unsigned int spicmddir = 0;
	
	unsigned char inbuf[96] = {0};
	
	int buf_len = 0, i = 0;
    char* pfilename = 0;
    struct stat buf = {0};
    unsigned char* pUpdateBuf = 0;
    unsigned int readLen = 0;

    unsigned int curpos = 0;
    unsigned int packsize = 64;
    unsigned int cntTransfered = 0;
    unsigned short checksum = 0;
	
	if(argc < 4){
		printf("Please input the command as following");
		printf("test361x slot_id cmd value\r\n\r\ntest361x 2 c1 08\r\nmeans set the slot 2 module gain code as 8");
		return 0;
	}
	
	slot_id = atoi(argv[1]);

    
    sscanf(argv[2], "%x", &cmd_id);
    
	sscanf(argv[3], "%x", &cmd_value);
    if (argc > 4)
    {
        sscanf(argv[4], "%x", &spicmddir);
    }
    if (argc > 5) 
    {
        sscanf(argv[5], "%x", &spicmdid);
    }
    if (argc > 6)
    {
        pfilename = argv[6];
    }

    inbuf[0] = 0;
    inbuf[1] = 1;
    inbuf[2] = cmd_id;
    inbuf[3] = cmd_value;
    if (argc > 4)
    {
        inbuf[4] = spicmddir;
    }
    if (argc > 5) 
    {
        inbuf[5] = spicmdid;
    }

    /////////////////////    
    if(stat(pfilename, &buf)<0) 
    { 
        return 0; 
    }
    if (buf.st_size > 0)
    {
        //pUpdateBuf = new char[buf.st_size + 2]; //+2用来存储checksum
        pUpdateBuf = (unsigned char*)malloc(buf.st_size); //+2用来存储checksum
    }

    if (0 != pUpdateBuf)
    {
        fd = open(device, O_RDWR);
        if (fd < 0)
        {
            pabort("can't open device");
            return -1;
        }
        gfd = fd;
        init_spidev(fd);

        fd_update = open(pfilename, O_RDONLY);

        if (fd_update < 0)
        {
            printf("Open file failed!\r\n");
            close(fd);
            free(pUpdateBuf);
            //delete[] pUpdateBuf;
            return -1;
        }
        readLen = read(fd_update, pUpdateBuf, buf.st_size);

        if (readLen < 0) 
        {
            printf("reading file failed\n");
            close(fd);    
            close(fd_update);
            free(pUpdateBuf);
            //delete[] pUpdateBuf;
            return -1;
        }
        
        if (readLen == 0)
        {
            printf("The source file length is zero or read file error!\r\n");
            close(fd); 
            close(fd_update);
            free(pUpdateBuf);
            //delete[] pUpdateBuf;
            return -1;
		}

        for (i = 0; i < buf.st_size; ++i)
        {
            checksum += pUpdateBuf[i];
        }
        memcpy(&pUpdateBuf[buf.st_size], &checksum, 2);
        
        strDlObj myDlobj = {0};
        myDlobj.ucSubCommand = DL_START_AI;
        
        buf_len = ConstructSPIPackage(slot_id, 0, CMD_MODULE_FW_DOWNLOAD, (unsigned char*)&myDlobj, 5, tx_search_msg);
        spi_send(fd, buf_len);
        usleep(1000000);

        //发送两次开始信号
        buf_len = ConstructSPIPackage(slot_id, 0, CMD_MODULE_FW_DOWNLOAD, (unsigned char*)&myDlobj, 5, tx_search_msg);        
        spi_send(fd, buf_len);
        usleep(3000000);

        curpos = 0;
        cntTransfered = 0;
        myDlobj.DownloadIdx = 0;
        while (cntTransfered < buf.st_size)
        {
            
            myDlobj.ucSubCommand = DL_PROCESS_AI;
            if ((buf.st_size - cntTransfered) > packsize)
            {
                myDlobj.ucDownloadDataLen = packsize;
                memcpy(myDlobj.pData, (unsigned char*)(&pUpdateBuf[curpos]), packsize);
                //myDlobj.pData = (uint8_t*)(&pUpdateBuf[curpos]);
                curpos += packsize;
                myDlobj.DownloadIdx &= 0x7FFF;
                cntTransfered += packsize;
            }
            else
            {
                myDlobj.ucDownloadDataLen = buf.st_size - cntTransfered + 2;
                memcpy(myDlobj.pData, (unsigned char*)(&pUpdateBuf[curpos]), buf.st_size - cntTransfered + 2);
                //myDlobj.pData = (uint8_t*)(&checksum);
                curpos += (buf.st_size - cntTransfered);
                myDlobj.DownloadIdx != 0x8000;
                cntTransfered += (buf.st_size - cntTransfered);
            }

            buf_len = ConstructSPIPackage(slot_id, 0, CMD_MODULE_FW_DOWNLOAD, (unsigned char*)&myDlobj, myDlobj.ucDownloadDataLen + 4, tx_search_msg);
            //spi_send(fd, buf_len);

            myDlobj.DownloadIdx++;

            printf("current index: %d, %d%% left\r\n", myDlobj.DownloadIdx, 100.0 * (buf.st_size - cntTransfered) / (float)buf.st_size);

            usleep(500000);
            memset(myDlobj.pData, 0, sizeof(myDlobj.pData));
            
        }

        myDlobj.ucSubCommand = DL_END_AI;     
        buf_len = ConstructSPIPackage(slot_id, 0, CMD_MODULE_FW_DOWNLOAD, (unsigned char*)&myDlobj, 5, tx_search_msg);
        spi_send(fd, buf_len);
        //Sleep(500);

        
//         buf_len = ConstructSPIPackage(slot_id, spicmddir, spicmdid, inbuf, 6, tx_search_msg);
//         
//         spi_send(fd, buf_len);
        close(fd_update);
        
	    close(fd);
        
        free(pUpdateBuf);
        pUpdateBuf = 0;
        //delete[] pUpdateBuf;
    }
    

	return ret;
}
