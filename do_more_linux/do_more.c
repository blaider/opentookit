/*************************************************************************
	> File Name: do_more.c
	> Author: suchao.wang
	> Mail: suchao.wang@advantech.com.cn 
	> Created Time: Wed 03 Dec 2014 05:10:43 PM CST
 ************************************************************************/

#include<stdio.h>

#include<stdio.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<termios.h>
void gettermsize(int *w,int *h);     //获取终端尺寸
void do_more(FILE *);   //根据see_more获取的用户输入，显示更多的文件内容或退出
int see_more();   //捕获用户的输入指令
void settermattr();    //设置终端属性
int width;      //终端屏幕宽度
int height;     //终端屏幕高度
int filesize;   //文件的大小
int readsize;   //已经显示的内容长度
struct termios ts,ots;     //终端属性
int main(int argc,char *argv[]){
    gettermsize(&width,&height);
    settermattr();
    FILE *fp;
    struct stat *buf;
    filesize=0;
    readsize=0;
    buf=(struct stat*)malloc(sizeof(struct stat));
    memset(buf,0x00,sizeof(struct stat));
    if(argc==1)     //如果没有指定文件，则从标准输入读取内容
        do_more(stdin);
    else{        //从文件列表中读取内容
        int i;
        for(i=1;i<argc;i++){       //more命令后面可能跟多个文件
            if((fp=fopen(*(argv+i),"r"))!=0){     //以只读形式打开文件
                if(lstat((const char*)*(argv+i),buf)==0){   //获取文件状态
                    filesize+=buf->st_size;     //获取文件大小
                    printf("fs=%ld\n",buf->st_size);
                }
                else{
                    exit(1);
                }
                fclose(fp);      //关闭文件
            }
            else{        //打开文件失败,退出程序
                exit(1);
            }
        }
        printf("size=%d\n",filesize);
        for(i=1;i<argc;i++){       //more命令后面可能跟多个文件
            if((fp=fopen(*(argv+i),"r"))!=0){     //以只读形式打开文件
                do_more(fp);        //显示文件内容
                fclose(fp);      //关闭文件
            }
            else{        //打开文件失败,退出程序
                exit(1);
            }
        }
    }
    tcsetattr(STDIN_FILENO,TCSANOW,&ots);
    return 0;
}
void gettermsize(int *w,int *h){
    struct winsize *ws;
    ws=(struct winsize*)malloc(sizeof(struct winsize));
    memset(ws,0x00,sizeof(struct winsize));
    ioctl(STDIN_FILENO,TIOCGWINSZ,ws);
    *w=ws->ws_col;
    *h=ws->ws_row;
}
void settermattr(){
    tcgetattr(STDIN_FILENO,&ts);    //获取终端属性
    ots=ts;         //备份终端属性
    ts.c_lflag &= (~ECHO);    //阻止回显
    ts.c_lflag &= (~ICANON);    //设置终端为非标准模式
    ts.c_cc[VMIN]=1;        //这两行当需要从终端获取字符时立即返回，不需要按回车键
    ts.c_cc[VTIME]=0;
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&ts);    //设置终端的新属性。TCSAFLUSH表示输出队列空了以后才生效，生效之前的输出队列被flush
}
void do_more(FILE *fp){
    int linesize=width;
    char line[linesize];
    int num_of_lines=0;     //记录本次读了多少行
    int reply;      //记录see_more()的返回值
//    FILE *fp_tty;
    while(fgets(line,linesize,fp)){    //从文件中读取一行内容
        readsize+=strlen(line);     //读出的内容长度
        if(num_of_lines==height){   //如果满屏了
            reply=see_more(); //从键盘获取用户输入的命令
            if(reply==0){        //用户不需要显示更多内容了，要退出
                tcsetattr(STDIN_FILENO,TCSANOW,&ots);
                exit(0);
            }
            num_of_lines-=reply;
        }
        if(fputs(line,stdout)==EOF){ //把这行内容显示在用户屏幕上
            tcsetattr(STDIN_FILENO,TCSANOW,&ots);        //TCSANOW表示修改立即生效
            perror("fputs");
            exit(1);
        }
        num_of_lines++;
    }
}
int see_more(){
    int c;
    printf("\033[7m--more--(%2.0f%%)\033[m",(double)readsize/filesize*100);
    while((c=getc(stdin))!=EOF){            //注意加括号，赋值操作符是右结合的。如果不加括号会把getchar()!=EOF的结果赋给c
        if(c=='q'){
        	printf("\r                            \r");
            return 0;
        }
        if(c==' ')      //空格
        {
        	printf("\r                            \r");
            return height;
        }
        if(c=='\n')     //回车
        {
        	printf("\r                            \r");
            return 1;
        }
    }
    return 0;
}
