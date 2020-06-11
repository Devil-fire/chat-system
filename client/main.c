#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "login_window.h" 
#include "chat.h"
char *str_ip;//存放ip

int main(int argc,char* argv[])
{   
    pthread_mutex_init(&mtx, NULL);
    str_ip=(char*)argv[1];
    system("mkdir file");
    system("mkdir history");
    loginpage(argc,argv);
    return 0;
}