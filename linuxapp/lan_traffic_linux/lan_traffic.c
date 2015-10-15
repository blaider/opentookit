/*************************************************************************
	> File Name: lan_traffic.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Tue 02 Dec 2014 02:46:22 PM CST
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void skipline(FILE *f)
{
  int ch;
  do {
    ch = getc(f);
  } while ( ch != '\n' && ch != EOF );
}

#define FILENAME "/etc/lantraffic"
int month;
unsigned int total;
int saveTraffic()
{
	char buf[100];
	memset(buf,0,sizeof(buf));
	sprintf(buf,"%02d %u\n",month,total);
	int fd = open(FILENAME,O_WRONLY|O_CREAT);
	if(fd < 0)
		return -1;
	write(fd,buf,strlen(buf));
	close(fd);
	sync();
	return 0;
}
int loadTraffic()
{
	int mon;
	unsigned int tot;
	char buf[100];
	memset(buf,0,sizeof(buf));
	int fd = open(FILENAME,O_RDONLY);
	if(fd < 0)
		return -1;
	int readn = read(fd,buf,sizeof(buf));
	close(fd);
	if (readn > 4)
	{
		time_t timer;
		struct tm *tblock;
		timer = time(NULL);
		tblock = localtime(&timer);
		sscanf(buf, "%d %u", &mon, &tot);
		printf("%d,%u,%d\n", mon, tot,tblock->tm_mon);

		if (tblock->tm_mon + 1 == mon)
		{
			month = mon;
			total = tot;
		}
		else
		{
			total = 0;
			month = tblock->tm_mon + 1;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
  FILE *pnd;
//  char buffer[BUFSIZ];
  char *interface;
  struct ifinfo {
    char name[8];
    unsigned int r_bytes, r_pkt, r_err, r_drop, r_fifo, r_frame;
    unsigned int r_compr, r_mcast;
    unsigned int x_bytes, x_pkt, x_err, x_drop, x_fifo, x_coll;
    unsigned int x_carrier, x_compr;
  } ifc;
  unsigned long long bin, bout, lbin, lbout;
  int first;

  if ( argc != 2 ) {
    fprintf(stderr, "Usage: %s interfacen", argv[0]);
    exit(1);
  }

  interface = argv[1];

  first = 1;
  lbin = 0; lbout = 0;

  while ( 1 ) {
    pnd = fopen("/proc/net/dev", "r");
    if ( !pnd ) {
      fprintf(stderr, "%s: /proc/net/dev: %s", argv[0], strerror(errno));
      exit(1);
    }

    /* Skip header */
    skipline(pnd);
    skipline(pnd);

    /* Get interface info */
    do {
    	memset(ifc.name,0,sizeof(ifc.name));
      int ret = fscanf(pnd, " %6[^:]:%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u ",
                  ifc.name,
                  &ifc.r_bytes, &ifc.r_pkt, &ifc.r_err, &ifc.r_drop,
                  &ifc.r_fifo, &ifc.r_frame, &ifc.r_compr, &ifc.r_mcast,
                  &ifc.x_bytes, &ifc.x_pkt, &ifc.x_err, &ifc.x_drop,
                  &ifc.x_fifo, &ifc.x_coll, &ifc.x_carrier, &ifc.x_compr);
      if(ret  != 17 ) {
        	exit(200);
      }
    } while ( strcmp(ifc.name,interface) );

    bin  = ifc.r_bytes + (lbin & ~0xffffffffULL);
    bout = ifc.x_bytes + (lbout & ~0xffffffffULL);

    if ( bin < lbin )
      bin += (1ULL << 32);
    if ( bout < lbout )
      bout += (1ULL << 32);

    if ( !first ) {
      printf("%s=time:%ld tx:%10u rx:%10u transmit:%10Lu receive:%10Lu\n",ifc.name, time(NULL),ifc.x_bytes,ifc.r_bytes, (bout-lbout), (bin-lbin));
      fflush(stdout);
      total  += (bout-lbout)+(bin-lbin);
      saveTraffic();
    } else {
      first = 0;
      loadTraffic();
    }

    lbin = bin;  lbout = bout;

    fclose(pnd);

    sleep(1);
  }
}
