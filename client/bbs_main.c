#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "bbs_main.h"
#include "chat.h"
#include "chatting_window.h"
#include "main_window.h"
#include "queue.h"
#include "bbs_detail.h"

char *bbs_username="new";
int flag = 0;
int i = 0;
int j = 0;
GtkWidget *window;
GtkWidget *fixed;
GtkWidget* fixed1;
GtkWidget *fixed2;
GtkWidget *fixed3;
GtkWidget *vbox;
GtkWidget *bbs_vbox;
GtkWidget *bbs_vbox_0;
GtkWidget *bbs_vbox_1;
GtkWidget *sep;
GdkPixbuf *pix0;
GtkWidget *label1;
GtkWidget *label2;
GtkWidget *label3;
GtkWidget *label4;
GtkWidget *button;
GtkWidget *button1;
GtkWidget *button2;
GtkWidget *button3;
GtkWidget *frame;
GtkWidget *image;
GtkWidget *entry;
GtkWidget *text;
GtkTextBuffer *buffer;
GtkWidget *scrolled;
char *docupath;
char *docuname;
GtkWidget *filew;
extern int client_socket;
char bufrev[50]={0};
Reply reply[100];
char *filepath;

void on_button_click_detail(GtkWidget* button)
{
	const char *buff = gtk_button_get_label((GtkButton *)button);
    Kind kind;
	Packet packet;
	Data data;
    int tmp = 0;
    strcpy(data.message.str,buff);
    build_packet(&packet,enum_bcont,data);
    queue_push(write_queue,packet);
    Post post_info;
    char filename[20];
    while (1)
    {
        if (bbs_detail_flag)
        {
            queue_pop(read_queue,&packet);
            parse_packet(packet,&kind,&data);
            if (kind==enum_bcont&&tmp!=0)
            {
                int num;
	            char *revbuf[3] = {0};
                split(data.message.str,"/",revbuf,&num); 
                if (num==3)
                {
                    strcpy(reply[--bbs_detail_num].ID,revbuf[0]);
                    strcpy(reply[bbs_detail_num].time,revbuf[1]);
                    strcpy(reply[bbs_detail_num].text,revbuf[2]);
                }
                tmp++;
                if (bbs_detail_num==0)
                {
                    break;
                }
            }
            else if (kind==enum_bcont&&tmp == 0)
            {
                int num;
	            char *revbuf[6] = {0};
                split(data.message.str,"/",revbuf,&num); 
                if (num==6)
                {
                    strcpy(post_info.ID,revbuf[0]);
                    strcpy(post_info.time,revbuf[1]);
                    strcpy(post_info.title,revbuf[2]);
                    strcpy(post_info.text,revbuf[3]);
                    post_info.flag_file = atoi(revbuf[4]);
                    strcpy(filename,revbuf[5]);
                }
                --bbs_detail_num;
                tmp++;
            }
            else
            {
                queue_push(read_queue,packet);
            }
        }
    }
    bbs_detail_flag = 0;
    bbs_detail_win(filename,bbs_username,tmp-1,post_info,reply);
    tmp = 0;
}
int bbs_file_size(char* filename)
{
    struct stat statbuf;
    stat(filename,&statbuf);
    int size=statbuf.st_size;
    return size;
}
int bbs_docu_send( char* docupath, char *docuname)
{
	char  file_buff[4096];
	Kind kind;
	Packet packet;
	Data data;
	FILE *fp;
	int size = bbs_file_size(docupath);
	char string[20];
    printf("size:%d\n",size);
    sprintf(data.message.str,"%d",size);
	build_packet(&packet,enum_file,data);
	if(write(client_socket,&packet,sizeof(Packet))<0){
		perror("fail to recv docusen");
		close(client_socket);
		exit(1);
	};

    if((fp = fopen(docupath,"rb")) == NULL ){
        printf("File open error.\n");
        return 0;
    }

    bzero(file_buff,sizeof(file_buff));
    while(!feof(fp))
    {
       int  len = fread(file_buff, 1, sizeof(file_buff), fp);
        if(len != write(client_socket, file_buff, len)){
            printf("error.\n");
            break;
        }
    }
    fclose(fp);
    return 1;
}

void show_list(list *list)
{
    gtk_widget_hide(bbs_vbox);
    //gtk_container_remove(GTK_CONTAINER(scrolled),bbs_vbox);
    //bbs_vbox = NULL;
    bbs_vbox=gtk_vbox_new(FALSE,0);
    gtk_widget_set_size_request(bbs_vbox,350,350);
    gtk_fixed_put(GTK_FIXED(fixed1), bbs_vbox, 25,30);
    //gtk_box_pack_start(GTK_BOX(vbox),bbs_vbox,TRUE,TRUE,0);
    frame=gtk_frame_new("帖子列表");
    gtk_container_add(GTK_CONTAINER(bbs_vbox),frame);
    bbs_vbox_0=gtk_vbox_new(FALSE,0);
    gtk_widget_set_size_request(bbs_vbox_0,0,0);
    gtk_container_add(GTK_CONTAINER(frame),bbs_vbox_0);

    //增加滚轮
    scrolled=gtk_scrolled_window_new(NULL,NULL);
    gtk_container_set_border_width(GTK_CONTAINER(scrolled),10);
    gtk_scrolled_window_set_policy((GtkScrolledWindow *)scrolled,GTK_POLICY_AUTOMATIC ,GTK_POLICY_ALWAYS);
    gtk_container_add(GTK_CONTAINER(bbs_vbox_0),scrolled);
    bbs_vbox_1=gtk_vbox_new(FALSE,0);
    gtk_widget_set_size_request(bbs_vbox_1,320,2000);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),bbs_vbox_1);
    //char buff[40];
    
    for(i = 0; strcmp(list[i].id,"") != 0; i++)
    {
	//memset(buff, 0, sizeof(buff));
	GtkWidget *fixed = gtk_fixed_new(); 
	gtk_widget_set_size_request(fixed,0,40);//width,height
	gtk_box_pack_start(GTK_BOX(bbs_vbox_1),fixed,FALSE,FALSE,0);
        //gtk_widget_set_size_request(fixed,350,20);//width,height
        //gtk_container_add(GTK_CONTAINER(bbs_vbox_1), fixed);

	//label2 = gtk_label_new(list[i].title);
	//gtk_fixed_put(GTK_FIXED(fixed), label2, 0, 15);
	//gtk_widget_set_size_request(label2,150, 20);

	button3 = gtk_button_new_with_label(list[i].title);
	gtk_fixed_put(GTK_FIXED(fixed), button3, 0, 15);
	gtk_widget_set_size_request(button3,150, 30);
	//gtk_button_set_relief(button3, GTK_RELIEF_NONE);
	g_signal_connect(button3, "clicked", G_CALLBACK(on_button_click_detail), NULL);

	if(list[i].flag == 1){
	    GdkPixbuf *pix0;
    	pix0=gdk_pixbuf_new_from_file("file.jpg",NULL);
    	GdkPixbuf *pixnew0;
    	pixnew0=gdk_pixbuf_scale_simple(pix0,20,20,GDK_INTERP_BILINEAR);
    	GtkWidget *image0;
     	image0=gtk_image_new_from_pixbuf(pixnew0);
	    gtk_fixed_put(GTK_FIXED(fixed), image0, 160, 25);
	    gtk_widget_set_size_request(image0,20, 20);
	}

	label3 = gtk_label_new(list[i].id);
	gtk_fixed_put(GTK_FIXED(fixed), label3, 200, 25);

	label4 = gtk_label_new(list[i].time);
	gtk_fixed_put(GTK_FIXED(fixed), label4, 240, 25);

	/*
	strncpy(buff, list[i].title, strlen(list[i].title));
	strcat(buff,"+");
	strncat(buff, list[i].time, strlen(list[i].time));
	buff[strlen(buff)-1]='\0';
        printf("%s\n", buff);
	string s(&buff[0], &buff[strlen(buff)])
	*/
        //g_signal_connect(button3, "clicked", G_CALLBACK(on_button_click_detail), "fffff");
	//gtk_widget_show_all(fixed);
	//sep = gtk_hseparator_new();//分割线
	//gtk_fixed_put(GTK_FIXED(fixed), sep, 0, 19);
    }
    gtk_widget_show_all(bbs_vbox);
}

void on_button_clicked_frash()    //点击主界面刷新按钮
{
    Kind kind;
    Data data;
    Packet packet;
    //char filename[20];
    build_packet(&packet,enum_blist,data);
    queue_push(write_queue,packet);
    while (1)
    {
        if (bbs_flag)
        {
            queue_pop(read_queue,&packet);
            parse_packet(packet,&kind,&data);
            if (kind==enum_blist)
            {
                int num;
	            char *revbuf[6] = {0};
                split(data.message.str,"/",revbuf,&num); 
                if (num==6)
                {
                    strcpy(bbs_list[--bbs_num].id,revbuf[0]);
                    strcpy(bbs_list[bbs_num].time,revbuf[1]);
                    strcpy(bbs_list[bbs_num].title,revbuf[2]);
                    strcpy(bbs_list[bbs_num].text,revbuf[3]);
                    bbs_list[bbs_num].flag = atoi(revbuf[4]);
                    //strcpy(filename,revbuf[5]);
                    printf("%d,%s\n",bbs_num,bbs_list[bbs_num].title);
                }
                if (bbs_num==0)
                {
                    break;
                }
                
            }
            else
            {
                queue_push(read_queue,packet);
            }
        }
    }
    bbs_flag = 0;

    show_list(bbs_list);
}



void bbs_file_ok_sel( GtkWidget *w, char *title)
{
	char buf[50]={0};
	filepath=gtk_file_selection_get_filename(GTK_FILE_SELECTION (filew));
	int len=strlen(filepath);
	int j=0;
	for(int i=len-1;i>0;i--)
	{
		if(filepath[i]=='/') break;
		buf[j++]=filepath[i];
	}
	int lb;
	lb=strlen(buf);
	for(int i=0;i<lb;i++)
	{
		bufrev[i]=buf[lb-i-1];
	}
    flag=1;
}

void on_button_clicked_upload(GtkButton* button)
{
    gtk_init (NULL, NULL);
    char title[20];
    strcpy(title, gtk_entry_get_text(GTK_ENTRY(entry)));
    filew = gtk_file_selection_new ("File selection");
    g_signal_connect (G_OBJECT (filew), "destroy",G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (G_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),"clicked",G_CALLBACK (bbs_file_ok_sel), title);
    g_signal_connect_swapped (G_OBJECT (GTK_FILE_SELECTION (filew)->ok_button),"clicked",G_CALLBACK (gtk_widget_destroy), filew);
    //gtk_file_selection_set_filename (GTK_FILE_SELECTION(filew),"penguin.png");
    gtk_widget_show (filew);
    gtk_main ();
}

void on_button_clicked_post(GtkWidget* button)    //点击发帖按钮
{
    list l;
    strcpy(l.id, bbs_username);
    time_t timep;
    struct tm* p;
    time(&timep);
    p = gmtime(&timep);
    sprintf(l.time,"%d:%d", p->tm_hour, p->tm_min);
    strcpy(l.title, gtk_entry_get_text(GTK_ENTRY(entry)));
    l.flag = flag;
    buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text));
    GtkTextIter start,end;
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buffer),&start,&end);
    gchar* post_text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, FALSE);
    strcpy(l.text, post_text);
    //传送l给服务器
    Kind kind;
    Data data;
    Packet packet;
    if (flag==1)
    {   
        sprintf(data.message.str,"%s/%s/%s/%s/%s/%d/%s\n",l.title,l.id,l.time,l.title,l.text,l.flag,bufrev);
        build_packet(&packet,enum_bpostf,data);
        queue_push(write_queue,packet);
        sleep(1);
        bbs_docu_send(filepath, bufrev);
        sleep(1);
    }
    else
    {
        sprintf(data.message.str,"%s/%s/%s/%s/%s/%d/%s\n",l.title,l.id,l.time,l.title,l.text,l.flag,"none");
        build_packet(&packet,enum_bpost,data);
        queue_push(write_queue,packet);
    }
    //刷新服务器
    //on_button_clicked_frash();
    build_packet(&packet,enum_blist,data);
    queue_push(write_queue,packet);
    while (1)
    {
        if (bbs_flag)
        {
            queue_pop(read_queue,&packet);
            parse_packet(packet,&kind,&data);
            if (kind==enum_blist)
            {
                int num;
	            char *revbuf[6] = {0};
                split(data.message.str,"/",revbuf,&num); 
                if (num==6)
                {
                    strcpy(bbs_list[--bbs_num].id,revbuf[0]);
                    strcpy(bbs_list[bbs_num].time,revbuf[1]);
                    strcpy(bbs_list[bbs_num].title,revbuf[2]);
                    strcpy(bbs_list[bbs_num].text,revbuf[3]);
                    bbs_list[bbs_num].flag = atoi(revbuf[4]);
                    printf("%d,%s\n",bbs_num,bbs_list[bbs_num].title);
                }
                if (bbs_num==0)
                {
                    break;
                }
                
            }
            else
            {
                queue_push(read_queue,packet);
            }
        }
    }
    bbs_flag = 0;

    show_list(bbs_list);
}



void title_id(char *id)
{
    fixed = gtk_fixed_new(); 
    gtk_widget_set_size_request(fixed,400,20);//width,height
    //gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,FALSE,0);
    gtk_container_add(GTK_CONTAINER(fixed1), fixed);

    label1=gtk_label_new("Bit bbs");
    gtk_fixed_put(GTK_FIXED(fixed), label1, 0,5);

    label2=gtk_label_new(id);
    gtk_fixed_put(GTK_FIXED(fixed), label2, 360,5);

    button = gtk_button_new_with_label("刷新");
    gtk_fixed_put(GTK_FIXED(fixed), button, 320,0); 
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked_frash),NULL);

    //gtk_widget_show_all(fixed1);
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

void bbs_main(char *user, list *list)
{
    bbs_username=user;
    gtk_init(NULL,NULL);
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"Bit bbs");
    gtk_widget_set_size_request(window,400,620);
    //bbs_set_background(window,400,800,"background.jpg");
    g_signal_connect (G_OBJECT (window), "destroy",G_CALLBACK (gtk_main_quit), NULL);

    fixed1 = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window),fixed1);    //框架
    title_id(user);
    
    show_list(list);    

    sep = gtk_hseparator_new();//分割线
    gtk_fixed_put(GTK_FIXED(fixed), sep, 0,520);

    fixed = gtk_fixed_new(); 
    gtk_widget_set_size_request(fixed,400,150);//width,height
    //gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,FALSE,0);
    gtk_fixed_put(GTK_FIXED(fixed1), fixed, 0, 390);

    label1=gtk_label_new("发表新帖");
    gtk_fixed_put(GTK_FIXED(fixed), label1, 25,0);

    entry = gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry, 25, 25);
    gtk_entry_set_max_length(GTK_ENTRY(entry), 350);

    text=gtk_text_view_new();
    gtk_widget_set_size_request(text,350, 100);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text),TRUE);
    gtk_fixed_put(GTK_FIXED(fixed),text,25,70);

    GtkWidget *button4 = gtk_button_new_with_label("上传文件");
    gtk_fixed_put(GTK_FIXED(fixed), button4, 25,180); 
    g_signal_connect(G_OBJECT(button4),"clicked",G_CALLBACK(on_button_clicked_upload),NULL);

    GtkWidget *button5 = gtk_button_new_with_label("发帖");
    gtk_fixed_put(GTK_FIXED(fixed), button5, 100,180); 
    g_signal_connect(G_OBJECT(button5),"clicked",G_CALLBACK(on_button_clicked_post),NULL);

    gtk_widget_show_all(window);
    gtk_main();
    //return FALSE;
}