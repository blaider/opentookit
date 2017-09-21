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
// File:   	 aes_test.c
// Author:  suchao.wang
// Created: Oct 27, 2016
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include <stdint.h>


//static unsigned char aes_key[AES_BLOCK_SIZE] = {0xE7,0x5A,0x60,0x5E,0x83,0x89,0x4B,0x43,0x64,0x38,0xCC,0x40,0xAF,0xD5,0xB3,0x3C};
//static char defaltAESpass[]= {0xe8,0xc8,0x02,0xea,0x07,0xe3,0x1d,0x99,0xec,0x09,0x06,0x66,0xc8,0x5c,0x47,0xa5};

//static unsigned char btKey[] = { 93, 30, 248, 134, 147, 238, 45, 98, 166, 213, 200, 219, 152, 78, 151, 172, 163, 207, 93, 149, 92, 246, 81, 60, 227, 148, 143, 194, 89, 19, 145, 100 };
//static unsigned char btIV[] = { 251, 249, 237, 13, 161, 251, 105, 42, 255, 132, 161, 124, 56, 210, 124, 185 };

//trans
//static unsigned char btKey[] = { 5,115,210,19,110,244,221,42,139,67,130,111,185,156,45,151,224,106,195,243,181,93,170,138,251,159,208,254,31,65,226,35 };
//static unsigned char btIV[] = { 7,61,84,73,221,82,156,91,158,143,188,46,217,173,59,198 };

//zip file
//static unsigned char btKey[] = { 123,69,17,90,212,234,99,40,58,235,134,27,130,210,168,129,142,245,153,14,170,127,65,61,210,90,105,155,42,97,138,59 };
//static unsigned char btIV[] = { 75,155,184,57,199,184,46,101,197,221,155,203,25,119,157,80 };

//default for 3600 local
//static unsigned char btKey[] = { 65,180,93,89,193,40,109,120,238,136,248,178,30,182,91,253,210,63,240,158,161,96,71,195,208,68,188,108,184,9,205,173 };
//static unsigned char btIV[] = { 200,18,107,177,0,45,89,72,121,103,233,180,113,12,46,243 };  //iv变量使用后会变化，使用时请注意

//default for sysnet
//static unsigned char btKey[] = { 68,233,172,219,119,104,90,165,137,231,59,26,56,38,13,17,186,26,37,87,55,122,225,128,96,78,130,144,155,16,102,89 };
//static unsigned char btIV[] = { 91,86,144,91,132,12,188,176,221,226,6,221,99,57,133,52 };  //iv变量使用后会变化，使用时请注意


//TMS10 用于TagLink Studio工程下载的
//static unsigned char btKey[] = [213,231,206,95,57,1,181,185,209,108,96,50,0,147,154,235,224,230,228,166,215,243,52,47,252,0,244,109,65,90,183,44]
//static unsigned char btIV[] = [244,112,3,140,208,39,222,36,235,19,48,181,210,191,150,112]

//TMS10 用于3600上密码保存的
static unsigned char btKey[] = {230,32,129,144,48,79,95,47,254,38,102,170,71,63,159,190,123,165,60,244,22,105,243,178,146,37,223,34,184,95,193,208};
static unsigned char btIV[]  = {65,250,175,22,203,73,168,63,87,121,202,47,148,189,206,84};



static char defaltAESpass[]=  {0xa2,0x29,0x84,0xbe,0xf8,0xcc,0x8b,0xe7,0x34,0x6a,0x60,0xd5,0xd3,0x29,0x4a,0x96};

int aes_encrypt(char *data) {
    AES_KEY aes;
//    unsigned char key[AES_BLOCK_SIZE];        // AES_BLOCK_SIZE = 16
    unsigned char iv[AES_BLOCK_SIZE];        // init vector
    unsigned char* input_string;
    unsigned char* encrypt_string;
    unsigned int len;        // encrypt length (in multiple of AES_BLOCK_SIZE)
    unsigned int i;

    // set the encryption length
    len = 0;
    if ((strlen(data) + 1) % AES_BLOCK_SIZE == 0) {
        len = strlen(data) + 1;
    } else {
        len = ((strlen(data) + 1) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
    }

    // set the input string
    input_string = (unsigned char*)calloc(len, sizeof(unsigned char));
    if (input_string == NULL) {
        fprintf(stderr, "Unable to allocate memory for input_string\n");
        exit(-1);
    }
    memset(input_string,0,len);
    strncpy((char*)input_string, data, strlen(data));

//    // Generate AES 128-bit key
//    for (i=0; i<16; ++i) {
//        key[i] = 32 + i;
//    }
    memcpy(iv,btIV,sizeof(btIV));

    // Set encryption key
//    for (i=0; i<AES_BLOCK_SIZE; ++i) {
//        iv[i] = 0;
//    }
    if (AES_set_encrypt_key(btKey, 256, &aes) < 0) {
        fprintf(stderr, "Unable to set encryption key in AES\n");
        exit(-1);
    }

    // alloc encrypt_string
    encrypt_string = (unsigned char*)calloc(len, sizeof(unsigned char));
    if (encrypt_string == NULL) {
        fprintf(stderr, "Unable to allocate memory for encrypt_string\n");
        exit(-1);
    }

    memset(encrypt_string,0,len);
    // encrypt (iv will change)
    AES_cbc_encrypt(input_string, encrypt_string, len, &aes, iv, AES_ENCRYPT);


    memcpy(data,encrypt_string,len);
    // print
     printf("input_string = %s\n", input_string);
     printf("encrypted string = ");
     for (i=0; i<len; ++i) {
         printf("%x%x", (encrypt_string[i] >> 4) & 0xf,
                 encrypt_string[i] & 0xf);
     }
     printf("\n");
     free(encrypt_string);
   free(input_string);
    return len;
}

int aes_decrypt(char *data,int len) {
    AES_KEY aes;
//    unsigned char key[AES_BLOCK_SIZE];        // AES_BLOCK_SIZE = 16
    unsigned char iv[AES_BLOCK_SIZE];        // init vector
    unsigned char* input_string;
    unsigned char* decrypt_string;
          // encrypt length (in multiple of AES_BLOCK_SIZE)
    unsigned int i;
//    unsigned int len;

//    len = 0;
//   if ((strlen(data) + 1) % AES_BLOCK_SIZE == 0) {
//	   len = strlen(data) + 1;
//   } else {
//	   len = ((strlen(data) + 1) / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
//   }
    // set the input string
    input_string = (unsigned char*)calloc(len, sizeof(unsigned char));
    if (input_string == NULL) {
        fprintf(stderr, "Unable to allocate memory for input_string\n");
        exit(-1);
    }
    memset(input_string,0,len);
    memcpy((char*)input_string, data, len);

//    // Generate AES 128-bit key
//    for (i=0; i<16; ++i) {
//        key[i] = 32 + i;
//    }

    // Set encryption key
//    for (i=0; i<AES_BLOCK_SIZE; ++i) {
//        iv[i] = 0;
//    }
    memcpy(iv,btIV,sizeof(btIV));
    if (AES_set_decrypt_key(btKey, 256, &aes) < 0) {
        fprintf(stderr, "Unable to set encryption key in AES\n");
        exit(-1);
    }

    // alloc encrypt_string
    decrypt_string = (unsigned char*)calloc(len, sizeof(unsigned char));
    if (decrypt_string == NULL) {
        fprintf(stderr, "Unable to allocate memory for decrypt_string\n");
        exit(-1);
    }

    memset(decrypt_string,0,len);
    // decrypt (iv will change)
    AES_cbc_encrypt( input_string, decrypt_string,len, &aes, iv,
            AES_DECRYPT);

    strcpy(data,(char *)decrypt_string);
    // print

    printf("input_string = ");
    for (i=0; i<len; ++i) {
        printf("%x%x", (input_string[i] >> 4) & 0xf,
        		input_string[i] & 0xf);
    }
    printf("\n");
    printf("input_string = ");
   for (i=0; i<len; ++i) {
	   printf("%x%x", (decrypt_string[i] >> 4) & 0xf,
			   decrypt_string[i] & 0xf);
   }
   printf("\n");
    printf("decrypt_string = %s\n", decrypt_string);

    free(decrypt_string);
    free(input_string);
    return 0;
}

const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char * base64_encode( const unsigned char * bindata, char * base64, int binlength )
{
    int i, j;
    unsigned char current;

    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    return base64;
}

int base64_decode( const char * base64, unsigned char * bindata )
{
    int i, j;
    unsigned char k;
    unsigned char temp[4];
    for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
    {
        memset( temp, 0xFF, sizeof(temp) );
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) |
                ((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) |
                ((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) |
                ((unsigned char)(temp[3]&0x3F));
    }
    return j;
}



static char encoding_table[] =
{
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
};

static char *decoding_table = NULL;

static void build_decoding_table()
{
	int i;

	decoding_table = (char*) malloc(256);

	for (i = 0; i < 64; i++)
		decoding_table[(unsigned char) encoding_table[i]] = i;
}

static unsigned char *base64_decodes(
	const unsigned char *data,
	size_t input_length,
	size_t *output_length)
{
	unsigned char *decoded_data;
	size_t i;
	size_t j;

	if (decoding_table == NULL) build_decoding_table();

	if (input_length % 4 != 0) return NULL;

	*output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=') (*output_length)--;
	if (data[input_length - 2] == '=') (*output_length)--;

	decoded_data = (unsigned char *) malloc(*output_length);
	if (decoded_data == NULL) return NULL;

	for (i = 0, j = 0; i < input_length;) {

		uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}

	return decoded_data;
}


static void base64_cleanup()
{
	free(decoding_table);
	decoding_table = NULL;
}

int main(int argc,char *argv[])
{
//	char data[] = {0xc3,0x75,0x0c,0x1b,0x83,0x8c,0x42,0x9a,0xf9,0x1a,0x62,0x73,0x1c,0x55,0x33,0x88};
//	aes_decrypt(data,sizeof(data));

	int len = 0;
	char data[1024];
	memset(data,0,sizeof(data));
	if(argc == 2 )
		strcpy(data,argv[1]);
	else
		strcpy(data,"abcde");
	len = aes_encrypt(data);
	printf("len:%d\n",len);
	char base64[1024];
	memset(base64,0,sizeof(base64));
	base64_encode(data,base64,len);
	printf("base64:%s\n",base64);
	aes_decrypt(data,len);


	unsigned int decoded_len = 0;
	unsigned char * decoded = base64_decodes( base64, strlen((char *)base64), &decoded_len );

	memset(data,0,sizeof(data));
	memcpy(data,decoded,decoded_len);
	aes_decrypt(data,decoded_len);
	base64_cleanup();
	free(decoded);

}

