#ifndef  bbs_main_h
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
#include "chat.h"

typedef struct _list{
	char id[SHTLEN + 1];         //发帖人
	char time[SHTLEN + 1];         //消息时间
	char title[SHTLEN + 1];          //帖子标题
	char text[MAXLEN + 1];           //帖子内容
	int flag;            //文件标志位，flag为0表示没有文件，为1表示有文件
}list;

void bbs_main(char *user, list *list);
int bbs_flag;
int bbs_num;
int bbs_detail_flag;
int bbs_detail_num;
list bbs_list[100];

#endif