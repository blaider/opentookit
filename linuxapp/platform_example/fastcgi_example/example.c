#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <locale.h>
#include <dirent.h>
#include <fcntl.h>
#include <shadow.h>
#include <dlfcn.h>
#include <unistd.h>

#include "fastcgi.h"
#include "fcgios.h"
#include "fcgi_stdio.h"

#define STANDALONE_MODE					1

#ifndef FCGI_SOCKET
#define FCGI_SOCKET						"/tmp/ext.fastcgi.socket"
#endif


#if STANDALONE_MODE
static int stdinFds[3];
#endif

static void handle_request (
	char * request_uri,
	char * query_string,
	char * request_method,
	char * http_accept,
	char * http_user_agent,
	char * http_accept_encoding,
	char * http_accept_language )
{

	if ( strcmp( request_method, "GET" ) != 0 )
	{
		printf( "Status: 403 Forbidden\r\nContent-Type: text/plain\r\n\r\nInvalid Request" );
		printf( "{\"HTTP\":\"403\"}" );
		return;
	}

	if ( strstr( request_uri, "/ext" ) == NULL )
	{
		printf( "Status: 403 Forbidden\r\nContent-Type: text/plain\r\n\r\nIncorrect URI" );
		printf( "{\"HTTP\":\"403\"}" );
		return;
	}
	
	printf( "Status: 200 OK\r\nContent-Type: application/json\r\n\r\n" );
	printf( "{\r\n" );
	printf( "this is a fastcgi example\r\n" );
	printf( "}\r\n" );
}

int main ( int argc, char * argv[] )
{
	int rc;
    int listen_fd;

    daemon(0,0);

#if STANDALONE_MODE
	rc = OS_LibInit( stdinFds );
	if ( rc != 0 )
    {
		printf( "Error initializing OS library: %d\n", rc );
		return -1;
    }

	if ( ( listen_fd = OS_CreateLocalIpcFd( FCGI_SOCKET, 5 ) ) == -1 )
	{
		printf( "OS_CreateLocalIpcFd failed\n" );
		return -2;
	}

	chmod( FCGI_SOCKET, ACCESSPERMS );

	close( STDIN_FILENO );
	if ( listen_fd != FCGI_LISTENSOCK_FILENO )
	{
		dup2( listen_fd, FCGI_LISTENSOCK_FILENO );
		close( listen_fd );
	}

	close( STDOUT_FILENO );
	close( STDERR_FILENO );
#endif

	while ( FCGI_Accept() >= 0 )
	{
		handle_request(
			getenv( "REQUEST_URI" ),
			getenv( "QUERY_STRING" ),
			getenv( "REQUEST_METHOD" ),
			getenv( "HTTP_ACCEPT" ),
			getenv( "HTTP_USER_AGENT" ),
			getenv( "HTTP_ACCEPT_ENCODING" ),
			getenv( "HTTP_ACCEPT_LANGUAGE") );
	}

	return 0;
}
