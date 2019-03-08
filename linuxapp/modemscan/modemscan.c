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


int showTty_byPrefix(char *prefix)
{
    char buf[256];
	char linkName[256];
	int i;
	int fd;

	for(i =0 ;i< 256;i++)
	{
		sprintf(buf, "%s%d", prefix,i);
		fd = open(buf, O_RDWR | O_NOCTTY);
		if (-1 == fd)
		{
			continue;
		}
		struct termios opt;
		if( tcgetattr(fd ,&opt) != 0)
		{
            close(fd);
			continue;
		}
		memset(linkName,0,sizeof(linkName));
		int linkSize = readlink(buf,linkName,sizeof(linkName));
		printf("%s %s %s\n",buf,(linkSize > 0)?" ->":"",linkName);
        close(fd);
	}
}


int check_tty(char *devnode)
{
    int fd = open(devnode, O_RDWR |O_NOCTTY | O_NDELAY );
    if (-1 == fd)
    {
        return 0;
    }
    if(isatty(fd) == 0)
    {
       close(fd);
       return 0;
    }
/*
    struct termios opt;
    if (tcgetattr(fd, &opt) != 0)
    {
       close(fd);
       return 0;
    }*/
    close(fd);
    return 1;
}

void show_usage(char *cmd)
{
    printf("Usage:");
    printf("\t%s -l           ;list all modem device\n", cmd);
    printf("\t%s /dev/ttyUSB1 ;check modem device\n", cmd);
    return;
}



int check_device(char *devname)
{
    if(access(devname,F_OK) != 0 )
    {
        printf("%s is not exist\n",devname);
        return 0;
    }
    if(check_tty(devname) == 0)
    {
        printf("%s is not a tty device\n",devname);
        return 0;
    }
    if(is_modem(devname) == 0)
    {
        printf("%s is not a modem\n",devname);
        return 0;
    } 
    printf("%s is a modem\n",devname);
    show_modem(devname);


    return 0;
}

void show_tty_device(struct dirent *entry)
{
    struct stat statbuf;
    lstat(entry->d_name, &statbuf);

    if (S_ISCHR(statbuf.st_mode)) {

        if(check_tty(entry->d_name))
        {
           // printf("%s\n",entry->d_name);
            check_device(entry->d_name);
        }
    }
    
    if (S_ISLNK(statbuf.st_mode) && check_tty(entry->d_name)) {

        char linkName[256];
        memset(linkName, 0, sizeof(linkName));
        int linkSize = readlink(entry->d_name, linkName, sizeof(linkName));
        printf("%s %s %s\n",entry->d_name,(linkSize > 0)?" ->":" ",linkName);    
    }

}

static char console_tty[64];
static char cmd_tty[64];

int filter(const struct dirent *entry)
{
    //skip console tty
    if(strcmp(entry->d_name,console_tty) == 0)
        return 0;
    
    //skip cmdline tty
    if(strcmp(entry->d_name,cmd_tty) == 0)
        return 0;


    return (strncmp(entry->d_name,"tty",3) == 0) 
            && (strlen(entry->d_name)>3) 
            && !isdigit(entry->d_name[3]);
}

void list_dev()
{
    const char *szPath = "/dev/";
	struct dirent *entry;
    struct dirent **namelist;
    int n;

    n = scandir(szPath,&namelist,filter,alphasort);
    if(n < 0)
    { 
        return;
    }
    else
    {
        int index=0;
        chdir(szPath);
        while(index < n)
        {
           // printf("d_ino：%ld  d_off:%ld d_name: %s\n", namelist[index]->d_ino,namelist[index]->d_off,namelist[index]->d_name);
            show_tty_device(namelist[index]);
            free(namelist[index]);
            index++;
        }
        free(namelist);
    }
}


int init_basetty()
{
    char buff[1024];
    memset(console_tty,0,sizeof(console_tty));
    strcpy(console_tty,ttyname(STDIN_FILENO));
    printf("Current con tty: %s\n", console_tty);

     memset(cmd_tty,0,sizeof(cmd_tty));
    int fd = open("/proc/cmdline",O_RDONLY);
    if(fd > 0)
    {
        int n = read(fd,buff,sizeof(buff));
        if(n > 0)
        {
            char *p = strstr(buff,"console=");
            if(p != NULL)
            {
                sscanf(p,"console=%[^,| ]",cmd_tty);
            }
        }

    }
    printf("Current cmd tty: %s\n", cmd_tty);

}

int main(int argc,char *argv[])
{
    //show_usage(argv[0]);
    init_basetty();

    if(argc < 2)
    {
        show_usage(argv[0]);
        return 0;
    }

    if(strcmp(argv[1],"-l") == 0)
    {
         list_dev();
    }else if(strncmp(argv[1],"/dev/",5) == 0){
        check_device(argv[1]);
        return 0;
    }

  //  show_usage(argv[0]);
    return 0;
}