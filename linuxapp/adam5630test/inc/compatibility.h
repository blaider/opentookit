#ifndef _WIN32
#include   <unistd.h>
#include   <string.h>
#else
#include <conio.h>
#pragma warning( disable: 4996 )
#endif


#define CHK_RESULT(ret) {if(BioFailed(ret))break;}


#if !defined(_WIN32)
#  define SLEEP(second)  sleep(second)
inline int kbhit(void)
{
   struct timeval tv = {0};
   fd_set rdfs;
   FD_ZERO(&rdfs);
   FD_SET (STDIN_FILENO, &rdfs);
   select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
   return FD_ISSET(STDIN_FILENO, &rdfs);
}
#else
#define SLEEP(second)  Sleep(second*1000)
#endif


