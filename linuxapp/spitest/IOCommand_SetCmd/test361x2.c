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

typedef struct _CMD_LOCATION
{
    int head;
    int tail;
}CMD_LOCATION, *PCMD_LOCATION;

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

static uint8_t tx_msg[256] = {0};

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

int ParseCmdFile(char* pfilebuf, int buflen, PCMD_LOCATION location, int* pcnt)
{
    int ret = 0;
    int cur_pos = 0;
    int cur_state = 0;
    int cmd_count = 0;
    do
    {
        if (cur_state == 0)
        {
            if (((pfilebuf[cur_pos] <= 'f') && (pfilebuf[cur_pos] >= 'a')) || 
                ((pfilebuf[cur_pos] <= 'F') && (pfilebuf[cur_pos] >= 'A')) || 
                ((pfilebuf[cur_pos] <= '9') && (pfilebuf[cur_pos] >= '0')) 
                )
            {
                location[cmd_count].head = cur_pos;
                //printf("start: %d\r\n", location[cmd_count].head);
                cur_state = 1;               
            }
        }
        else
        {
            if ((pfilebuf[cur_pos] == '\r') || (pfilebuf[cur_pos] == '\n'))
            {
                location[cmd_count].tail = cur_pos;                            
                //printf("end: %d\r\n", location[cmd_count].tail);
                cur_state = 0;  
                cmd_count++;
            }
        }
        cur_pos++;
        if (cmd_count > 16)
        {
            break;
        } 
        if (cur_pos == buflen)
        {
            if (cur_state == 1)
            {
                location[cmd_count].tail = cur_pos;
                //printf("end: %d\r\n", location[cmd_count].tail);
                cmd_count++;
            }           
            break;
        }
    }while(1);

    *pcnt = cmd_count;
    return ret;
}

int ParseCmdLine(char* pchbuf, int len, unsigned char* pcmdbuf, unsigned char* pcmdlen)
{
    int ret = 0;
    unsigned char* pcurpos = pchbuf;
    int curidx = 0;
    int prs_len = 0;



    while (prs_len < len)
    {
        
        if (1 == sscanf(pcurpos, "%02x ", &pcmdbuf[curidx]))
        {
            
            //printf("%d: %02x\r\n", curidx, pcmdbuf[curidx]);

            *pcmdlen = curidx++;
        }
        
        pcurpos += 3;
        prs_len += 3;
    }
    
    return ret;
}

int SendSpiCmdInFile(int dev_hdl, char* p_filename)
{
    int ret = 0;
    unsigned char* pfilebuf = 0;
    unsigned int readLen = 0;
    struct stat buf = {0};
    int fd_spicmd = 0;
    int cmd_cnt = 0;
    int i = 0;
    char singel_cmd[1024] = {0};
    int user_sel_idx = -1;
    unsigned char spi_cmd_buf[256] = {0};
    unsigned char spi_cmd_len = 0;
    

    CMD_LOCATION cmd_location[16] = {0};

    do 
    {
        if(stat(p_filename, &buf) < 0) 
        {
            printf("Open %s error!\r\n", p_filename);
            ret = -1;
            break;
        }

        if (buf.st_size <= 0)
        {
            printf("the size of the %d is invalid\r\n", p_filename);
            ret = -1;
            break;
        }

        pfilebuf = (unsigned char*)malloc(buf.st_size);

        if (pfilebuf == 0)
        {
            printf("allocate memory error\r\n");
            ret = -1;
            break;
        }

        fd_spicmd = open(p_filename, O_RDONLY);
        
        if (fd_spicmd < 0)
        {
            printf("Open file failed!\r\n");
            close(fd_spicmd);
            free(pfilebuf);
            ret = -1;
            break;
        }

        readLen = read(fd_spicmd, pfilebuf, buf.st_size);
        
        if (readLen <= 0) 
        {
            printf("reading file failed\r\n");
            close(fd_spicmd);    
            free(pfilebuf);
            ret = -1;
            break;
        }
        
        ParseCmdFile(pfilebuf, readLen, &cmd_location[0], &cmd_cnt);

        printf("%d line data contained\r\n", cmd_cnt);

        for (i = 0; i < cmd_cnt; ++i)
        {
/*            printf("head: %d, tail:%d", cmd_location[i].head, cmd_location[i].tail);*/
           memcpy(&singel_cmd[0], &pfilebuf[cmd_location[i].head], cmd_location[i].tail - cmd_location[i].head);
           singel_cmd[cmd_location[i].tail - cmd_location[i].head] = '\0';
           printf("%d. ", i + 1);
           printf("%s\r\n", singel_cmd);
        }
        printf("please select the cmd you want to send: \r\n");

        scanf("%d", &user_sel_idx);
        
        printf("%d selected\r\n", user_sel_idx);

        if ((user_sel_idx > cmd_cnt) || (user_sel_idx < 1))
        {
            printf("invalid index !\r\n");
            ret = -1;
            break;
        }

        user_sel_idx = user_sel_idx - 1;
        memcpy(&singel_cmd[0], &pfilebuf[cmd_location[user_sel_idx].head], cmd_location[user_sel_idx].tail - cmd_location[user_sel_idx].head);
        singel_cmd[cmd_location[user_sel_idx].tail - cmd_location[user_sel_idx].head] = ' ';
        singel_cmd[cmd_location[user_sel_idx].tail - cmd_location[user_sel_idx].head + 1] = '\0';

        printf("test\r\n");

        ParseCmdLine(singel_cmd, cmd_location[user_sel_idx].tail - cmd_location[user_sel_idx].head + 1, spi_cmd_buf, &spi_cmd_len);

        while (1)
        {
            spi_send(dev_hdl, spi_cmd_buf, spi_cmd_len + 1);
            
            for (i = 0; i < spi_cmd_len + 1; ++i)
            {
                printf("%02x ", spi_cmd_buf[i]);
            }
            printf("\n");
            
            usleep(500000);
        }

        //printf("%s\r\n", pfilebuf);

        free(pfilebuf);

    } while (0);

    return ret;
    
}
int DownloadFWFile(int dev_hdl, char* p_filename, unsigned char slot_id)
{

    int ret = 0;
    struct stat buf = {0};
    unsigned char* p_update_buf = 0;
    int fd_update = 0;
    unsigned int readLen = 0;
    unsigned short checksum = 0;
    int i = 0;
    unsigned char buf_len = 0;

    unsigned int curpos = 0;
    unsigned int cntTransfered = 0;
    unsigned int packsize = 64;

    do
    {
        if(stat(p_filename, &buf) < 0)
        {
            printf("Open %s error!\r\n", p_filename);
            ret = -1;
            break;
        }

        if (buf.st_size > 0)
        {
            p_update_buf = (unsigned char*)malloc(buf.st_size + 2);

            fd_update = open(p_filename, O_RDONLY);

            if (fd_update < 0)
            {
                printf("Open %s failed!\r\n", p_filename);
                free(p_update_buf);
                ret = -1;
                break;
            }

            readLen = read(fd_update, p_update_buf, buf.st_size);

            if (readLen < 0)
            {
                printf("reading file failed\n");
                close(fd_update);
                free(p_update_buf);
                return -1;
            }

            if (readLen == 0)
            {
                printf("The source file length is zero or read file error!\r\n");
                close(fd_update);
                free(p_update_buf);
                return -1;
		    }

            for (i = 0; i < buf.st_size; ++i)
            {
                checksum += p_update_buf[i];
            }
            memcpy(&p_update_buf[buf.st_size], &checksum, 2);

            strDlObj myDlobj = {0};
            myDlobj.ucSubCommand = DL_START_AI;

            buf_len = ConstructSPIPackage(slot_id, 0, CMD_MODULE_FW_DOWNLOAD, (unsigned char*)&myDlobj, 5, tx_msg);
            spi_send(dev_hdl, tx_msg, buf_len);
            usleep(1000000);

            //�������ο�ʼ�ź�
            buf_len = ConstructSPIPackage(slot_id, 0, CMD_MODULE_FW_DOWNLOAD, (unsigned char*)&myDlobj, 5, tx_msg);
            spi_send(dev_hdl, tx_msg, buf_len);
            usleep(3000000);

            curpos = 0;
            cntTransfered = 0;
            myDlobj.DownloadIdx = 0;
            while (cntTransfered < (buf.st_size + 2))
            {

                myDlobj.ucSubCommand = DL_PROCESS_AI;
                if (((buf.st_size + 2) - cntTransfered) > packsize)
                {
                    myDlobj.ucDownloadDataLen = packsize;
                    memcpy(myDlobj.pData, (unsigned char*)(&p_update_buf[curpos]), packsize);
                    curpos += packsize;
                    myDlobj.DownloadIdx &= 0x7FFF;
                    cntTransfered += packsize;
                }
                else
                {
                    myDlobj.ucDownloadDataLen = (buf.st_size + 2) - cntTransfered;
                    memcpy(myDlobj.pData, (unsigned char*)(&p_update_buf[curpos]), myDlobj.ucDownloadDataLen);
                    curpos += myDlobj.ucDownloadDataLen;
                    myDlobj.DownloadIdx != 0x8000;
                    cntTransfered += myDlobj.ucDownloadDataLen;
                }
                buf_len = ConstructSPIPackage(slot_id, 0, CMD_MODULE_FW_DOWNLOAD, (unsigned char*)&myDlobj, myDlobj.ucDownloadDataLen + 4, tx_msg);

                spi_send(dev_hdl, tx_msg, buf_len);

                myDlobj.DownloadIdx++;

                printf("buf.st.size:%d, cnttrans:%d\r\n", buf.st_size, cntTransfered);

                printf("current index: %d, %d%% left\r\n", myDlobj.DownloadIdx, 100.0 * ((float)buf.st_size - (float)cntTransfered) / (float)buf.st_size);

                usleep(200000);

                memset(myDlobj.pData, 0, sizeof(myDlobj.pData));
            }

            myDlobj.ucSubCommand = DL_END_AI;
            buf_len = ConstructSPIPackage(slot_id, 0, CMD_MODULE_FW_DOWNLOAD, (unsigned char*)&myDlobj, 5, tx_msg);
            spi_send(dev_hdl, tx_msg, buf_len);

            usleep(500000);

            free(p_update_buf);
        }
    } while (0);

    return ret;
}

int main(int argc, char *argv[])
{
	int ret = 0;

    int fd = 0;

    char* p_option = NULL;
    char* p_filename = NULL;

    if (argc < 3)
    {
        printf("Please input the command as following");
        printf("test361x -s spicmdfilename\r\nor\r\ntest361x -d downloadfilename\r\n");
        return 0;
    }
    else if (argc < 4)
    {
        
        p_option = argv[1];
        p_filename = argv[2];

        fd = open(device, O_RDWR);
        if (fd < 0)
        {
            pabort("can't open device");
            return -1;
        }
        gfd = fd;
        init_spidev(fd);

        switch (p_option[1])
        {
        case 's':
            SendSpiCmdInFile(fd, p_filename);
            break;
        case 'd':
            DownloadFWFile(fd, p_filename, 0);
            break;
        default:
            printf("wrong option\r\n");
            break;
        }


        close(fd);
    }
	return ret;
}
