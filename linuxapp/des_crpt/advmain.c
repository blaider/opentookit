#include <stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include <string.h>
#include <rpc/des_crypt.h>
#include <iconv.h>

static char key[] = {65, 68, 65, 77, 51, 54, 48, 48,0};
static   char iv[] = {114, 100, 97, 98, 106, 49, 53, 55,0};

//注意：这里有个坑，需要保证 data 可以存放的字符串在后面补了\8 不会越界。否则结果是不确定的。
void cbc_encrypt(const char *key, char *data, int len,char iv[])
{
    char pkey[9];
    char piv[9];
    sprintf(pkey, key);
    sprintf(piv,iv);
    des_setparity(pkey);
    do {
           data[len++] = '\x8';
       } while (len % 8 != 0);
    int stat  = cbc_crypt(pkey, data, len, DES_ENCRYPT| DES_SW,iv);
    if (DES_FAILED(stat))
	{
		/* parse the error returned for a known meaning */
		printf("cbc_crypt() fatal error while encrypting: ");
		switch (stat)
		{
		case DESERR_HWERROR:
			printf( "Fatal driver error.");
			break;
		case DESERR_BADPARAM:
			printf( "Bad parameter passed.");
			break;
		default:
			printf( "Unknown fault.");
			break;
		}
		/* print the cause of an error and exit */
		exit(EXIT_FAILURE);
	}
    int i=0;
	for( i = 0;i<strlen(data);i++)
	{
		printf("%hhx ",data[i]);
	}
	printf(" dddddd\n");
	checkPassword(data,strlen(data));
}

int checkPassword(char *da,int len)
{


	char data[4096];
	memset(data,0,sizeof(data));
	strncpy(data, da,len);

	int i=0;
		for( i = 0;i<strlen(data);i++)
		{
			printf("%hhx ",data[i]);
		}
		printf("\n");

		  do {
		           data[len++] = '\x8';
		       } while (len % 8 != 0);
	//decrypt password
	char pkey[9];
	char piv[9];
	sprintf(pkey, key);
	sprintf(piv,iv);
	des_setparity(pkey);
	cbc_crypt(pkey, data, strlen(data), DES_DECRYPT, piv);

	int pad = data[strlen(data)-1];
	memset(data+strlen(data) - pad,0,pad);
	printf("%s:decrypt raw=%s,%d\n",__func__,data,strlen(data));

	return 0;
}

int main()
{
	char data[1024] = {6,54,61,183,216,91,251,45,0};
	char data1[1024] = {203,37,146,215,65,128,225,151,200,35,133,142,62,112,49,81,143,70,68,93,229,93,226,43,0};
	char nullpasswd[] = {115, 7, 27, 62, 252, 126, 188, 160,0};
//	checkPassword(data,strlen(data));
	checkPassword(data1,strlen(data1));
	checkPassword(nullpasswd,strlen(nullpasswd));

	char data2[1024];
	memset(data2,0,sizeof(data2));
	sprintf(data2,"123456");
	cbc_encrypt(key,data2,strlen(data2),iv);
	checkPassword(data2,strlen(data2));
	char data3[1024]= {0xe4,0x6c,0x7d,0x5b,0x23,0x76,0xb6,0x80};
	checkPassword(data3,strlen(data3));
}
