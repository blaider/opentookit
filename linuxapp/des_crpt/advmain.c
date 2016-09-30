#include <stdio.h>
#include <stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include <string.h>
#include <rpc/des_crypt.h>
#include <iconv.h>

static char key[] = {65, 68, 65, 77, 51, 54, 48, 48,0};
static   char iv[] = {114, 100, 97, 98, 106, 49, 53, 55,0};
int checkPassword(char *da,int len)
{

	char data[4096];
	memset(data,0,sizeof(data));
	strncpy(data, da,len);

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
	char data[] = {6,54,61,183,216,91,251,45,0};
	char data1[] = {203,37,146,215,65,128,225,151,200,35,133,142,62,112,49,81,143,70,68,93,229,93,226,43,0};
	checkPassword(data,strlen(data));
	checkPassword(data1,strlen(data1));

	char datastr[]="BjY9t9hb+y0=";
	char data3[204];
	size_t ib,ob;
	ib = strlen(datastr);
	iconv_t cd = iconv_open("ansi", "utf-8");
	iconv(cd,(char **)&datastr, &ib,
	                    (char **)&data3, &ob);
	 iconv_close(cd);
	 printf("%d\n",ob);
}
