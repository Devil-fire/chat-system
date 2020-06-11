main:main.o login_window.o chatting_window.o chatting_together_window.o chat.o AES.o main_window.o  queue.o hashmap.o bbs_main.o history_window.o bbs_detail.o md5.o
	gcc -o $@ $^ `pkg-config --cflags --libs gtk+-2.0`
main.o:main.c
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
login_window.o:login_window.c login_window.h
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
chatting_window.o:chatting_window.c chatting_window.h
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
chatting_together_window.o:chatting_together_window.c
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
main_window.o:main_window.c main_window.h
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
bbs_main.o:bbs_main.c bbs_main.h
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
bbs_detail.o:bbs_detail.c bbs_detail.h
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
history_window.o:history_window.c history_window.h
	gcc -c $< `pkg-config --cflags --libs gtk+-2.0`
chat.o:chat.c chat.h
	gcc -c $<
AES.o:AES.c
	gcc -c $<
md5.o:md5.c md5.h
	gcc -c $<
queue.o:queue.c queue.h
	gcc -c $<
hashmap.o:hashmap.c hashmap.h
	gcc -c $<
all:main
clean:all
	rm -f *.o
	rm *.txt
