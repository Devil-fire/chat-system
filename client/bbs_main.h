//#ifndef bbs_main_h
#define bbs_main_h

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

#define MYPORT 4567			//约定端口
#define MAXLEN 140			//最大消息长度
#define SHTLEN 20       //短消息长度

typedef struct _list{
	char id[SHTLEN + 1];         //发帖人
	char time[SHTLEN + 1];         //消息时间
	char title[SHTLEN + 1];          //帖子标题
	char text[MAXLEN + 1];           //帖子内容
	int flag;            //文件标志位，flag为0表示没有文件，为1表示有文件
}list;

void bbs_main(char *user, list *list[]);