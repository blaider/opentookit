/*************************************************************************
	> File Name: readself.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 05 Dec 2014 06:51:33 PM CST
 ************************************************************************/

#include<stdio.h>
#include <limits.h>
#include <unistd.h>

int skipline(FILE *f)
{
  int ch;
  do {
    ch = getc(f);
  } while ( ch != '\n' && ch != EOF );
  return 0;
}
#define PROC_NET_WIRELESS	"/proc/net/wireless"
int readWirelessSignal(const char *ifname)
{
	int ret=0;
	FILE *	f = fopen(PROC_NET_WIRELESS, "r");
	      char	buf[256];
	      char *	bp;
	      int	t;

	      if(f==NULL)
		return -1;
	      /* Loop on all devices */
	      while(fgets(buf,255,f))
		{
		  bp=buf;
		  while(*bp&&isspace(*bp))
		    bp++;
		  /* Is it the good device ? */
		  if(strncmp(bp,ifname,strlen(ifname))==0 && bp[strlen(ifname)]==':')
		    {
			  printf("buf:%s",buf);
		      /* Skip ethX: */
		      bp=strchr(bp,':');
		      bp++;
		      /* -- status -- */
		      bp = strtok(bp, " ");
		      sscanf(bp, "%X", &t);
		      printf("stats->status =%d\n", t);
		      /* -- link quality -- */
		      bp = strtok(NULL, " ");
		      if(strchr(bp,'.') != NULL)
		    	  printf("updated\n");
//			stats->qual.updated |= 1;
		      sscanf(bp, "%d", &t);
		      printf(" stats->qual.qual =%d\n",  (unsigned char) t);
		      /* -- signal level -- */
		      bp = strtok(NULL, " ");
		      if(strchr(bp,'.') != NULL)
		    	  printf("updated\n");
		      sscanf(bp, "%d", &t);
		      printf(" stats->qual.level =%d\n",   t);
		      /* -- noise level -- */
		      bp = strtok(NULL, " ");
		      if(strchr(bp,'.') != NULL)
		    	  printf("updated\n");
		      sscanf(bp, "%d", &t);
		      printf(" stats->qual.noise =%d\n",  t);
		      /* -- discarded packets -- */
		      int nwid,code,misc;
		      bp = strtok(NULL, " ");
		      sscanf(bp, "%d", &nwid);
		      bp = strtok(NULL, " ");
		      sscanf(bp, "%d", &code);
		      bp = strtok(NULL, " ");
		      sscanf(bp, "%d", &misc);
		      printf("nwid=%d,code=%d,misc=%d\n",nwid,code,misc);
		      fclose(f);
		      /* No conversion needed */
		      return 0;
		    }
		}
	      fclose(f);
	return ret;

}
int main(int argc,char *argv[])
{
		while(1)
		{
			readWirelessSignal("wlan0");
			sleep(1);
		}
		return 0;
}
