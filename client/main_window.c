#include <gtk/gtk.h>	// 头文件
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <glib.h>
#include "chat.h"
#include "chatting_together_window.h"
#include "queue.h"
#include "hashmap.h"
#include "bbs_main.h"

extern int client_socket;

char *username="fwx";
char file_name[30]="friend_list";
//g_thread_m
GtkWidget *window;
GtkWidget *vbox;
GtkWidget *hbox1;
GtkWidget *hbox2;
GtkWidget *hbox3;
GtkWidget *vbox1;
GtkWidget *vbox1_0;
GtkWidget *vbox1_1;
GdkPixbuf *pix;
GdkPixbuf *pixnew;
GtkWidget *image;
GtkWidget *label1;
GtkWidget *search_friend;
GtkButton *button;
GtkWidget *frame;
GtkWidget *scrolled;

int bbs_open = 0;
FILE *fp;
int count = -1;
//char *id;
int friend_num_now=0;
GtkButton *button_friend[MAX_Friend];
int chatting_num=0;
pthread_t thIDr,thIDw;


int docu_rece(char* file_name)
{
    char  file_buff[4096];
    FILE *fp;
    int  n;
    Kind kind;
    Data data;
    Packet packet;
    pthread_mutex_lock(&mtx);
    read(client_socket, &packet, sizeof(Packet));
    parse_packet(packet, &kind, &data);
    int size = atoi(data.message.str);
    int now = 0;
    char path[50];
    sprintf(path,"file/%s",file_name);
    if((fp = fopen(path,"wb") ) == NULL)
    {
        printf("new file create fail.\n");
        return 0;
    }
    while(now < size){
        if (size - now >= 4096)
        {
           n = read(client_socket, file_buff, sizeof(file_buff));
        }
        else
        {
            n = read(client_socket, file_buff, size-now);
        }
        fwrite(file_buff, 1, n, fp);//将缓冲区内容写进文件
        now += n;
    }
    fclose(fp);
    pthread_mutex_unlock(&mtx);
    return 1;
}

void pop_document(GtkWindow *parent,Data data){
	Packet packet;
	gdk_threads_enter(); 
	GtkWidget *dialog;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *hbox;
    dialog = gtk_dialog_new_with_buttons("TeliTalk",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
    char message_str[500];
    strcpy(message_str,data.message.id_from);
    strcat(message_str,"给您发送了文件"); 
	strcat(message_str,data.message.str);
    label = gtk_label_new(message_str);
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
    hbox = gtk_hbox_new(FALSE,5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
    gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
    gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
    gtk_widget_show_all(dialog);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    switch(result)
    {
        case GTK_RESPONSE_OK:
            build_packet(&packet,enum_fyes,data);
            write(client_socket,&packet,sizeof(Packet));
            docu_rece(data.message.str);
            break;
        case GTK_RESPONSE_CANCEL:
            gtk_widget_destroy(dialog);
            gdk_threads_leave(); 
            return;
        default:
            g_print("something wrong!\n");
            break;
    }
    gtk_widget_destroy(dialog);
	gdk_threads_leave();
}
void pop_friend(GtkWindow *parent,int flag,char *id_to)
{
    gdk_threads_enter(); 
    GtkWidget *dialog;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *hbox;
    Packet packet;
    Data data;
    if(flag==1)
    {
        dialog = gtk_dialog_new_with_buttons("error",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("该人不存在");
    }
    else if(flag==0)
    {
        dialog = gtk_dialog_new_with_buttons("TeliTalk",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("好友请求已发送");
    }
    else if(flag==2)
    {
        GtkWidget *dialog;
        gint result;
        dialog = gtk_dialog_new_with_buttons("好友请求",NULL,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,NULL);
        gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_OK);
        char friend_str[50];
        strcpy(friend_str,id_to);
        strcat(friend_str,"想添加您为好友");
        label = gtk_label_new(friend_str);
        gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),label);
        gtk_widget_show_all(dialog);
        result = gtk_dialog_run(GTK_DIALOG(dialog));
        switch(result)
        {
            case GTK_RESPONSE_OK:
                strcpy(data.message.str,"accept");
                g_print("ok\n");
                update_new(id_to);
                gtk_widget_show_all(window);
                fp=fopen(file_name,"a+");
                fputs(id_to,fp);
                fprintf(fp,"\n");
                fclose(fp);
                break;
            case GTK_RESPONSE_CANCEL:
                gtk_widget_destroy(dialog);
                gdk_threads_leave(); 
                return;
                //break;
            default:
                g_print("something wrong!\n");
                break;
        }
        strcpy(data.message.id_from,username);
        strcpy(data.message.id_to,id_to);
        build_packet(&packet,enum_friend,data);
        queue_push(write_queue,packet);
        //write(client_socket, &packet, sizeof(Packet));
        gtk_widget_destroy(dialog);
        gdk_threads_leave(); 
        return ;
    }
    else if (flag == 3)
    {
        dialog = gtk_dialog_new_with_buttons("TeliTalk",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("对方已添加您为好友");
        update_new(id_to);
        gtk_widget_show_all(window);
        fp=fopen(file_name,"a+");
        fputs(id_to,fp);
        fprintf(fp,"\n");
        fclose(fp);
    }
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
    hbox = gtk_hbox_new(FALSE,5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
    gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
    gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    gdk_threads_leave();
}
void pop_message(GtkWindow *parent,int flag,char *id_from)
{
    gdk_threads_enter(); 
    GtkWidget *dialog;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *hbox;
    Packet packet;
    Data data;
    if(flag==0)
    {
        dialog = gtk_dialog_new_with_buttons("TeliTalk",parent,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        //gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_OK);
        char message_str[50];
        strcpy(message_str,id_from);
        strcat(message_str,"给您发送了消息"); 
        label = gtk_label_new(message_str);
    }
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
    hbox = gtk_hbox_new(FALSE,5);
    gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
    gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
    gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    gdk_threads_leave();
}

void search(char *to_username)
{
    Kind kind=enum_friend;
    Data data;
    Packet packet;
    strcpy(data.message.id_from,username);
    strcpy(data.message.str,"add_friend");
    strcpy(data.message.id_to,to_username);
    if(!strcmp(data.message.id_to,data.message.id_from)) 
    {
        GtkWidget *dialog;
        GtkWidget *label;
        GtkWidget *hbox;
        dialog = gtk_dialog_new_with_buttons("error",window,GTK_DIALOG_MODAL,GTK_STOCK_OK,GTK_RESPONSE_OK,NULL);
        label = gtk_label_new("不能添加自己");
        gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
        hbox = gtk_hbox_new(FALSE,5);
        gtk_container_set_border_width(GTK_CONTAINER(hbox),10);
        gtk_box_pack_start_defaults(GTK_BOX(hbox),label);
        gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),hbox);
        gtk_widget_show_all(dialog);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    build_packet(&packet,kind,data);
    queue_push(write_queue,packet);
    //write(client_socket, &packet, sizeof(Packet));
}
void on_button_clicked_search(GtkWidget* button,gpointer data)
{
    gchar* searchid = gtk_entry_get_text(GTK_ENTRY(search_friend));
    search((char*)searchid);
}
void on_button_clicked_chat(GtkWidget* button)
{
    const char*friend_name = gtk_button_get_label(button);
    friend_chatting[hash_table_lookup(hashTable,friend_name)->nValue] = 1;
    chatting_win(friend_name);
}
//chat_together

void on_button_clicked_chat_together(GtkWidget* button)
{
    chatting_win_together();
}

void destroy_logout()
{
    Kind kind=enum_logout;
    Data data;
    Packet packet;
    strcpy(data.userinfo.account,username);
    build_packet(&packet,kind,data);
    queue_push(write_queue,packet);
    //write(client_socket, &packet, sizeof(Packet));
    gtk_main_quit();
}
void split(char *src,const char *separator,char **dest,int *num) {
     char *pNext;
     int count = 0;
     if (src == NULL || strlen(src) == 0)
        return;
     if (separator == NULL || strlen(separator) == 0)
        return;
     pNext = (char *)strtok(src,separator);
     while(pNext != NULL) {
          *dest++ = pNext;
          ++count;
         pNext = (char *)strtok(NULL,separator); 
    }  
    *num = count;
} 	
void bbs_window()
{
    bbs_flag = 0;
    bbs_detail_flag = 0;
    Data data;
    Packet packet;
    build_packet(&packet,enum_blist,data);
    queue_push(write_queue,packet);
    while(1)
    {
        if (bbs_flag)
        {
            bbs_open = 1;
            bbs_flag = 0;
            bbs_main(username, bbs_list);
            bbs_open = 0;
            break;
        }
    }
    bbs_flag = 0;
}
void read_from()
{
    Kind kind;
    Data data;
    Packet packet;
    while(1)
    {
        pthread_mutex_lock(&mtx);
		if(read(client_socket, &packet, sizeof(Packet))<0)
		{
			perror("fail to recv");    //把"fail to recv"输出到标准错误stderr。
			close(client_socket);    //关闭socket端口。
			exit(1);
		}
        pthread_mutex_unlock(&mtx);
		parse_packet(packet,&kind,&data);
        printf("%d,%s\n",kind,data.message.str);
        if(kind==enum_friend&&(!strcmp(data.message.str,"1"))) pop_friend(window,1,data.message.id_from);
        else if(kind==enum_friend&&(!strcmp(data.message.str,"add_friend"))) pop_friend(window,2,data.message.id_from);
        else if(kind==enum_friend&&(!strcmp(data.message.str,"accept"))) pop_friend(window,3,data.message.id_from);
        else if(kind==enum_friend) pop_friend(window,0,data.message.id_from);
        else if(kind==enum_chat){
            char buf[100];
            strcpy(buf,data.message.id_from);
            strcat(buf,":\n\t");
            strcat(buf,data.message.str);
            strcat(buf,"\n");
            char path[50] = "./history/";
            strcat(path,data.message.id_from);
            FILE *fp = fopen(path,"a+");
            fputs(buf,fp);
            fclose(fp);
            if( friend_chatting[hash_table_lookup(hashTable,data.message.id_from)->nValue] == 1)
            {
                queue_push(read_queue,packet);
            }
            else
            {
                pop_message(window,0,data.message.id_from);
                queue_push(read_queue,packet);
            }
        }
        else if(kind==enum_file)
        {
            pop_document(window,data);
        }
        else if(kind==enum_blist && bbs_open == 0)
        {
            if (count == -1)
            {
                count = atoi(data.message.str);
            }
            else if(count != 0)
            {
                int num;
	            char *revbuf[6] = {0};
                split(data.message.str,"/",revbuf,&num); 
                printf("num:%d",num);
                if (num==6)
                {
                   strcpy(bbs_list[--count].id,revbuf[0]);
                   strcpy(bbs_list[count].time,revbuf[1]);
                   strcpy(bbs_list[count].title,revbuf[2]);
                   strcpy(bbs_list[count].text,revbuf[3]);
                   bbs_list[count].flag = atoi(revbuf[4]);
                }
                printf("count:%d\n",count);
                if(count==0)
                {
                    bbs_flag = 1;
                    count=-1;
                }
            }
            else
            {
                bbs_flag = 1;
                count=-1;
            }
        }
        else if(kind==enum_blist && bbs_open == 1)
        {
            if (count == -1)
            {
                if (strcmp(data.message.str,""))
                {
                    count = atoi(data.message.str); 
                    bbs_num = count;
                }
            }
            else if(count != 0)
            {
                --count;
                queue_push(read_queue,packet);
                if(count==0)
                {
                    bbs_flag = 1;
                    count=-1;
                }
            }
        }
        else if(kind==enum_bcont)
        {
            if (count == -1)
            {
                if (strcmp(data.message.str,""))
                {
                    count = atoi(data.message.str); 
                    bbs_detail_num = count;
                }
            }
            else if(count != 0)
            {
                --count;
                queue_push(read_queue,packet);
                if(count==0)
                {
                    bbs_detail_flag = 1;
                    count=-1;
                }
            }
        }
        sleep(0.1);
    }
}
void write_to()
{
    Kind kind;
    Data data;
    Packet packet;
    while(1)
    {
        if(queue_is_empty(write_queue))
        {
            queue_pop(write_queue,&packet);
            printf("write packet\n");
            write(client_socket, &packet, sizeof(Packet));
        }
        sleep(0.1);
    }
}
void set_background(GtkWidget *widget,int w,int h,gchar *path)
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
void personal_id(char *id)
{
    hbox1=gtk_hbox_new(FALSE,30);
    gtk_widget_set_size_request(hbox1,100,150);//width,height
    gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,FALSE,0);
    pix=gdk_pixbuf_new_from_file("portrait.png",NULL);
    pixnew=gdk_pixbuf_scale_simple(pix,100,100,GDK_INTERP_BILINEAR);//图像对象
    image=gtk_image_new_from_pixbuf(pixnew);
    gtk_box_pack_start(GTK_BOX(hbox1),image,FALSE,FALSE,10);
    GtkWidget *vbox1_0=gtk_vbox_new(FALSE,0);
    label1=gtk_label_new(id);
    PangoFontDescription *font_desc=pango_font_description_from_string("Serif 15");//设置字体
    pango_font_description_set_size(font_desc,30*PANGO_SCALE);//设置标签字大小
    gtk_widget_modify_font(label1,font_desc);
    pango_font_description_free(font_desc);
    gtk_box_pack_start(GTK_BOX(vbox1_0),label1,FALSE,FALSE,50);
    gtk_box_pack_start(GTK_BOX(hbox1),vbox1_0,FALSE,FALSE,0);
}
void update_new(char *friend)//新建联系人
{
    GtkWidget *hbox4_0=gtk_hbox_new(FALSE,10);
    gtk_widget_set_size_request(hbox4_0,0,100);
    gtk_box_pack_start(GTK_BOX(vbox1_1),hbox4_0,FALSE,FALSE,0);
    
    //联系人头像图片
    GdkPixbuf *pix0;
    pix0=gdk_pixbuf_new_from_file("friends.png",NULL);
    GdkPixbuf *pixnew0;
    pixnew0=gdk_pixbuf_scale_simple(pix0,60,60,GDK_INTERP_BILINEAR);
    GtkWidget *image0;
    image0=gtk_image_new_from_pixbuf(pixnew0);
    gtk_box_pack_start(GTK_BOX(hbox4_0),image0,FALSE,FALSE,20);

    //联系人id
    //GtkWidget *vbox4_0=gtk_vbox_new(FALSE,0);
    GtkWidget *label4_0;

    // gtk_widget_modify_font(label4_0,font_desc);
    // pango_font_description_free(font_desc);
    // gtk_box_pack_start(GTK_BOX(vbox4_0),label4_0,FALSE,FALSE,20);
    // gtk_box_pack_start(GTK_BOX(hbox4_0),vbox4_0,FALSE,FALSE,0);
    friend_num_now++;
    button_friend[friend_num_now]=gtk_button_new_with_label(friend);
    hash_table_insert(hashTable, friend,friend_num_now);
    g_signal_connect(G_OBJECT(button_friend[friend_num_now]),"clicked",G_CALLBACK(on_button_clicked_chat),NULL);
    gtk_button_set_relief(button_friend[friend_num_now],GTK_RELIEF_NONE);
    PangoFontDescription *font_desc=pango_font_description_from_string("Serif 15");
    pango_font_description_set_size(font_desc,15*PANGO_SCALE);
    label4_0=gtk_bin_get_child(GTK_WIDGET(button_friend[friend_num_now]));
    gtk_widget_modify_font(GTK_WIDGET(label4_0),font_desc);

    gtk_container_add(GTK_CONTAINER(hbox4_0),button_friend[friend_num_now]);
    gtk_widget_show_all(window);
}
void test()
{
    while(1)
    {
        printf("cor\n");
        gtk_button_set_relief(button_friend[1],GTK_RELIEF_HALF);
        sleep(2);
        printf("corrent\n");
        gtk_button_set_relief(button_friend[1],GTK_RELIEF_NONE);
        sleep(2);
    }
}
void main_win(char *user)
{
    hash_table_init(hashTable);
    write_queue = createQueue();
    read_queue = createQueue();
    pthread_create(&thIDr, NULL, (void *)read_from, NULL);
    pthread_create(&thIDw, NULL, (void *)write_to, NULL);
    username=user;
    gdk_threads_init();
    gtk_init(NULL,NULL);
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window),"联系人列表");
    gtk_widget_set_size_request(window,400,950);
    set_background(window,400,950,"background.jpg");
    g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(destroy_logout),NULL);
    
    //总box框架&个人id栏
    vbox=gtk_vbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(window),vbox);
    personal_id(user);

    //搜索好友栏
    hbox2=gtk_hbox_new(FALSE,0);//box内部控件间距离
    gtk_widget_set_size_request(hbox2,300,40);
    gtk_box_pack_start(GTK_BOX(vbox),hbox2,FALSE,FALSE,0);
    search_friend=gtk_entry_new();
    gtk_entry_set_max_length(search_friend,150);
    gtk_box_pack_start(GTK_BOX(hbox2),search_friend,FALSE,FALSE,20);
    button=gtk_button_new_with_label("search");
    g_signal_connect(G_OBJECT(button),"clicked",G_CALLBACK(on_button_clicked_search),NULL);
    gtk_box_pack_start(GTK_BOX(hbox2),button,FALSE,FALSE,0);
    GtkWidget *button1=gtk_button_new_with_label("广场入口");
    gtk_box_pack_start(GTK_BOX(hbox2),button1,FALSE,FALSE,50);
    g_signal_connect(G_OBJECT(button1),"clicked",G_CALLBACK(on_button_clicked_chat_together),NULL);

    //群聊接口
    hbox3=gtk_hbox_new(FALSE,0);
    gtk_widget_set_size_request(hbox3,400,50);
    gtk_box_pack_start(GTK_BOX(vbox),hbox3,FALSE,FALSE,0);
    GtkWidget *button2=gtk_button_new_with_label("论坛入口");
    gtk_widget_set_size_request(button2,360,40);
    // gtk_button_set_relief(button1,GTK_RELIEF_NONE);
    gtk_box_pack_start(GTK_BOX(hbox3),button2,FALSE,FALSE,20);
    g_signal_connect(G_OBJECT(button2),"clicked",G_CALLBACK(bbs_window),NULL);
    
    //好友列表
    vbox1=gtk_vbox_new(FALSE,0);
    gtk_widget_set_size_request(vbox1,0,0);
    gtk_box_pack_start(GTK_BOX(vbox),vbox1,TRUE,TRUE,0);
    frame=gtk_frame_new("好友列表");
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
    gtk_widget_set_size_request(vbox1_1,350,2000);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),vbox1_1);
    //
    char buff[30];
    strcat(file_name,"_");
    strcat(file_name,username);
    strcat(file_name,".txt");
    fp=fopen(file_name,"a+");
    printf("%s\n",file_name);
    while(1)
    {
        fscanf(fp,"%s",buff);
        if(feof(fp)) break;
        printf("%s\n",buff);
        update_new(buff);
    }
    pthread_t t;
    //pthread_create(&t,NULL,(void*)test,NULL);
    fclose(fp);
    gtk_widget_show_all(window);
    gtk_main();
    return ;
}
