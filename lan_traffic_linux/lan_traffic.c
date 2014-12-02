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

void skipline(FILE *f)
{
  int ch;
  do {
    ch = getc(f);
  } while ( ch != '\n' && ch != EOF );
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
      printf("time:%ld transmit:%10Lu receive:%10Lu\n", time(NULL), (bout-lbout), (bin-lbin));
      fflush(stdout);
    } else {
      first = 0;
    }

    lbin = bin;  lbout = bout;

    fclose(pnd);

    sleep(1);
  }
}
