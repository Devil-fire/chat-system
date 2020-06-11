#define MAXLEN 140
#define SHTLEN 20
typedef struct Reply {
	char ID[SHTLEN + 1];
	char time[SHTLEN + 1];
	char text[MAXLEN + 1];
}Reply;
typedef struct Post {
	char ID[SHTLEN + 1];
	char title[SHTLEN + 1];
	char time[SHTLEN + 1];
	char text[MAXLEN + 1];
	int flag_file;
}Post;
int bbs_detail_win(char *filename,char *name,int num, Post post_info, Reply reply[]);