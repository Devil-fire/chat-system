#include <gtk/gtk.h> 
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include "bbs_detail.h"
#include "chat.h"
#include "queue.h"

char post_name[20];
GtkTextBuffer* post_buffer;
GtkTextBuffer* reply_list_buffer;
GtkTextBuffer* reply_buffer;
GtkTextBuffer* my_reply_buffer;
extern int client_socket;
char post_title[SHTLEN + 1];
char post_time[SHTLEN + 1];

int reply_num;
int temp_reply_num;

struct My_reply {
	char ID[SHTLEN + 1];
	char time[SHTLEN + 1];
	char text[MAXLEN + 1];
	char post_title[SHTLEN + 1];
	char post_time[SHTLEN + 1];
}my_reply;

void file_download(GtkWidget *button)
{
	const char*name = gtk_button_get_label(button);
	Packet packet;
	Data data;
	strcpy(data.message.str,name);
	build_packet(&packet,enum_bfyes,data);
	queue_push(write_queue,packet);
	pthread_mutex_lock(&mtx);
	printf("%s\n",data.message.str);
	char  file_buff[4096];
    FILE *fp;
    int  n;
    Kind kind;
    read(client_socket, &packet, sizeof(Packet));
    parse_packet(packet, &kind, &data);
    int size = atoi(data.message.str);
    int now = 0;
    char path[50];
    sprintf(path,"file/%s",name);
    if((fp = fopen(path,"wb") ) == NULL)
    {
        printf("new file create fail.\n");
        return 0;
    }
    while(now < size){
		printf("%d\n",now);
        if (size - now >= 4096)
        {
           n = read(client_socket, file_buff, sizeof(file_buff));
        }
        else
        {
            n = read(client_socket, file_buff, size-now);
        }
        fwrite(file_buff, 1, n, fp);
        now += n;
    }
    fclose(fp);
	pthread_mutex_unlock(&mtx);
}

void nofile_prompt()
{
	gtk_init(NULL, NULL);
	// prompt_window
	GtkWidget* prompt_window;
	prompt_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(prompt_window), " ");
	gtk_window_set_position(GTK_WINDOW(prompt_window), GTK_WIN_POS_CENTER);
	gtk_widget_set_size_request(prompt_window, 220, 80);
	gtk_window_set_resizable(GTK_WINDOW(prompt_window), FALSE);
	g_signal_connect(G_OBJECT(prompt_window), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	GtkWidget* fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(prompt_window), fixed);
	GtkWidget* label_prompt;
	label_prompt = gtk_label_new("No file to download!");
	gtk_fixed_put(GTK_FIXED(fixed), label_prompt, 40, 30);
	// GtkWidget* button_prompt;
	// button_prompt = gtk_button_new_with_label("OK");
	// gtk_fixed_put(GTK_FIXED(fixed), button_prompt, 75, 60);
	// g_signal_connect(button_prompt, "clicked", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show_all(prompt_window);
	gtk_main();
}
 
void send_my_reply(GtkWidget* button,Reply reply[])
{
	Data data;
	Packet packet;
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(my_reply_buffer),&start,&end);
	gchar* my_reply_text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(my_reply_buffer), &start, &end, FALSE);
	
	strcpy(my_reply.ID, post_name);
	time_t timep;
	struct tm* p;
	time(&timep);
	p = gmtime(&timep);
	sprintf(my_reply.time,"%d:%d", p->tm_hour, p->tm_min);
	strcpy(my_reply.text, my_reply_text);

	char buf[200];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s  %s\n%s\n\n", my_reply.ID, my_reply.time, my_reply.text);
	sprintf(data.message.str, "%s/%s/%s\n", my_reply.ID, my_reply.time, my_reply.text);
	strcpy(data.message.id_to,post_title);
	build_packet(&packet,enum_brepl,data);
	queue_push(write_queue,packet);
	GtkTextIter start1, end1;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(reply_list_buffer), &start1, &end1);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(reply_list_buffer), &end1, buf, strlen(buf));
}

void detail_set_background(GtkWidget* widget, int w, int h, gchar* path)
{
	gtk_widget_set_app_paintable(widget, TRUE);
	gtk_widget_realize(widget);
	gtk_widget_queue_draw(widget);

	GdkPixbuf* src_pixbuf = gdk_pixbuf_new_from_file(path, NULL);
	GdkPixbuf* dst_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf, w, h, GDK_INTERP_BILINEAR);
	GdkPixmap* pixmap = NULL;
	gdk_pixbuf_render_pixmap_and_mask(dst_pixbuf, &pixmap, NULL, 128);
	gdk_window_set_back_pixmap(widget->window, pixmap, FALSE);

	g_object_unref(src_pixbuf);
	g_object_unref(dst_pixbuf);
	g_object_unref(pixmap);
}



int bbs_detail_win(char * filename, char *name, int num, Post post_info, Reply reply[])
{
	strcpy(post_title,post_info.title);
	strcpy(post_time,post_info.time);
	strcpy(post_name,name);
	reply_num = num;

	gtk_init(NULL, NULL);
	// detail_wondow
	GtkWidget* detail_wondow;
	detail_wondow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	//���ô��ڱ���
	gtk_window_set_title(GTK_WINDOW(detail_wondow), post_title);
	// ���ô�������ʾ���е�λ��Ϊ����
	gtk_window_set_position(GTK_WINDOW(detail_wondow), GTK_WIN_POS_CENTER);
	// ���ô��ڵ���С��С
	gtk_widget_set_size_request(detail_wondow, 650, 650);
	// �̶����ڵĴ�С
	gtk_window_set_resizable(GTK_WINDOW(detail_wondow), FALSE);
	// ��gtk_main_quit����
	g_signal_connect(G_OBJECT(detail_wondow), "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	//���ñ���
	detail_set_background(detail_wondow, 400, 950, "background.jpg");
	//����һ���̶�����
	GtkWidget* fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(detail_wondow), fixed);

	PangoFontDescription* pattern;
	pattern = pango_font_description_from_string("Simsun 10");
	
	char buf[500];
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s  %s\n%s\n", post_info.ID, post_info.time, post_info.text);	
	GtkWidget* scrolled0;
	scrolled0 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled0, 560, 130);
	gtk_fixed_put(GTK_FIXED(fixed), scrolled0, 45, 10);
	GtkWidget* post_view = gtk_text_view_new();
	//gtk_widget_set_size_request(post_view, 560, 130);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(post_view), TRUE);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled0), post_view);
	gtk_widget_modify_font(post_view, pattern);
	post_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(post_view));
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(post_buffer), &start, &end);
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(post_buffer), &end, buf, strlen(buf));
	//�ļ�����
	GtkWidget* button_download;
	button_download = gtk_button_new_with_label(filename);
	gtk_fixed_put(GTK_FIXED(fixed), button_download, 45, 140);//put in
	if (post_info.flag_file == 1)
	{
		g_signal_connect(button_download, "clicked", G_CALLBACK(file_download),NULL);
	}
	else if (post_info.flag_file == 0)
	{
		g_signal_connect(button_download, "clicked", G_CALLBACK(nofile_prompt), NULL);
	}

	
	
	//����list
	GtkWidget* label_replies;
	label_replies = gtk_label_new("Replies");
	gtk_fixed_put(GTK_FIXED(fixed), button_download, 45, 190);//put in
	//��������
	char buf1[3000];
	char buf2[200];
	memset(buf1, 0, sizeof(buf1));
	memset(buf2, 0, sizeof(buf2));
	for (int i = 0;i < reply_num;i++)
	{
		sprintf(buf2, "%s  %s\n%s\n\n", reply[i].ID, reply[i].time, reply[i].text);
		strcat(buf1, buf2);
	}
	temp_reply_num = reply_num;
	//���������ı���
	GtkWidget* scrolled;
	scrolled = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled, 560, 230);
	gtk_fixed_put(GTK_FIXED(fixed), scrolled, 45, 200);//put in 
	//��ʾ�������ڹ���
	gtk_widget_show(scrolled);
	//�����ı���ͼ����
	GtkWidget* reply_list_view = gtk_text_view_new();
	//gtk_widget_set_size_request(reply_list_view, 560, 130);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(reply_list_view), TRUE);
	//���ı���ͼ���������������
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled), reply_list_view);
	gtk_widget_modify_font(reply_list_view, pattern);
	reply_list_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(reply_list_view));
	GtkTextIter start1, end1;    //�½�����������buffer��λ�õĽṹstart��end��
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(reply_list_buffer), &start1, &end1);
	/*�����ı���������*/
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(reply_list_buffer), &end1, buf1, strlen(buf1));

	//������
	GtkWidget* label_reply;
	label_reply = gtk_label_new("Write your reply");
	gtk_fixed_put(GTK_FIXED(fixed), label_reply, 45, 450);//put in
	//������
	GtkWidget* scrolled1;
	scrolled1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(scrolled1, 560, 130);
	gtk_fixed_put(GTK_FIXED(fixed), scrolled1, 45, 470);//put in 
	gtk_widget_show(scrolled1);
	//�ı���
	GtkWidget* reply_view = gtk_text_view_new();
	//gtk_widget_set_size_request(reply_view, 400, 200);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(reply_view), TRUE);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled1), reply_view);//put in
	gtk_widget_modify_font(reply_view, pattern);
	// ��ȡ�ı�������
	my_reply_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(reply_view));
	//���Ͱ�ť
	GtkWidget* reply_button = gtk_button_new_with_label("Reply");
	gtk_fixed_put(GTK_FIXED(fixed), reply_button, 45, 610);//put in
	gtk_widget_set_size_request(reply_button, 60, 30);
//##����������ͻ������ݲ�����
	g_signal_connect(reply_button, "clicked", G_CALLBACK(send_my_reply), reply);

	gtk_widget_show_all(detail_wondow);
	gtk_main();
	return FALSE;
}