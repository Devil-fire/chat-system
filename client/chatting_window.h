#include <gtk/gtk.h>

void sendtouser(GtkButton  *button,char *friend);

//保存消息记录
void savetxt(GtkButton  *button, gpointer entry);

void readtxt(GtkButton  *button, gpointer entry);

void *strdeal(void *arg);

int chatting_win(char *friend_name);


