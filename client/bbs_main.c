#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "bbs_main.h"
#include "chat.h"
#include "chatting_window.h"
#include "main_window.h"

char *bbs_username="new";
int flag = 0;
int i = 0;
int j = 0;
GtkWidget *window;
GtkWidget *fixed;
GtkWidget *fixed2;
GtkWidget *fixed3;
GtkWidget *vbox;
GtkWidget *vbox1;
GtkWidget *vbox1_0;
GtkWidget *vbox1_1;
GtkWidget *sep;
GdkPixbuf *pix0;
GtkWidget *label1;
GtkWidget *label2;
GtkWidget *label3;
GtkWidget *label4;
GtkButton *button;
GtkButton *button1;
GtkButton *button2;
GtkButton *button3;
GtkWidget *frame;
GtkWidget *image;
GtkWidget *entry;
GtkWidget *text;
pthread_t thID;
GtkTextBuffer *buffers;
GtkWidget *scrolled;
char *docupath;
char *docuname;

void on_button_click_detail(GtkWidget* button, gpointer buff)   //点击详情按钮，buff为该贴的标题+时间，格式为“%s+%s"
{

}

// int docu_send()
// {   
//     //读取文件本地路径及文件名

//     //发送文件名
// 	Kind kind;
// 	Packet packet;
// 	Data data;
// 	int verify;
// 	//strcpy(data.message.id_from,bbs_username);
// 	strcpy(data.message.str,docuname);
// 	//strcpy(data.message.id_from,id_from);
// 	//strcpy(data.message.id_to,id_to);
// 	//build_packet(&packet,enum_docu,data);
// 	// if(write(client_socket,&packet,sizeof(Packet))<0){
// 	// 	perror("fail to recv docusen");    //把"fail to recv"输出到标准错误stderr。
// 	// 	close(client_socket);  //关闭socket端口。
// 	// 	exit(1);
// 	// };
//     printf("docusen: filename=%s\n",docuname);


//     //打开文件
//     // if( ( fq = fopen(fpath,"rb") ) == NULL ){
//     //     printf("File open error.\n");
//     //     return 0;
//     // }

//     // //传输文件
// 	// //memset(file_buff,0,4096);
//     // bzero(file_buff,sizeof(file_buff));
//     // while(!feof(fq))
//     // {
//     //     len = fread(file_buff, 1, sizeof(file_buff), fq);
//     //     if(len != write(client_socket, file_buff, len)){
//     //         printf("write.\n");
//     //         break;
//     //     }
//     // }
//     // fclose(fq);
//     return 1;
// }

void show_list(list *list[])
{
    for(i = 0; strcmp(list[i]->id,"") != 0; i++)
    {
	fixed = gtk_fixed_new(); 
        gtk_widget_set_size_request(fixed,350,40);//width,height
        gtk_container_add(GTK_CONTAINER(vbox1_1), fixed);

	label2 = gtk_label_new(list[i]->title);
	gtk_fixed_put(GTK_FIXED(fixed), label2, 0, 10);

	if(list[i]->flag == 1){
	    GdkPixbuf *pix0;
    	    pix0=gdk_pixbuf_new_from_file("file.jpg",NULL);
    	    GdkPixbuf *pixnew0;
    	    pixnew0=gdk_pixbuf_scale_simple(pix0,20,20,GDK_INTERP_BILINEAR);
    	    GtkWidget *image0;
     	    image0=gtk_image_new_from_pixbuf(pixnew0);
	    gtk_fixed_put(GTK_FIXED(fixed), image0, 180, 10);
	}

	label3 = gtk_label_new(list[i]->id);
	gtk_fixed_put(GTK_FIXED(fixed), label3, 200, 10);

	label4 = gtk_label_new(list[i]->time);
	gtk_fixed_put(GTK_FIXED(fixed), label4, 240, 10);

	char buff[40];
	strncpy(buff, list[i]->title, strlen(list[i]->title));
	strcat(buff,"+");
	strncat(buff, list[i]->time, strlen(list[i]->time));
	buff[strlen(buff)-1]='\0';
	button3 = gtk_button_new_with_label("详情");
	gtk_fixed_put(GTK_FIXED(fixed), button3, 280, 10);
	g_signal_connect(button3, "clicked", G_CALLBACK(on_button_click_detail), buff);

	sep = gtk_hseparator_new();//分割线
	gtk_container_add(GTK_CONTAINER(vbox1_1),sep);
    }

}

void on_button_clicked_frash(GtkWidget* button)    //点击主界面刷新按钮
{
   
   //向服务器发送刷新帖子列表请求 


   //将获得的信息处理成list数组

   //show_list(l);
   
}



// void file_ok_sel( GtkWidget *w,GtkFileSelection *fs )
// {
// 	char buf[50]={0};
// 	char bufrev[50]={0};
// 	char *filepath=gtk_file_selection_get_filename (GTK_FILE_SELECTION (fs));
// 	if(strcmp(filepath, "") != 0)    flag = 1;
// 	int len=strlen(filepath);
// 	int j=0;
// 	for(i=len-1;i>0;i--)
// 	{
// 		if(filepath[i]=='/') break;
// 		buf[j++]=filepath[i];
// 	}
// 	int lb;
// 	lb=strlen(buf);
//     //printf("%s       %s\n",filepath,buf);
// 	for(i=0;i<lb;i++)
// 	{
//         //printf("%c   ",buf[lb-i-1]);
// 		bufrev[i]=buf[lb-i-1];
// 	}
//     //printf("\n%s\n",bufrev);
// 	docupath = filepath;
// 	docuname = bufrev;

// 	printf("docupath=%s,docuname=%s\n",docupath,docuname);
//         docu_send();
// }

void on_button_clicked_upload(GtkWidget* button)    //点击上传文件按钮
{
    GtkWidget *filew;
    gtk_init (NULL, NULL);
    filew = gtk_file_selection_new ("File selection");
    g_signal_connect (G_OBJECT (filew), "destroy",G_CALLBACK (gtk_main_quit), NULL);
    //g_signal_connect (G_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),"clicked",G_CALLBACK (file_ok_sel), filew);
    g_signal_connect_swapped (G_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),"clicked",G_CALLBACK (gtk_widget_destroy), filew);
    //gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew),"penguin.png");
    gtk_widget_show (filew);
    gtk_main ();
}

void on_button_clicked_post(GtkWidget* button)    //点击发帖按钮
{
    time_t timep;
    list l;
    strcpy(l.id, bbs_username);
    time (&timep);
    strcpy(l.time, ctime(&timep));
    strcpy(l.title, gtk_entry_get_text(GTK_ENTRY(entry)));
    l.flag = flag;
    GtkTextIter start,end;
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buffers),&start,&end);
    gchar* text1=gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffers),&start,&end,FALSE);
    strcpy(l.text, text1);
    //传送l给服务器

 
    //刷新服务器
    on_button_clicked_frash(button);

    //
    gtk_entry_set_text(GTK_ENTRY(entry), "");
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(text), "");
}



void title_id(char *id)
{
    fixed = gtk_fixed_new(); 
    gtk_widget_set_size_request(fixed,400,40);//width,height
    //gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,FALSE,0);
    gtk_container_add(GTK_CONTAINER(window), fixed);

    label1=gtk_label_new("Neu bbs");
    gtk_fixed_put(GTK_FIXED(fixed), label1, 0,0);

    label2=gtk_label_new(id);
    gtk_fixed_put(GTK_FIXED(fixed), label2, 0,360);

    button = gtk_button_new_with_label("刷新");
    gtk_fixed_put(GTK_FIXED(fixed), button, 0,320); 
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked_frash),NULL);

    gtk_widget_show_all(window);
}
void bbs_set_background(GtkWidget *widget,int w,int h,gchar *path)
{
    gtk_widget_set_app_paintable(widget,TRUE);
    gtk_widget_realize(widget);
    gtk_widget_queue_draw(widget);

    GdkPixbuf *src_pixbuf=gdk_pixbuf_new_from_file(path,NULL);
    GdkPixbuf *dst_pixbuf=gdk_pixbuf_scale_simple(src_pixbuf,w,h,GDK_INTERP_BILINEAR);
    GdkPixmap *pixmap=NULL;
    gdk_pixbuf_render_pixmap_and_mask(dst_pixbuf,&pixmap,NULL,128);
    gdk_window_set_back_pixmap(widget->window,pixmap,FALSE);

    g_object_unref(src_pixbuf);
    g_object_unref(dst_pixbuf);
    g_object_unref(pixmap);
}
void bbs_main(char *user, list *list[])
{
    bbs_username=user;
    gtk_init(NULL,NULL);
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"Neu bbs");
    gtk_widget_set_size_request(window,400,800);
    bbs_set_background(window,400,800,"background.jpg");
    g_signal_connect (G_OBJECT (window), "destroy",G_CALLBACK (gtk_main_quit), NULL);

    vbox=gtk_vbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(window),vbox);    //框架
    title_id(user);

    vbox1=gtk_vbox_new(FALSE,0);
    gtk_widget_set_size_request(vbox1,0,0);
    gtk_box_pack_start(GTK_BOX(vbox),vbox1,TRUE,TRUE,0);
    frame=gtk_frame_new("帖子列表");
    gtk_container_add(GTK_CONTAINER(vbox1),frame);
    vbox1_0=gtk_vbox_new(FALSE,0);
    gtk_widget_set_size_request(vbox1_0,0,0);
    gtk_container_add(GTK_CONTAINER(frame),vbox1_0);

    //增加滚轮
    scrolled=gtk_scrolled_window_new(NULL,NULL);
    gtk_container_set_border_width(GTK_CONTAINER(scrolled),10);
    gtk_scrolled_window_set_policy(scrolled,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(vbox1_0),scrolled);
    vbox1_1=gtk_vbox_new(FALSE,0);
    gtk_widget_set_size_request(vbox1_1,350,560);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),vbox1_1);

    show_list(list);    

    sep = gtk_hseparator_new();//分割线
    gtk_container_add(GTK_CONTAINER(window), sep);

    fixed = gtk_fixed_new(); 
    gtk_widget_set_size_request(fixed,400,200);//width,height
    //gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,FALSE,0);
    gtk_container_add(GTK_CONTAINER(window), fixed);

    label1=gtk_label_new("发表新帖");
    gtk_fixed_put(GTK_FIXED(fixed), label1, 20,10);

    entry = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), label1, 20,40);
    gtk_entry_set_max_length(GTK_ENTRY(entry), 20);

    text=gtk_text_view_new();
    gtk_widget_set_size_request(text,360,90);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text),TRUE);
    gtk_fixed_put(GTK_FIXED(fixed),text,20,70);

    button1 = gtk_button_new_with_label("上传文件");
    gtk_fixed_put(GTK_FIXED(fixed), button1, 20,170); 
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked_upload),NULL);

    button2 = gtk_button_new_with_label("发帖");
    gtk_fixed_put(GTK_FIXED(fixed), button1, 60,170); 
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked_post),NULL);

    gtk_widget_show_all(window);
    
}
    
