// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2017, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 openssl_md5.c
// Author:  suchao.wang
// Created: Jan 9, 2017
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <openssl/md5.h>

int main(int argc , char *argv[])
{
  char data_buf[1024];
  unsigned char md5[16];
  MD5_CTX ctx;
  int data_fd;
  int nread;
  int i;

  if (argc != 2)
  {
    fprintf(stderr, "Please enter the filename\n");
    exit(EXIT_FAILURE);
  }

  data_fd = open(argv[1], O_RDONLY);
  if(data_fd == -1)
  {
    perror("open");
    exit(EXIT_FAILURE);
  }

  MD5_Init(&ctx);
  while(nread = read(data_fd, data_buf, sizeof(data_buf)), nread > 0)
  {
    MD5_Update(&ctx, data_buf, nread);
  }
  MD5_Final(md5, &ctx);

  for(i = 0; i < sizeof(md5); ++i)
  {
    printf("%02x", md5[i]);
  }
  putchar('\n');

  exit(EXIT_SUCCESS);
}
