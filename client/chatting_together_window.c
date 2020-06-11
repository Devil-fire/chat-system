#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include"chat.h"
#include"chatting_window.h"
#include"queue.h"
#include"hashmap.h"

GtkWidget *chatting_window;

//发送目标用户窗口。
// 文本框缓冲区。
GtkTextBuffer *bufferuser;
GtkTextBuffer *buffernotice;
GtkTextBuffer *buffers;
GtkWidget *filew;
extern int client_socket;
extern char *username;


//根据button的值发送消息时的自我维护。
void sendtouser_together(GtkButton  *button)
{
	char str[250];
	Packet packet;
	Data data;
	Kind kind;
	char buf[100];
	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(buffers,&start,&end);
	gchar* text=gtk_text_buffer_get_text(buffers,&start,&end,FALSE);
	gtk_text_buffer_set_text(buffers,"",0);
	if(strlen(text)==0)
	{
		printf("不能为空\n");	//打印内容。
	}
	else
	{
		strcpy(data.message.id_from,username);
		strcpy(data.message.id_to,"_all_");
		strcpy(data.message.str,text);
		if(build_packet(&packet,enum_chat,data) == -1)
		{	 
			printf("fail to build the packet!\n");
			return;
		}
		queue_push(write_queue,packet);
		strcpy(buf,data.message.id_from);
		strcat(buf,":\n\t");
		strcat(buf,data.message.str);
		strcat(buf,"\n");
		GtkTextIter start,end; 
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(bufferuser),&start,&end);
		gtk_text_buffer_insert(GTK_TEXT_BUFFER(bufferuser),&end,buf,strlen(buf));
		char path[50] = "./history/";
        strcat(path,data.message.id_to);
		FILE *fp = fopen("群聊","a+");
        fputs(buf,fp);
        fclose(fp);
	}
}

///处理接受到的消息
void process_together()
{
	Packet packet;
	Data data;
	Kind kind;
	char buf[100];
	int i=0;
	while(1)
	{
		printf("%d\n",i++);
		if(queue_is_empty(read_queue))
		{
			queue_pop(read_queue,&packet);
			parse_packet(packet,&kind,&data);
			//printf("%s,%s,%d\n",data.message.id_from,friend,strcmp(data.message.id_from,friend));
			if(kind==enum_chat && !strcmp(data.message.id_to,"_all_"))
			{
				strcpy(buf,data.message.id_from);
				strcat(buf,":\n\t");
				strcat(buf,data.message.str);
				strcat(buf,"\n");
				GtkTextIter start,end;
				gtk_text_buffer_get_bounds(bufferuser,&start,&end);
				gtk_text_buffer_insert(bufferuser,&end,buf,strlen(buf));
			}
			else
			{
				queue_push(read_queue,packet);
			}
		}
		sleep(1);
	}
}

gboolean quit_together_chatroom(GtkWidget *button)
{
	// Packet packet;
	// Data data;
	// if(build_packet(&packet,enum_quitchat,data) == -1)
	// {	    //打包类型为enum_chat的包
	// 	printf("fail to build the packet!\n");
	// 	return;
	// }
	// queue_push(write_queue,packet);
	// write(client_socket,&packet,sizeof(Packet));
	//memset(friend_chatting[chatting_num],0,20);
	//friend_chatting[hash_table_lookup(hashTable,friend)->nValue] = 0;
	gtk_main_quit();
	//gtk_widget_destroy(chatting_window);
	return FALSE;
}

int chatting_win_together()
{
	pthread_t thread;
	GtkWidget *entry;
	gtk_init(NULL, NULL);	
    // 创建顶层窗口
    chatting_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // 设置窗口的标题
    gtk_window_set_title(GTK_WINDOW(chatting_window), "群聊");
    // 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(chatting_window), GTK_WIN_POS_CENTER);
    // 设置窗口的最小大小
	gtk_widget_set_size_request(chatting_window, 700, 800);
    // 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(chatting_window), FALSE); 
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(G_OBJECT(chatting_window), "destroy", G_CALLBACK(quit_together_chatroom),NULL);
    //创建一个固定容器
 	GtkWidget *fixed = gtk_fixed_new(); 	
	gtk_container_add(GTK_CONTAINER(chatting_window), fixed);

	// g_signal_connect(G_OBJECT(chatting_window),1000,800);
    //GtkWidget *scrolled=gtk_scrolled_window_new(NULL,NULL);
	//gtk_scrolled_window_set_policy( scrolled, NULL, GTK_POLICY_AUTOMATIC );

    GtkWidget *label_two;
	GtkWidget *label_one;

	PangoFontDescription *pattern;
	pattern = pango_font_description_from_string("Simsun 15");
	// 创建标签
	label_one = gtk_label_new("聊天内容");
	// 将按钮放在布局容器里
	gtk_fixed_put(GTK_FIXED(fixed), label_one,45,20);
	gtk_widget_modify_font(label_one,pattern);	
 
 	//创建聊天框
    GtkWidget *chat_view=gtk_text_view_new();
    gtk_widget_set_size_request(chat_view,560,130);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(chat_view),TRUE);
    gtk_fixed_put(GTK_FIXED(fixed),chat_view,45,625);
    gtk_widget_modify_font(chat_view,pattern);
	buffers = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat_view));
// 创建按钮
    GtkWidget *bsend = gtk_button_new_with_label("--发送--");
    gtk_fixed_put(GTK_FIXED(fixed), bsend, 505, 570);
    gtk_widget_set_size_request(bsend,100,40);
//    gtk_widget_modify_font(bsend,pattern);

	g_signal_connect(bsend, "clicked", G_CALLBACK(sendtouser_together),NULL);

 
	// 文本框聊天窗口
	GtkWidget *view = gtk_text_view_new(); 
	gtk_widget_set_size_request (view, 560,500);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed), view, 45, 55);
	gtk_widget_modify_font(view,pattern);
	// 获取文本缓冲区
	bufferuser=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));     	
 
 
	// 显示窗口全部控件
	gtk_widget_show_all(chatting_window);	
	
	int sendbytes, res;

	//开启线程监听收到的数据
	//res = pthread_create(&thread, NULL, strdeal, NULL);
	res = pthread_create(&thread, NULL, process_together, NULL);
	if (res != 0)
	{          
		exit(res);
	}
	usleep(10);
	gtk_main();
	pthread_cancel(thread);
	return 0;
}

