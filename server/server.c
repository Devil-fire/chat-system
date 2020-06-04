#include "chat.h"
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXMSG 500            /*最大消息数*/
#define MAXUSER 20            /*用户最大数量*/
#define RW 0                  /*读写进程对共享内存区的信号量  初始值：1*/
#define MUTEX 1               /*计数器信号量                 初始值：1*/
#define W 2                   /*为了写进程优先设置的信号量   初始值：1  2*/
#define COUNT 3               /*读进程数量                   初始值：0*/
#define FILESEM 4             /*文件访问信号量               初始值：1*/

union semun{                  /*信号量处理必需的共用体*/
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

typedef struct _space{        /*共享缓存区*/
    int length;               /*当前消息条数*/
    // Message message[MAXMSG];  /*消息条数上限*/
    Packet packet[MAXMSG];
    int online[MAXUSER + 1];  /*在线用户列表，id*/
}Space;

Space *space;                 /*共享缓存区*/
int client_socket;            /*客户端套接字*/
int server_socket;            /*服务器套接字*/
char client_ip[20];           /*客户端IP的字符串表示*/
int shmid;                    /*共享内存区(多人聊天)标识符(ID)*/
int semid;                    /*信号量标识符(ID)*/
char fork_name[20];           /*关键变量，用于标示当前fork的用户名*/
int fork_id;
char  file_buff[4096];

int init_socket(int port,int addr);
/*初始化套接字，传入端口和地址，自动生成一个套接字并关联地址，监听。返回套接字。若失败，返回-1*/
void do_server();
/*子进程对客户端的具体处理，使用多线程，用两个线程分别监控客户端的输入和输出到客户端*/
void exitfunc(int signal);
/*退出处理函数，捕获SIGINT信号。关闭信号量和共享内存区，保存历史聊天记录*/
void waitchild(int signal);
/*子进程退出处理函数，捕获SIGCHIL信号，防止出现僵尸进程*/
int init_sem(int rw,int mutex,int w,int count,int file);
/*设置信号量集中五个信号量的初值，失败返回-1*/
int P(int type);
/*P操作，将信号量集合中对应类型的信号量的数量-1，失败返回-1*/
int V(int type);
/*V操作，将信号量集合中对应类型的信号量的数量+1，失败返回-1*/
int sem_setval(int type,int value);
/*设置信号量type的值为value，失败返回-1*/
void read_from();
/*从客户端接收包*/
void write_to();
/*向客户端发送包*/
void handle_packet(Packet packet);
/*处理包*/
void regist(User user);
/*处理注册请求*/
void login(User user);
/*处理登陆请求*/
void chat_with(Packet packet);
/*发送信息处理*/
int make_friend(Message message);
/*好友申请处理*/
int docu_rece();
/*接收客户端传来的文件，缓存*/
int docu_send();
/*向客户端发送缓存的文件*/

int main(){
    struct in_addr client_addr;
    struct sockaddr_in mysock;
    int length;
    int len;
    char *addr;
    signal(SIGINT, exitfunc); /*设置函数捕获并处理Ctrl+C按下时的信号*/
    signal(SIGCHLD, waitchild); /*子进程退出后wait它防止出现僵尸进程*/
    server_socket = init_socket(MYPORT, INADDR_ANY);
    shmid = shmget(IPC_PRIVATE, sizeof(Space), IPC_CREAT|0660); /*创建一个共享内存区*/
    if(shmid == -1)
    {
        printf("shared memeoy created failed.\n");
        return -1;
    }
    space=(Space*)shmat(shmid, NULL, 0);
    if((int)space == -1)
    {
        printf("shared memeoy matched failed.\n");
        return -1;
    }
    semid=semget(IPC_PRIVATE, 5, IPC_CREAT|0660);                /*创建一个有5个信号量的信号量集*/
    if(semid == -1)
    {
        printf("semaphore created failed!\n");
        return -1;
    }
    if(init_sem(1, 1, 1, 0, 1) == -1)                /*将5个信号量初始化值为1 1 1 0 1*/
    {
        printf("semaphore initilize failed!\n");
        return -1;
    }

    printf("Wating for connection......\n");
    while(1)
    {
        length = sizeof(mysock);
        client_socket = accept(server_socket, (struct sockaddr*)&mysock, &length);   /*接收连接请求*/
        if(client_socket != -1)
        {
            if(fork() == 0)                                                     /*子进程进行具体处理*/
            {
		        int len = sizeof(client_addr);
                getpeername(client_socket, (struct sockaddr*)&client_addr, &len);
                strcpy(client_ip, inet_ntoa(client_addr));
                printf("Connect succeed!\n");
                printf("Client ip:%s\n", client_ip);
                do_server();/*进行具体交互*/
            }
            else
            {                                                                   /*父进程关闭客户端套接字，继续监听*/
                close(client_socket);
                strcpy(client_ip, "");
            }
        }
        else printf("connect failed!\n");
    }
}

void regist(User user){
    FILE *fp;
    int flag = 0;
    int same = 0;
    char line[100];
    Packet packet;
    P(FILESEM);
    fp = fopen("userinfo.txt", "r+");
    if(fp == NULL)
    {
        printf("cannot load file\n");
    }
    while (!feof(fp))
    {
        fgets(line, 100, fp);
        if(flag == 0)
        {
            flag = 1;
            if(strncmp(line, user.account, strlen(user.account)) == 0)/*用户名重复*/
            {
                if(build_packet(&packet, falacc, 0) == -1)
                {
                    printf("fail to build the packet!\n");
                    return;
                }
                write(client_socket,&packet,sizeof(Packet));
                printf("%s regist fail.\n", user.account);
                same = 1;
                break;
            }
        }
        else
        {
            flag = 0;
        }
    }
    fclose(fp);
    if(same == 0)
    {
        fp = fopen("userinfo.txt", "at+");
        if(fp == NULL)
        {
            printf("cannot load file\n");
        }
        fputs(user.account, fp);
        fputs("\n", fp);
        fputs(user.password, fp);
        fputs("\n", fp);
        if(build_packet(&packet, regsuc, 0) == -1)
        {
            printf("fail to build the packet!\n");
            return;
        }
        write(client_socket,&packet,sizeof(Packet));
        printf("%s regist succeed.\n", user.account);
        fclose(fp);
    }
    V(FILESEM);
    return;
}

void login(User user){
    FILE *fp;
    int flag = 0;
    int id = -1;
    int correct = 0;
    char line[100];
    Packet packet;
    P(FILESEM);
    fp = fopen("userinfo.txt", "rt");
    if(fp == NULL)
    {
        printf("cannot load file\n");
    }
    while (!feof(fp)) {
        fgets(line, 100, fp);
        if(flag == 0)
        {
            id++;
            flag = 1;
            if(strncmp(line, user.account, strlen(user.account)) == 0)/*用户名存在*/
            {
                if(!feof(fp))
                {
                    flag = 0;
                    fgets(line, 100, fp);
                }
                if(strncmp(line, user.password, strlen(user.password)) == 0)/*密码匹配*/
                {
                    if(build_packet(&packet, logsuc, 0) == -1)
                    {
                        printf("fail to build the packet!\n");
                        return;
                    }
                    write(client_socket,&packet,sizeof(Packet));
                    printf("%s login succeed.\n", user.account);
                    strcpy(fork_name, user.account);
                    fork_id = id;
                    correct = 1;
                    break;
                }
                else/*密码错误*/
                {
                    if(build_packet(&packet, falpwd, 0) == -1)
                    {
                        printf("fail to build the packet!\n");
                        return;
                    }
                    write(client_socket,&packet,sizeof(Packet));
                    printf("%s login fail for wrong password.\n", user.account);
                    break;
                }
            }
        }
        else
        {
            flag = 0;
        }
    }
    if(correct == 0)
    {
        if(build_packet(&packet, falacc, 0) == -1)/*用户不存在*/
        {
            printf("fail to build the packet!\n");
            return;
        }
        write(client_socket,&packet,sizeof(Packet));
        printf("%s login fail for not existing.\n", user.account);
    }
    fclose(fp);
    V(FILESEM);
    return;
}

void chat_with(Packet packet){
    int msglength;
    P(W);                                           /*在无写进程时进入*/
    P(RW);                                          /*互斥访问共享内存区*/
    msglength = space->length%MAXMSG;               /*更新内存区,超过500条则覆盖前面的，写的位置永远是space->length%500*/
    space->packet[msglength] = packet;
    space->length++;
    V(RW);                                          /*释放共享内存区信号量*/
    V(W);
    return;
}

int make_friend(Message message){
    FILE *ffp, *ufp;
    int flag = 0;
    char line[100];
    Packet packet;
    int i, j;
    int fri_matrix[20][20] = {0};
    int id = -1;
    int idto = -1;
    int idfrom = fork_id;
    P(FILESEM);
    ffp = fopen("friend.txt", "r+");
    if(ffp == NULL)
    {
        printf("cannot load file\n");
    }
    for(i = 0; i < 20; i++){
    	  fgets(line, 100, ffp);
        for(j = 0; j < 20; j++){
            fri_matrix[i][j] = line[j] - 48;
		    }
    }
    fclose(ffp);
    printf("first:\n");
    for(i = 0; i < 20; i++){
        for(j = 0; j < 20; j++){
            printf("%d ", fri_matrix[i][j]);
        }
        printf("\n");
    }
    ufp = fopen("userinfo.txt", "at+");
    if(ufp == NULL)
    {
        printf("cannot load file\n");
    }
    while (!feof(ufp)) {
        fgets(line, 100, ufp);
        if(flag == 0){
            flag = 1;
            id++;
            if(strncmp(line, message.id_to, strlen(message.id_to)) == 0){
                idto = id;
            }
        }else{
            flag = 0;
        }
    }
    fclose(ufp);
    flag = 0;
    if(idto != -1 && idto != idfrom){
        if(fri_matrix[idfrom][idto] == 0 && fri_matrix[idto][idfrom] == 0){
            fri_matrix[idfrom][idto] = 1;
            flag = 1;
            printf("%s asks %s for being friend.\n", message.id_from, message.id_to);
        }
        else if(fri_matrix[idfrom][idto] == 0 && fri_matrix[idto][idfrom] == 1){
            fri_matrix[idfrom][idto] = 1;
            flag = 2;
            printf("%s agrees asking from %s.\n", message.id_from, message.id_to);
        }
        else if(fri_matrix[idfrom][idto] == 1 && fri_matrix[idto][idfrom] == 1){
            flag = 0;
            printf("%s and %s are friends allready.\n", message.id_from, message.id_to);
        }
    }
    else{
        flag = 0;
        strcpy(message.str, "wrong name for friend.");
        if(build_packet(&packet, enum_friend, message) == -1)
        {
            printf("fail to build the packet!\n");
            return -1;
        }
        write(client_socket,&packet,sizeof(Packet));
        /*当请求的好友不存在或请求的好友为自己时，发回类型为friend，内容为wrong name for friend.的包*/
        printf("%s gives wrong name for friend.\n", message.id_from);
    }
    ffp = fopen("friend.txt", "w+");
    if(ffp == NULL)
    {
        printf("cannot load file\n");
    }
    for(i = 0; i < 20; i++){
        for(j = 0; j < 20; j++){
            line[j] = fri_matrix[i][j] + 48;
		    }
        fputs(line, ffp);
    }
    fclose(ffp);
    printf("second:\n");
    for(i = 0; i < 20; i++){
        for(j = 0; j < 20; j++){
            printf("%d ", fri_matrix[i][j]);
        }
        printf("\n");
    }
    V(FILESEM);
    return flag;
}
int file_size(char* filename)
{
    struct stat statbuf;
    stat(filename,&statbuf);
    int size=statbuf.st_size;
    return size;
}
int docu_send(char *fpath){
    int len, rec_len;
    FILE *fq;
    //读取文件本地路径及文件名
    int size = file_size(fpath);
    char string[20];
    Kind kind;
	Packet packet;
	Data data;
    sprintf(string,"%d",size);
	strcpy(data.message.str,string);
	build_packet(&packet,enum_file,data);
    write(client_socket,&packet,sizeof(Packet));
    //打开文件
    if( ( fq = fopen(fpath,"rb") ) == NULL ){
        printf("File open error.\n");
        return 0;
    }

    //传输文件
    bzero(file_buff,sizeof(file_buff));
    while(!feof(fq)){
        len = fread(file_buff, 1, sizeof(file_buff), fq);
        if(len != write(client_socket, file_buff, len)){
            printf("write.\n");
            break;
        }
    }
    fclose(fq);
    return 1;
}

int docu_rece(char *file_name)
{
    FILE *fp;
    int n = 0;
    //接受客户端传过来的文件名

    Kind kind;
    Data data;
    Packet packet;
    read(client_socket, &packet, sizeof(Packet));
    parse_packet(packet, &kind, &data);
    int size = atoi(data.message.str);
    int now = 0;
    sleep(3);
    //创建待接收文件实体
    if((fp = fopen(file_name,"wb") ) == NULL )
    {
        printf("new file create fail.\n");
        return 0;
    }
    memset(file_buff,0,sizeof(file_buff));
    //把二进制文件读取到缓冲区
    while(now < size){
        n = read(client_socket, file_buff, sizeof(file_buff));
        printf("n=%d\n",n);
        fwrite(file_buff, 1, n, fp);//将缓冲区内容写进文件
        now += n;
        //if(n < 4096)
         //   break;
    }
    fclose(fp);
    return 1;
}

int recv_file(Message message){
    char path[4096] = "file/";
    strcat(path, message.str);
    int ret = docu_rece(path);
    printf("111");
    return ret;
}

void trans_file(Message message){
    char path[4096] = "file/";
    strcat(path, message.str);
    docu_send(path);
}

void handle_packet(Packet packet){
    Kind kind;
    Data data;
    parse_packet(packet, &kind, &data);
    int ret = 0;
    switch (kind) {
        case enum_regist:
            regist(data.userinfo);
            break;
        case enum_login:
            login(data.userinfo);
            break;
        case enum_chat:
            chat_with(packet);
            break;
        case enum_friend:
            ret = make_friend(data.message);
            if(ret == 1 || ret == 2){
                chat_with(packet);
            }
            break;
        case enum_file:
            ret = recv_file(data.message);
            if(ret == 1){
                chat_with(packet);
            }
            break;
        case enum_fyes:
            trans_file(data.message);
            break;
        default:
            printf("oops, we failed in catch your kind: %d\n", kind);
            break;
    }
    return;
}

void write_to(){
    int msglength;
    msglength = space->length;
    int count;
    Packet packet;
    Kind kind;
    Data data;
    int temp;
    while(1)
    {
        if(msglength < space->length)
        {                      /*有新消息，互斥访问共享内存*/
            P(W);                                           /*无写进程等待进入时*/
            P(MUTEX);                                       /*对计数器加锁*/
            if((count=semctl(semid,COUNT,GETVAL)) == 0)
            {
                P(RW);
            }
            if(sem_setval(COUNT,count+1) == -1)     /*如果不是第一个进入，则表示已经对共享内存加锁了*/
            {
                printf("semaphore set value failed!\n");
            }
            V(MUTEX);                                       /*对计数器访问完毕，释放计算器信号量*/
            V(W);                                           /*释放W信号量，写进程可以进了*/
            for(;msglength<space->length;msglength++)       /*读取新消息*/
            {
                parse_packet(space->packet[msglength%MAXMSG], &kind, &data);
                printf("%s,%s\n",data.message.id_to,data.message.str);
                if(strcmp(fork_name, data.message.id_to) == 0)
                {
                    write(client_socket,&(space->packet[msglength%MAXMSG]),sizeof(Packet));
                }
            }
            P(MUTEX);                                       /*对计数器加锁*/
            count=semctl(semid,COUNT,GETVAL);               /*读进程访问完毕，计数器减1*/
            if(sem_setval(COUNT,count-1) == -1)
            {
                printf("semaphore set value failed!\n");
            }
            if(semctl(semid,COUNT,GETVAL) == 0)             /*如果是最后一个读进程，则要将共享内存区的锁解开，方便写进程进入*/
            {
                V(RW);
            }
            V(MUTEX);                                       /*计数器访问完毕，释放信号量*/
        }
        sleep(1);                                 /*每秒轮询一次*/
    }
}

void read_from(){
    printf("reading...\n");
    char str[MAXLEN+1];
    int msglength;
    Packet packet;
    Kind kind;
    Data data;
    int i;
    while(1)
    {
        if(read(client_socket, &packet, sizeof(Packet)))/*直接放进来了*/
        {
            handle_packet(packet);
        }
    }
}

void do_server(){
    pthread_t thIDr, thIDw;
    pthread_create(&thIDr, NULL, (void *)read_from, NULL);
    pthread_create(&thIDw, NULL, (void *)write_to, NULL);
    pthread_join(&thIDr, NULL);
    return;
}

void exitfunc(int signal){
    if(shmctl(shmid, IPC_RMID, 0) == -1)                    /*关闭共享内存区*/
    {
        printf("shared memory closed error!\n");
    }
    if(semctl(semid, 0, IPC_RMID, 0) == -1)               /*关闭信号量*/
    {
        printf("semaphore closed error!\n");
    }
    if(close(server_socket) == -1)                              /*关闭服务器套接字*/
    {
        printf("server_socket closed error!\n");
    }
    _exit(0);
}

void waitchild(int signal){
    wait(NULL);
}

int init_sem(int rw, int mutex, int w, int count, int file){
    union semun arg;
    int flag;
    arg.array = (unsigned short*)malloc(sizeof(unsigned short) * 5);
    arg.array[RW] = rw;                                           /*初值为1*/
    arg.array[MUTEX] = mutex;                                     /*初值为1*/
    arg.array[W] = w;                                             /*初值为1*/
    arg.array[COUNT] = count;                                     /*初值为0*/
    arg.array[FILESEM] = file;                                    /*初值为1*/
    flag = semctl(semid, 0, SETALL, arg);                   /*给5个信号量赋初值*/
    free(arg.array);
    return flag;
}

int P(int type){
    struct sembuf buf;
    buf.sem_num = type;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;
    return semop(semid, &buf, 1);
}

int V(int type){
    struct sembuf buf;
    buf.sem_num = type;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;
    return semop(semid, &buf, 1);
}

int sem_setval(int type, int value){
    union semun arg;
    arg.val = value;
    return semctl(semid, type, SETVAL, arg);
}

int init_socket(int port, int addr){
    struct sockaddr_in server_addr;             /*服务器地址结构*/
    int server_socket;                          /*服务器套接字*/
    server_socket=socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1)
    {
        return -1;
    }
    server_addr.sin_port = htons(port);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(addr);    /*套接字关联地址*/
    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        return -1;
    }
    if(listen(server_socket, 5) != 0)            /*设置最大监听数并监听*/
    {
        return -1;
    }
    return server_socket;
}
