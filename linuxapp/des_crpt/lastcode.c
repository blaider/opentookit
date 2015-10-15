// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2015, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 goocode.c
// Author:  suchao.wang
// Created: Jul 23, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

/*
    ============================================================================
    Name        : test-c.c
    Description : Testing Project
                  Trying to do a C version of this perl code:
                    my $cipher = Crypt::CBC->new( -key => $salt_key, -cipher => 'DES' -header => 'none');
                    my $enc_text = $cipher->encrypt_hex($text);
    Requires    : -lcrypt
    References  :
      Function: cbc_crypt (char *key, char *blocks, unsigned len, unsigned mode, char *ivec)
      GNU C Library: DES Encryption (http://www.gnu.org/software/libc/manual/html_node/DES-Encryption.html#DES-Encryption)
      cbc_crypt (http://unix.derkeiler.com/Newsgroups/comp.unix.programmer/2012-10/msg00023.html)
    ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/des_crypt.h>

showbuf(char *buf,int len)
{
	int i = 0;
	for(i=0;i<len;i++)
		printf("%d ",*(buf+i)&0xff);
	printf("\n");
}
#define BUFFSIZE 420
int main(void) {
    //  char key[]     = "aBcDeFg1";
//    char keys[] = {50, 48, 49, 53, 48, 55, 48, 49,0};
//    char ivs[] = {49, 50, 51, 52, 53, 54, 55, 56 ,0};
//    char das[] = {128, 145, 246, 75, 79, 100, 47, 247, 113, 246, 170, 253, 40, 206, 205, 131,0};

    char keys[] = {135, 209, 173, 241, 160, 190, 131, 51,0};
      char ivs[]  ={22, 55, 73, 173, 2, 21, 82, 104,0};
       char das[] = {21, 204, 19, 17, 181, 199, 214, 224, 99, 227, 212, 54, 95, 131, 93, 143,0};


    char das1[] = {48,48,48,48,48,48,48,48,8,8,8,8,8,8,8,8,0};
    char key[9];
    sprintf(key,keys);
    //char pass[]    = "mypass1234test";
    char pass[BUFFSIZE];
    sprintf(pass,das1);

    //  char encbuff[] = "87654321";
    char ivec[9];
    sprintf(ivec,ivs);
    //  char decbuff[] = "87645321";
    char ivectemp[9];
    strcpy(ivectemp,ivec);
    int buffsize;
    int result;

    des_setparity(key);
    showbuf(key,strlen(key));
    /* Encrypt pass, result is in encbuff */
    buffsize = strlen(pass);
    printf("buffsize is %d\n",buffsize);
    /* Add to pass to ensure size is divisable by 8. */
    while (buffsize % 8 && buffsize<BUFFSIZE) {
        pass[buffsize++] = '\0';
    }
    printf("pass is %s\n",pass);
    showbuf(pass,strlen(pass));
    printf("buffsize is %d\n",buffsize);
    printf("Encrypted: ");
    result = cbc_crypt(key, pass, buffsize, DES_ENCRYPT | DES_SW, ivectemp);
    if (DES_FAILED(result) || strcmp(pass, "") == 0) {
        if(strcmp(pass, "") == 0) {
            printf("*** Null Output ***\n");
        } else {
            printf("*** Encryption Error ***\n");
        }
    } else {
        printf("%s\n", pass);
        showbuf(pass,strlen(pass));
    }

//    strcpy(pass,das);

    /* Decrypt encbuff, result is in decbuff */

    /* FIXME: Decryption doesn't work:
            Encrypted: ,ï¿½7&ï¿½ï¿½ï¿½8
            Decrypted: *** Decryption Error ***
     */
    buffsize = strlen(pass);
    printf("buffsize is %d\n",buffsize);
    /* Add to pass to ensure size is divisable by 8. */
    while (buffsize % 8 && buffsize<BUFFSIZE) {
        pass[buffsize++] = '\0';
    }
    printf("buffsize is %d\n",buffsize);
    printf("Decrypted: ");
    //keeping the same initialization vector for decrypting, encrypt has altered ivectemp
    strcpy(ivectemp,ivec);
    result = cbc_crypt(key, pass, buffsize, DES_DECRYPT | DES_SW, ivectemp);
    if(DES_FAILED(result) || strcmp(pass, "") == 0) {
        if(strcmp(pass, "") == 0) {
            printf("*** Null Output ***\n");
        } else {
            printf("*** Decryption Error ***\n");
        }
    } else {
        printf("%d,%s\n",strlen(pass),pass);
        showbuf(pass,strlen(pass));
    }

    return 0;
}

