#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include"chat.h"
#include"history_window.h"
#include"queue.h"
#include"hashmap.h"

GtkWidget *history_window;

//发送目标用户窗口。
// 文本框缓冲区。

GtkTextBuffer *historytext;
void show_history(char *friend_name)
{
    char buf[200];
    char tmp1[50];
    char tmp2[140];
    char filepath[50];
    sprintf(filepath,"./history/%s",friend_name);
    FILE *fp = fopen(filepath, "r");
    if(!fp){
        printf("can't open file\n");
        return;
    }
    while(!feof(fp)){
        fscanf(fp, "%s", tmp1);
        fscanf(fp, "%s", tmp2);
        strcpy(buf,tmp1);
		strcat(buf,"\n\t");
		strcat(buf,tmp2);
		strcat(buf,"\n");
        GtkTextIter start,end;
        gtk_text_buffer_get_bounds(historytext,&start,&end);
        gtk_text_buffer_insert(historytext,&end,buf,strlen(buf));
    }
    fclose(fp);	
}
int history_win(char *friend_name)
{
	GtkWidget *entry;
	gtk_init(NULL, NULL);	
    // 创建顶层窗口
    history_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // 设置窗口的标题
    gtk_window_set_title(GTK_WINDOW(history_window), "历史记录");
    // 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(history_window), GTK_WIN_POS_CENTER);
    // 设置窗口的最小大小
	gtk_widget_set_size_request(history_window, 700, 800);
    // 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(history_window), FALSE); 
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(G_OBJECT(history_window), "destroy", G_CALLBACK(gtk_main_quit),NULL);
    //创建一个固定容器
 	GtkWidget *fixed = gtk_fixed_new(); 	
	gtk_container_add(GTK_CONTAINER(history_window), fixed);

	PangoFontDescription *pattern;
	pattern = pango_font_description_from_string("Simsun 15");
	GtkWidget* label_one = gtk_label_new("历史记录");
	gtk_fixed_put(GTK_FIXED(fixed), label_one,45,20);
	gtk_widget_modify_font(label_one,pattern);	
 
    GtkWidget *history_view=gtk_text_view_new();
    gtk_widget_set_size_request(history_view,560,740);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(history_view),TRUE);
    gtk_fixed_put(GTK_FIXED(fixed),history_view,45,45);
    gtk_widget_modify_font(history_view,pattern);
	historytext = gtk_text_view_get_buffer(GTK_TEXT_VIEW(history_view));
    show_history(friend_name);
	gtk_widget_show_all(history_window);	
	
	usleep(10);
	gtk_main();
	return 0;
}

