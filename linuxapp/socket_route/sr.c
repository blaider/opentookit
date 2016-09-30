// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2016, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 sr.c
// Author:  suchao.wang
// Created: Sep 29, 2016
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <net/route.h>
#include <unistd.h>
#include <stdio.h>

int set_gateway(unsigned long gw)
{
  int skfd;
  struct rtentry rt;
  int err;

  skfd = socket(PF_INET, SOCK_DGRAM, 0);
  if (skfd < 0)
    return -1;

  /* Delete existing defalt gateway */
  memset(&rt, 0, sizeof(rt));

  rt.rt_dev = "eth1";
  rt.rt_dst.sa_family = AF_INET;
  ((struct sockaddr_in *)&rt.rt_dst)->sin_addr.s_addr = 0;

  rt.rt_genmask.sa_family = AF_INET;
  ((struct sockaddr_in *)&rt.rt_genmask)->sin_addr.s_addr = 0;

  rt.rt_flags = RTF_UP;

  err = ioctl(skfd, SIOCDELRT, &rt);

  if ((err == 0 || errno == ESRCH) && gw) {
  /* Set default gateway */
  memset(&rt, 0, sizeof(rt));

  rt.rt_dev = "eth1";
  rt.rt_dst.sa_family = AF_INET;
  ((struct sockaddr_in *)&rt.rt_dst)->sin_addr.s_addr = 0;

  rt.rt_gateway.sa_family = AF_INET;
  ((struct sockaddr_in *)&rt.rt_gateway)->sin_addr.s_addr = gw;

  rt.rt_genmask.sa_family = AF_INET;
  ((struct sockaddr_in *)&rt.rt_genmask)->sin_addr.s_addr = 0;

  rt.rt_flags = RTF_UP | RTF_GATEWAY;

  err = ioctl(skfd, SIOCADDRT, &rt);
  if(!err)
	  printf("success\n");
  else
	  perror("SIOCADDRT");
 }else
	  perror("SIOCDELRT");

  close(skfd);

  return err;
}

int get_gateway(unsigned long *p)
{
  FILE *fp;
  char buf[256]; // 128 is enough for linux
  char iface[16];
  unsigned long dest_addr, gate_addr;
  *p = INADDR_NONE;
  fp = fopen("/proc/net/route", "r");
  if (fp == NULL)
    return -1;
  /* Skip title line */
  fgets(buf, sizeof(buf), fp);
  while (fgets(buf, sizeof(buf), fp)) {
    if (sscanf(buf, "%s\t%lX\t%lX", iface,       &dest_addr, &gate_addr) != 3 ||
  dest_addr != 0)
  continue;
  *p = gate_addr;
  break;
  }

  fclose(fp);
  return 0;
}

int main()
{
	set_gateway(inet_addr("192.168.1.3"));
}



