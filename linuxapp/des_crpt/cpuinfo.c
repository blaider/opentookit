#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <rpc/des_crypt.h>
#include <pwd.h>
#include <shadow.h>

//注意：这里有个坑，需要保证 data 可以存放的字符串在后面补了\8 不会越界。否则结果是不确定的。
void des_encrypt(const char *key, char *data, int len)
{
    char pkey[8];
    strncpy(pkey, key, 8);
    des_setparity(pkey);
    do {
        data[len++] = '\x8';
    } while (len % 8 != 0);
    ecb_crypt(pkey, data, len, DES_ENCRYPT);
}

void des_decrypt(const char *key, char *data, int len)
{
    char pkey[8];
    strncpy(pkey, key, 8);
    des_setparity(pkey);
    ecb_crypt(pkey, data, len, DES_DECRYPT);
}


//注意：这里有个坑，需要保证 data 可以存放的字符串在后面补了\8 不会越界。否则结果是不确定的。
void cbc_encrypt(const char *key, char *data, int len,char iv[])
{
    char pkey[8];
    strncpy(pkey, key, 8);
    des_setparity(pkey);
    do {
           data[len++] = '\x8';
       } while (len % 8 != 0);
    cbc_crypt(pkey, data, len, DES_ENCRYPT,iv);
}

void cbc_decrypt(const char *key, char *data, int len,char iv[])
{
    char pkey[8];
    strncpy(pkey, key, 8);
    des_setparity(pkey);
    cbc_crypt(pkey, data, len, DES_DECRYPT,iv);
}


showbuf(char *buf,int len)
{
	int i = 0;
	for(i=0;i<len;i++)
		printf("%d ",*(buf+i)&0xff);
	printf("\n");
}

int main(int argc, char *argv[])
{
    /*
    * char data[4096] = "cea3e8e1659582206e0be32539729e9f";
    * des_encrypt("12345678", data, strlen(data));
    * printf("%s\n", data);
    * //should be "04fMaWegkH1/BL9CNYxgusFpYK8wdraBX06mPiRmxJP+uVm31GQvyw=="
    */
	int stat = 0;

    char data[4096];
    memset(data,0,sizeof(data));
//    int i = 0;
//    while (EOF != (data[i] = fgetc(stdin))) {
//        i++;
//    }
////    data[i] = '\0';
//    char key[] = {50, 48, 49, 53, 48, 55, 48, 49,0};
//    char iv[] = {49, 50, 51, 52, 53, 54, 55, 56 ,0};
//    char da[] = {128, 145, 246, 75, 79, 100, 47, 247, 113, 246, 170, 253, 40, 206, 205, 131,0};
//    char da[] = {165, 59, 71, 41, 91, 24, 33, 144,0};
//    char da[] = {68, 5, 18, 186, 37, 101, 16, 194 ,0};
//    char da[] = "FcwTEbXH1uBj49Q2X4Ndjw==";

//    char key[] = {135, 209, 173, 241, 160, 190, 131, 51,0};
//   char iv[]  ={22, 55, 73, 173, 2, 21, 82, 104,0};
//    char da[] = {21, 204, 19, 17, 181, 199, 214, 224, 99, 227, 212, 54, 95, 131, 93, 143,0};
// char da[] = {30,192,187,117,213,199,231,32,220,186,163,190,254,101,228,75,0};

//   char da[] = {68, 5, 18, 186, 37, 101, 16, 194 ,0};

    char key[] = {65, 68, 65, 77, 51, 54, 48, 48,0};
    char iv[] = {114, 100, 97, 98, 106, 49, 53, 55,0};
    char da[] =   {5, 236, 234, 163, 54, 157, 138, 110, 205, 144, 8, 236, 109, 164, 67, 117,0};
    sprintf(data,da);
    char pkey[9];
    char piv[9];
   sprintf(pkey, key);
   sprintf(piv,iv);
   des_setparity(pkey);
   stat = cbc_crypt(pkey, data, strlen(data), DES_DECRYPT|DES_SW,piv);
    printf("%d,%s\n",stat, data);
    showbuf(data,strlen(data)+8);
   //user password crypt
    printf("code:%s\n",crypt("123456", "th9hLv8/RSRa."));
    /*
    * echo -n 04fMaWegkH1/BL9CNYxgusFpYK8wdraBX06mPiRmxJP+uVm31GQvyw== | base64 -d | ./des
    * should be "cea3e8e1659582206e0be32539729e9f"
    */
    //echo -n gJH2S09kL/dx9qr9KM7Ngw== | base64 -d | ./des
    return 0;
}
