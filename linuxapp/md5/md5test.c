#include <stdio.h>
#include <string.h>

#include "md5.h"

int main(int argc, char **argv)
{
	struct MD5Context md5c;
	char *s;
	unsigned char ss[16];
	char buf[33] =	{ '\0' };
	char tmp[3] =	{ '\0' };
	int i;

	if (2 != argc)
	{
		printf("usage :\tmd5 <string>\n");
		return 1;
	}
	s = argv[1];

	MD5Init(&md5c);
	MD5Update(&md5c, s, strlen(s));
	MD5Final(ss, &md5c);

	for (i = 0; i < 16; i++)
	{
		sprintf(tmp, "%02X", ss[i]);
		strcat(buf, tmp);
	}
	printf("%s", buf);
	printf("\t%s\n", s);

	return 0;
}
