#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/resource.h>
#include <syslog.h>
#include <termios.h>

static sem_t m_hEvent;
static pthread_t	m_hThread;
static pthread_mutex_t mutex_socket= PTHREAD_MUTEX_INITIALIZER;
static int wdt_enable=1;
static char serial_device_name[64] = {0};

struct __list_info
{
	struct __list_info *pre;
	struct __list_info *next;
	int *socket;
	char *buff;
	int buff_len;
};
typedef struct __list_info LISTINFO;

static LISTINFO *list_header = NULL;
static LISTINFO *list_tailer = NULL;

static void push_to_list(LISTINFO *info)
{
	//connect node
	pthread_mutex_lock(&mutex_socket);
	if(list_tailer != NULL)
	{
		info->pre = list_tailer;
		list_tailer->next = info;
	}else{
		list_header = info;
	}

	//move node point to new
	list_tailer = info;
	pthread_mutex_unlock(&mutex_socket);
}

static LISTINFO * pop_list()
{
	pthread_mutex_lock(&mutex_socket);
	if(list_header == NULL)
	{
		pthread_mutex_unlock(&mutex_socket);
		return NULL;
	}

	LISTINFO * info = list_header;

	list_header = info->next;
	if(list_header == NULL)
	{
		list_tailer = NULL;
	}

	pthread_mutex_unlock(&mutex_socket);
	return info;
}

int push_at_command(char *buff,int len,int *socket)
{
	LISTINFO *info = (LISTINFO *)malloc(sizeof(LISTINFO));
	memset(info,0,sizeof(*info));
	info->socket = socket;
	info->buff_len = len;
	info->buff = (char *)malloc(len);
	memcpy(info->buff,buff,len);
	printf("%s,%s\n",__func__,info->buff);
	push_to_list(info);
	return 0;
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


int readMobile(int fd,char *cmd,int cmdLen,char *res,int resLen)
{
	if(cmd == NULL || res == NULL )
		return -1;
	char buf[512];
	if(cmdLen > 512)
		return -1;
	memcpy(buf,cmd,cmdLen);
	if(buf[cmdLen-1] != '\r')
	{
		buf[cmdLen] = '\r';
		cmdLen ++;
	}
	memset(res, 0, resLen);

	int nwrite,nread;

	nwrite = write(fd, buf, cmdLen);
	if(cmdLen != nwrite )
	{
		printf("close %s when write %d/%d",serial_device_name,nwrite,cmdLen);
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

	return offset;
}

static void sigroutine(int dunno) { /**/
	exit(-1);
}

#define APP_NAME						"SerialDaemon"
#ifndef VERSION_NUMBER
#define VERSION_NUMBER					""
#endif

static void print_app_version ( void )
{
#ifdef REVISION_NUMBER
	printf( "%s %s rev %s\n", APP_NAME, VERSION_NUMBER, REVISION_NUMBER );
#else
	printf( "%s %s build %s %s\n", APP_NAME, VERSION_NUMBER,__DATE__,__TIME__ );
#endif
}

int Lockfiles(const int iFd)
{
	struct flock    stLock;

	stLock.l_type = F_WRLCK;        /* F_RDLCK, F_WRLCK, F_UNLCK */
	stLock.l_start = 0;    /* byte offset, relative to l_whence */
	stLock.l_whence = SEEK_SET;    /* SEEK_SET, SEEK_CUR, SEEK_END */
	stLock.l_len = 0;        /* #bytes (0 means to EOF) */

	return (fcntl(iFd, F_SETLK, &stLock));
}
static bool process_is_first_instance ( char const * proc_name )
{
	char pid_file_name[ PATH_MAX ];
	int pid_file;
	int ret;
	mode_t pre_mode;

	sprintf(
		pid_file_name,
		"/tmp/apal_proc_%s.pid",
		proc_name );

	pre_mode = umask(0);
	pid_file = open(
		pid_file_name,
		O_CREAT | O_RDWR,
		0666 );
	umask(pre_mode);
	if ( -1 == pid_file )
	{
		return 0;
	}

	ret = Lockfiles( pid_file );
	if ( 0 == ret )
	{
		// this is the first instance
		return 1;
	}
	return 0;
}

static int get_at_command_count(char *buf,int len)
{
	int cmdcount = 0;
	char *p = NULL;
	char *pstart = buf;
	do
	{
		p = strchr(pstart,'\r');
		if( p!= NULL && p-buf < len)
		{
			cmdcount++;
			pstart = p+1;
		}else if(pstart-buf < len)
		{
			cmdcount++;
		}
	}while(p != NULL && p-buf < len);
	return cmdcount;
}

static int split_and_send_at_command(char *inbuff,int len,char *buff,int size)
{
	int cmdcount  = 0;
	char *p = NULL;
	char *pstart = inbuff;
	int ret = 0;
	int fd;

	if(strlen(serial_device_name) < 5)
	{
//		if(get_gprs_config_node_name(usbLabe,configPort,serialName,sizeof(serialName)) < 0)
//			return -2;

		if(access(serial_device_name,F_OK))
		{
			memset(serial_device_name,0,sizeof(serial_device_name));
			return -2;
		}
	}

	fd = open(serial_device_name, O_RDWR | O_NOCTTY | O_NDELAY);
	if (-1 == fd)
	{
		perror(serial_device_name);
		memset(serial_device_name,0,sizeof(serial_device_name));
		return -2;
	}
	serial_init(fd);

	cmdcount = get_at_command_count(inbuff,len);

	printf("%s:count=%d\n\n",__func__,cmdcount);

	pstart = inbuff;
	do
	{
		p = strchr(pstart,'\r');
		if( (p!= NULL && p-inbuff < len) || (pstart-inbuff < len))
		{
			int slen = 0;
			if(p != NULL)//at comamnd end with '\r'
				slen = p-pstart + 1;
			else//last at command with no '\r'
				slen = len - (pstart - inbuff) ;
			memset(buff,0,size);
			ret = readMobile(fd,pstart, slen ,buff,size);
			if(strstr(buff,"ERROR") != NULL)
			{
				close(fd);
				return ret;
			}
			if(p != NULL)
				pstart = p+1;
			else
				pstart = inbuff+len;
		}
	}while(p != NULL && p-inbuff < len);

	close(fd);
	return ret;
}

static void* ProcessLocalSerialDeviceThread( void* arg )
{
	char lbuff[2000];
	int rlen = 0;
	while(sem_trywait(&m_hEvent))
	{
		LISTINFO * info = pop_list();
		if(info == NULL)
		{
			sleep(1);
			continue;
		}
		//socket should be valid
		if(*(info->socket) > 0)
		{
			rlen = split_and_send_at_command(info->buff,info->buff_len,lbuff,sizeof(lbuff));
			//send last at command response data back to client
			if(rlen > 0 && *(info->socket) > 0)
				send(*info->socket,lbuff,rlen,MSG_NOSIGNAL);
		}
		free(info->buff);
		free(info);
	}
	return 0;
}


static void *read_local_socket_data(void *arg)
{
	int client_sockfd = (int)arg;
	printf("the server wait for client data\n");
	char buff[2000] = {0};
	while(1)
	{
		memset(buff,0,sizeof(buff));
		int n = recv(client_sockfd,buff,sizeof(buff),0);
		//socket error
		if(n < 0)
		{
			break;
		}else if( n > 0)//read data
		{
			push_at_command(buff,n,&client_sockfd);
		}else//scoket break
		{
			break;
		}
	}
	close(client_sockfd);
	client_sockfd = -1;
	return NULL;
}

#define SOCKETFILE "/tmp/socket_gprs_serial"
int main ( int argc, char* argv[] )
{
	//Enable core_dump
#ifdef DEBUG
	struct rlimit rlim;
	rlim.rlim_cur = 10*1024*1024;
	rlim.rlim_max = 10*1024*1024;
	setrlimit( RLIMIT_CORE, &rlim );
#endif
	//end enable core_dump
	mode_t pre_mode;

	if ( argc > 1 )
	{
 		if ( strcmp( argv[ 1 ], "--version" ) == 0 )
 		{
 			print_app_version();
 			return 0;
 		}

		if ( strcmp( argv[ 1 ], "-d" ) == 0 )
		{
			daemon(0, 0);
 		}
	}

	signal(SIGINT, sigroutine);

	int ret;
	ret = process_is_first_instance(APP_NAME);
	if (!ret)
	{
//		LOGEX("%s already running\n", APP_NAME);
		exit(0);
	}
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	struct sockaddr_un server_address;
	struct sockaddr_un client_address;
	unlink(SOCKETFILE);
	server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_sockfd == -1)
	{
		perror("socket");
		exit(1);
	}
	server_address.sun_family = AF_UNIX;
	strcpy(server_address.sun_path, SOCKETFILE);
	server_len = sizeof(server_address);

	pre_mode = umask(0);
	bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
	umask(pre_mode);
	listen(server_sockfd, 5);
	printf("server waiting for client connect\n");
	client_len = sizeof(client_address);

	strcpy(serial_device_name,"/dev/ttyUSB2");
	printf("wdt_enable=%d\n",wdt_enable);

	if (sem_init(&m_hEvent, 0, 0) == -1)
		return false;
	pthread_create(&m_hThread, NULL, ProcessLocalSerialDeviceThread, (void*) 0);
	while(1)
	{
		client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address,
				(socklen_t *) &client_len);
		pthread_create(&m_hThread, NULL, read_local_socket_data, (void*) client_sockfd);
	}
	unlink(SOCKETFILE);

}
