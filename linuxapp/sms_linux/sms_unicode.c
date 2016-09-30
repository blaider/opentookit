#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#include <string.h>


int showPdu(char *content)
{

	char * iconv_dst = NULL;
	iconv_t iconv_fd;
	ssize_t iconv_rc = 0;
	size_t iconv_src_len = 0;
	size_t iconv_dst_len = 0;
	unsigned char code[1024];
	char iconv_dst_buf[1024];

	memset( iconv_dst_buf, 0, sizeof( iconv_dst_buf ) );
	iconv_dst = iconv_dst_buf;
	iconv_src_len = strlen( content )/2;
	iconv_dst_len = sizeof( iconv_dst_buf );
	iconv_fd = iconv_open(  "UTF-8","UCS-2BE");

	int i=0;
	memset(code,0,sizeof(code));

	for(i=0;i<iconv_src_len;i++)
	{
		char tmp[4];
		memset(tmp,0,sizeof(tmp));
		memcpy(tmp,content+i*2,2);
		code[i] = strtoul(tmp,NULL,16)&0xff;
	}
	char *p = code;

	iconv_rc = iconv(
			iconv_fd,
			&p,
			&iconv_src_len,
			&iconv_dst,
			&iconv_dst_len );
		if ( -1 == iconv_rc )
		{
			return 0;
		}
	printf("%s\n",iconv_dst_buf);
	iconv_close( iconv_fd );
	return 0;
}
int main()
{
	showPdu("5bb691cc7740706b0021");
	return 0;
}
