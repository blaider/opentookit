/*************************************************************************
	> File Name: libcurltest.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 07 Dec 2016 02:48:12 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/time.h>
#include "base64.h"

char base64Temp[2048];
char m_strSendName[1024];
char m_strCharset[1024];
char m_strFileName[1024];
void SetSendName(const char * sendname)
{
	char tem[2048];
	memset(base64Temp,0,sizeof(base64Temp));
	sprintf(tem,"=?%s?B?%s?=",m_strCharset,base64_encode(sendname,base64Temp,strlen(sendname)));
	strcpy(m_strSendName,tem);
//	std::string strTemp = "";
//	strTemp += "=?";
//	strTemp += m_strCharset;
//	strTemp += "?B?";
//	strTemp += base64_encode((unsigned char *)sendname.c_str(), sendname.size());
//	strTemp += "?=";
//	m_strSendName = strTemp;
}
void SetFileName(const char * FileName)
{
	char tem[2048];
		memset(base64Temp,0,sizeof(base64Temp));
		sprintf(tem,"=?%s?B?%s?=",m_strCharset,base64_encode(FileName,base64Temp,strlen(FileName)));
		strcpy(m_strFileName,tem);
//	std::string EncodedFileName = "=?";
//	EncodedFileName += m_strCharset;
//	EncodedFileName += "?B?";//修改
//	EncodedFileName += base64_encode((unsigned char *)FileName.c_str(), FileName.size());
//	EncodedFileName += "?=";
//	m_strFileName = EncodedFileName;
}
char m_strSubject[1024];
void SetSubject(const char * subject)
{
	char tem[2048];
	memset(base64Temp,0,sizeof(base64Temp));
	sprintf(tem,"Subject: =?%s?B?%s?=",m_strCharset,base64_encode(subject,base64Temp,strlen(subject)));
	strcpy(m_strSubject,tem);
//	std::string strTemp = "";
//	strTemp = "Subject: ";
//	strTemp += "=?";
//	strTemp += m_strCharset;
//	strTemp += "?B?";
//	strTemp += base64_encode((unsigned char *)subject.c_str(), subject.size());
//	strTemp += "?=";
//	m_strSubject = strTemp;
}

int getFileType( const char * stype)
{
	if (!strcmp(stype,"txt"))
	{
		return 0;
	}
	else if (!strcmp(stype,"xml"))
	{
		return 1;
	}
	else if (!strcmp(stype,"html"))
	{
		return 2;
	}
	else if (!strcmp(stype,"jpeg"))
	{
		return 3;
	}
	else if (!strcmp(stype,"png"))
	{
		return 4;
	}
	else if (!strcmp(stype,"gif"))
	{
		return 5;
	}
	else if (!strcmp(stype,"exe"))
	{
		return 6;
	}

	return -1;
}

char * m_strContentType = NULL;
void SetContentType(const char * stype)
{
	int type = getFileType(stype);
	switch (type)
	{//
	case 0:
		m_strContentType = "plain/text;";
		break;

	case 1:
		m_strContentType = "text/xml;";
		break;

	case 2:
		m_strContentType = "text/html;";

	case 3:
		m_strContentType = "image/jpeg;";
		break;

	case 4:
		m_strContentType = "image/png;";
		break;

	case 5:
		m_strContentType = "image/gif;";
		break;

	case 6:
		m_strContentType = "application/x-msdownload;";
		break;

	default:
		m_strContentType = "application/octet-stream;";
		break;
	}
}

void GetFileName(const char *file, char * filename)
{

	char *pslash;
	pslash = rindex(file, '/');
	if(pslash == NULL)
		pslash = (char *)file;
	strcpy(filename,pslash);
	printf("%s\n", filename);
}

void GetFileType(const char * file, char * stype)
{
	char *p;
	p = rindex(file, '.');
	printf("%s\n", p);
	strcpy(stype,p+1);
}

char m_strMessage[4096*10];

int CreatMessage()
{

	int offset = 0;

//	sprintf(m_strCharset,"UTF-8");
	SetSubject("test multi mime");

	memset(m_strMessage,0,sizeof(m_strMessage));
	offset += sprintf(m_strMessage+offset,"From: <suchao.wang@advantech.com.cn>");
//	m_strMessage += m_strSendMail;
	offset += sprintf(m_strMessage+offset,"\r\nReply-To: <blaider@qq.com>");
//	m_strMessage += m_strSendMail;
	offset += sprintf(m_strMessage+offset, "\r\nTo: <blaider@qq.com>");
//	for (size_t i = 0; i < m_vRecvMail.size(); i++)
//	{
//		if (i > 0) {
//			m_strMessage += ",";
//		}
//		m_strMessage += m_vRecvMail[i];
//	}


	offset += sprintf(m_strMessage+offset,"\r\n");
	offset += sprintf(m_strMessage+offset,m_strSubject);
	offset += sprintf(m_strMessage+offset, "\r\nX-Mailer: The Bat! (v3.02) Professional");
	offset += sprintf(m_strMessage+offset, "\r\nMime-Version: 1.0");
	offset += sprintf(m_strMessage+offset, "\r\nContent-Type: multipart/mixed;");
	offset += sprintf(m_strMessage+offset, "boundary=\"simple boundary\"");
	offset += sprintf(m_strMessage+offset, "\r\nThis is a multi-part message in MIME format.");
	offset += sprintf(m_strMessage+offset, "\r\n--simple boundary");
	//正文
	offset += sprintf(m_strMessage+offset,"\r\nContent-Type: text/html;");
	offset += sprintf(m_strMessage+offset,"charset=");
	offset += sprintf(m_strMessage+offset,"\"");
	offset += sprintf(m_strMessage+offset,"%s","gb2312");
	offset += sprintf(m_strMessage+offset,"\"");
	offset += sprintf(m_strMessage+offset,"\r\nContent-Transfer-Encoding: 7BIT");
	offset += sprintf(m_strMessage+offset,"\r\n\r\n");

	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	offset += sprintf(m_strMessage+offset,"%lf",ts.tv_sec+1.0*ts.tv_nsec/(1000*1000*1000));
	offset += sprintf(m_strMessage+offset,"\r\n\r\n");

	printf("%s",m_strMessage);
	//附件
	char filename[1024];
	char filetype[1024];
//	for (size_t i = 0; i < m_vAttachMent.size(); i++)
	FILE *fp = fopen("data.txt", "w+");
	fwrite(m_strMessage,offset,1,fp);
	offset = 0;
	do
#define AFILE "b.txt"
	{
		offset += sprintf(m_strMessage+offset,"\r\n--simple boundary");
		GetFileName(AFILE, filename);
		GetFileType(AFILE, filetype);
		SetContentType(filetype);
		SetFileName(filename);

		offset += sprintf(m_strMessage+offset,"\r\nContent-Type: ");
		offset += sprintf(m_strMessage+offset, "%s",m_strContentType);
		offset += sprintf(m_strMessage+offset, "\tname=");
		offset += sprintf(m_strMessage+offset, "\"");
		offset += sprintf(m_strMessage+offset, "%s",m_strFileName);
		offset += sprintf(m_strMessage+offset, "\"");
		offset += sprintf(m_strMessage+offset, "\r\nContent-Disposition:attachment;filename=");
		offset += sprintf(m_strMessage+offset, "\"");
		offset += sprintf(m_strMessage+offset, "%s",m_strFileName);
		offset += sprintf(m_strMessage+offset, "\"");
		offset += sprintf(m_strMessage+offset, "\r\nContent-Transfer-Encoding:base64");
		offset += sprintf(m_strMessage+offset, "\r\n\r\n");


		printf("%s",m_strMessage);
		fwrite(m_strMessage,offset,1,fp);
		offset = 0;
		FILE *pt = NULL;
		if ((pt = fopen(AFILE, "rb")) == NULL) {

			continue;
		}
		fseek(pt, 0, SEEK_END);
		int len = ftell(pt);
		fseek(pt, 0, SEEK_SET);
		int rlen = 0;
		char buf[55];
		size_t i = 0;
		for ( i = 0; i < len / 54 + 1; i++)
		{
			memset(buf, 0, 55);
			rlen = fread(buf, sizeof(char), 54, pt);
			memset(base64Temp,0,sizeof(base64Temp));
			offset += sprintf(m_strMessage+offset,"%s",base64_encode((const unsigned char*)buf,base64Temp,rlen));
			offset += sprintf(m_strMessage+offset, "\r\n");

			if(offset >= 1024)
			{
//				printf("%s",m_strMessage);
				fwrite(m_strMessage,offset,1,fp);
				offset = 0;
			}
		}
		fclose(pt);
		pt = NULL;

	}while(0);
	offset += sprintf(m_strMessage+offset,"\r\n--simple boundary\r\n");
	offset += sprintf(m_strMessage+offset, "\r\n.\r\n");
	printf("%s",m_strMessage);

	fwrite(m_strMessage,offset,1,fp);
	fclose(fp);

}

size_t read_data(void *ptr, size_t size, size_t nmemb, void *data)
{
    FILE *fp = (FILE *)data;
    size_t return_size = fread(ptr, size, nmemb, data);
    printf("size:%d,nmem:%d,read %d\n",size, nmemb, (int)return_size);
//    if(return_size > 0)
//    {
//    	sprintf(return_size+ptr,"\r\n.\r\n");
//    	return_size+=5;
//    }
//    sprintf(ptr+return_size,"\r\n");
    return return_size;// +2;

}
int main()
{


	CreatMessage();

    CURL *curl;
    CURLcode res;

    FILE *fp = fopen("data.txt", "rb");
    if (fp == NULL) {
        printf("can't open \n");
        return -1;
    }
    struct curl_slist *slist=NULL;

    curl = curl_easy_init();
    if(curl) {
    	curl_easy_setopt( curl, CURLOPT_USE_SSL, (long) CURLUSESSL_ALL );
    	curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0L );
		curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 0L );
//		curl_easy_setopt( curl, CURLOPT_TIMEOUT, mail->timeout );
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt( curl, CURLOPT_UPLOAD, 1L );
//        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
//        curl_easy_setopt(curl, CURLOPT_FTPSSLAUTH, CURLFTPAUTH_SSL);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<suchao.wang@advantech.com.cn>");    //发送者
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://mail.advantech.com.cn:587");
        slist = curl_slist_append(slist, "blaider@qq.com");  //接收者
//        slist = curl_slist_append(slist, "wschao2002@163.com");
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, slist);
        curl_easy_setopt(curl, CURLOPT_USERNAME, "suchao.wang");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "bjsuchao");
        curl_easy_setopt(curl, CURLOPT_READDATA, fp);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_data);

    }
    res = curl_easy_perform(curl);

    printf("The return code is %d\n", res);

    fclose(fp);
    curl_slist_free_all(slist);
    curl_easy_cleanup(curl);

    return 0;
}

