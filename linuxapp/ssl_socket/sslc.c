#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <signal.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#define FAIL    -1
int OpenConnection(const char*hostname, int port)
{
	int sd;
	struct hostent *host;
	struct sockaddr_in addr;
	if ((host = gethostbyname(hostname)) == NULL)
	{
		printf("Eroor: %s\n", hostname);
		perror(hostname);
		abort();
	}
	sd = socket(PF_INET, SOCK_STREAM, 0);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = *(long*) (host->h_addr);
	if (connect(sd, (struct sockaddr*) &addr, sizeof(addr)) != 0)
	{
		close(sd);
		perror(hostname);
		abort();
	}
	return sd;
}
SSL_CTX* InitCTX(void)
{
	SSL_METHOD *method;
	SSL_CTX *ctx;
	OpenSSL_add_all_algorithms(); /* Load cryptos, et.al. */
	SSL_load_error_strings(); /* Bring in and register error messages */
	method = SSLv23_client_method(); /* Create new client-method instance */
	ctx = SSL_CTX_new(method); /* Create new context */
	if (ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
//		printf("Eroor: %s\n", stderr);
		abort();
	}
	return ctx;
}
void ShowCerts(SSL* ssl)
{
	X509 *cert;
	char*line;
	cert = SSL_get_peer_certificate(ssl);/* get the server's certificate */
	if (cert != NULL)
	{
		printf("Server certificates:\n");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		printf("Subject: %s\n", line);
		free(line); /* free the malloc'ed string */
		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		printf("Issuer: %s\n", line);
		free(line); /* free the malloc'ed string */
		X509_free(cert); /* free the malloc'ed certificate copy */
	}
	else
		printf("No certificates.\n");
}

void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
	SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL);
//New lines
	if (SSL_CTX_load_verify_locations(ctx, CertFile, NULL) != 1)
		ERR_print_errors_fp(stderr);
	if (SSL_CTX_set_default_verify_paths(ctx) != 1)
		ERR_print_errors_fp(stderr);
	//End new lines

	/* set the local certificate from CertFile */
	if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		abort();
	}
	/* set the private key from KeyFile (may be the same as CertFile) */
	if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
		abort();
	}
	/* verify private key */
	if (!SSL_CTX_check_private_key(ctx))
	{
		fprintf(stderr, "Private key does not match the public certificate\n");
		abort();
	}
	printf("LoadCertificates Compleate Successfully.....\n");
}
SSL *ssl;

void sig_int(int sig)
{
	SSL_free(ssl); /* release connection state */

}

int main( int count, char*strings[])
{
	SSL_CTX *ctx;
	int server;

	char buf[1024];
	int bytes;
	char*hostname, *portnum;
	if (count != 3)
	{
		printf("usage: %s <hostname> <portnum>\n", strings[0]);
		exit(0);
	}
	signal(SIGINT,sig_int);
	SSL_library_init();
	hostname = strings[1];
	portnum = strings[2];
	ctx = InitCTX();


//	LoadCertificates(ctx, "cacert.pem",
//			"privkey.pem"); /* load certs */


	server = OpenConnection(hostname, atoi(portnum));
	ssl = SSL_new(ctx); /* create new SSL connection state */
	SSL_set_fd(ssl, server); /* attach the socket descriptor */
	if (SSL_connect(ssl) == FAIL) /* perform the connection */
	{
//		printf('Eroor: %s\n', stderr);
		ERR_print_errors_fp(stderr);
	}
	else
	{
		int i = 0;
		char msg[] = {0x02,0xaa,0x97,0x00,0x01,0x0,0x0,0x0,0x0,0x0,0x44,0x03};
		printf("Connected with %s encryption\n", SSL_get_cipher(ssl));
		ShowCerts(ssl); /* get any certs */
		SSL_write(ssl, msg, sizeof(msg)); /* encrypt & send message */
//		SSL_shutdown(ssl);
//		SSL_free(ssl);
		while(0)
		{
			bytes = SSL_read(ssl, buf, sizeof(buf));/* get reply & decrypt */
			if(bytes > 0)
			{
				buf[bytes] = 0;
				printf("Received:%d\n",bytes);
				for(i = 0;i<bytes;i++)
					printf("%02x ",buf[i]&0xff);
				printf("\n");
			}else if(bytes < 0)
			{
				ERR_print_errors_fp(stderr);
				break;
			}
			sleep(1);
			SSL_write(ssl, msg, sizeof(msg)); /* encrypt & send message */
		}
		SSL_free(ssl); /* release connection state */
	}
	close(server); /* close socket */
	SSL_CTX_free(ctx); /* release context */
	return 0;
}
