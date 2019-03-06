/*************************************************************************
	> File Name: systemresult.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Fri 18 Nov 2016 01:38:53 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
    pid_t status;


    status = system("test.sh");

    if (-1 == status)
    {
        printf("system error!");
    }
    else
    {
        printf("exit status value = [0x%x]\n", status);

        if (WIFEXITED(status))
        {
            if (0 == WEXITSTATUS(status))
            {
                printf("run shell script successfully.\n");
            }
            else
            {
                printf("run shell script fail, script exit code: %d\n", WEXITSTATUS(status));
            }
        }
        else
        {
            printf("exit status = [%d]\n", WEXITSTATUS(status));
        }
    }

    return 0;
}



/*
//return 0 for success else for failed
int check_zipfile(const char *filename)
{
	char cmd[PATH_MAX] = {0};
	pid_t status;

	if(access(filename,F_OK))
		return -1;

	sprintf(cmd,"unzip -l %s",filename);

	status = system(cmd);

	if (-1 == status)
	{
		printf("system error!");
		return -1;
	}

	if (WIFEXITED(status))
	{
		if (0 == WEXITSTATUS(status))
		{
			return 0;
		}
		else
		{
			printf("run shell script fail, script exit code: %d\n", WEXITSTATUS(status));
			return -1;
		}
	}
	else
	{
		printf("exit status = [%d]\n", WEXITSTATUS(status));
		return -1;
	}
	return 0;
}
if(check_zipfile(filename))
{
	print_app_version();
	print_app_usage();
	showMessage("Incorrect zip file[%s]",filename);
	return -4;
}
*/
