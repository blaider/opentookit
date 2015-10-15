#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>

#define CP_OK 0
#define CP_ERR_PARAMETER 1
#define CP_ERR_NO_OLDPASSWD 2
#define CP_ERR_NO_NEWPASSWD 3
#define CP_ERR_PIPE 3
#define CP_ERR_FORK 4
#define CP_ERR_DUP 5
#define CP_ERR_WRONG_PASSWD 6
#define CP_ERR_SIMPLE_CHK 7
#define CP_ERR_NO_MATCH 8
#define CP_ERR_OTHER 9

int main(int argc, char *argv[])
{

	int fd_w[2],fd_r[2],ret;
	char buf[256];
	char prompt_1[]="New password:";
	char prompt_2[]="Re-enter new password:";
	char prompt_3[]="(current) UNIX password:";
	char prompt_4[]="passwd: Authentication token manipulation error";
	char prompt_5[]="BAD PASSWORD";
	char prompt_6[]="Sorry, passwords do not match";

	int i=0;
	pid_t pid;
	fd_set readset;

	uid_t userid;
	char new_password[256]={0};
	char old_password[256]={0};
	char user_name[256]={0};
	struct timeval tv;

	int ch;
	while((ch = getopt(argc,argv,"n:o:u:h"))!= -1)
	{
		switch(ch)
		{
			case 'n':
				strncpy(new_password,optarg,30);
				strncat(new_password,"\n",1);
				break;
			case 'o':
				strncpy(old_password,optarg,30);
				strncat(old_password,"\n",1);
				break;
			case 'u':
				strncpy(user_name,optarg,30);
				break;
			default:
				printf("usage:%s -n new_password [ -o old_password :is needs if not root ] [ -u username :can use only root ]\n",argv[0]);
				return CP_ERR_PARAMETER;
		}
	}

	if(strlen(new_password) == 0)
	{
		printf("you must specify a new password by parameter -n\n"); 
		printf("usage:%s -n new_password [ -o old_password :is needs if not root ] [ -u username :can use only root ]\n",argv[0]);
		return CP_ERR_NO_NEWPASSWD;
	}
	
	userid = getuid();
	if(userid != 0)/*not root*/
	{
		if(strlen(old_password) == 0)
		{
			printf("the current password is needs\n");
			return CP_ERR_NO_OLDPASSWD;
		}
	}
	
	FD_ZERO(&readset);
	if(pipe(fd_w) != 0)
	{
		perror("pipe");
		return CP_ERR_PIPE;
	}

	if(pipe(fd_r) != 0)
	{
		perror("pipe");
		return CP_ERR_PIPE;
	}


	FD_SET(fd_w[0],&readset);
	pid=fork();
	if(pid < 0)
	{
		perror("fork");
		return CP_ERR_FORK;
	}
	else if(pid==0)/*child*/
	{

		if(dup2(fd_r[0], STDIN_FILENO) != STDIN_FILENO)
		{
			perror("dup2");
			return CP_ERR_DUP;
		}

		if(dup2(fd_w[1], STDERR_FILENO) != STDERR_FILENO)
		{
			perror("dup2");
			return CP_ERR_DUP;
		}

		close(fd_r[1]);
		close(fd_w[0]);
		close(STDOUT_FILENO);
		if(strlen(user_name))
		{
			execl("/usr/bin/passwd","passwd",user_name,(char *)0);	
		}
		else
		{
			execl("/usr/bin/passwd","passwd",(char *)0);
		}
	}
	else/*parent*/
	{

		close(fd_r[0]);
		close(fd_w[1]);
		/* Wait up to 3 seconds. */
		tv.tv_sec = 3;
		tv.tv_usec = 0;	   
		for(;;)
		{
			memset(buf,0,256);
			ret=select(fd_w[0]+1,&readset,NULL,NULL,&tv);
			if(ret < 0)
			{
				return CP_ERR_OTHER;
			}

			if(FD_ISSET(fd_w[0],&readset))
			{
				read(fd_w[0],buf,256);
			}
			else
			{
				return CP_ERR_OTHER;
			}

			if(strlen(buf))
			{
				if(userid != 0)
				{
					
					if (strstr(buf,prompt_3))
					{
						write(fd_r[1],old_password,strlen(old_password));
					}
					if(strstr(buf,prompt_4))
					{    
						return CP_ERR_WRONG_PASSWD; 
					}
					if( strstr(buf,prompt_5))
					{
						return CP_ERR_SIMPLE_CHK;
					}	
					if(strstr(buf,prompt_6))
					{
						return CP_ERR_NO_MATCH;
					}


				}
				if(strstr(buf,prompt_1) || strstr(buf,prompt_2))
				{
					write(fd_r[1],new_password,strlen(new_password));
					i++;
					if(i==2)
					{
						return CP_OK;
					}
				}
			}
		}
	}
}
