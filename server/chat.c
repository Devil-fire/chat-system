#include "chat.h"

int build_packet(Packet *packet,Kind kind,...){
	va_list ap;			//va_list 一个指向当前参数的指针类型。
	packet->kind=kind;
	va_start(ap,kind);		//va_start 初始化va_list的对象
	switch(kind){
		case enum_regist:packet->data=(Data)va_arg(ap,User);break;
		case enum_logout:break;
		case enum_login:packet->data=(Data)va_arg(ap,User);break;
		case enum_chat:packet->data=(Data)va_arg(ap,Message);break;
		case enum_friend:packet->data=(Data)va_arg(ap,Message);break;
		case enum_file:packet->data=(Data)va_arg(ap,Message);break;
		case enum_fyes:packet->data=(Data)va_arg(ap,Message);break;
		case enum_blist:packet->data=(Data)va_arg(ap,Message);break;
		case enum_bcont:packet->data=(Data)va_arg(ap,Message);break;
		case enum_bpost:packet->data=(Data)va_arg(ap,Message);break;
		case enum_brepl:packet->data=(Data)va_arg(ap,Message);break;
		case enum_bpostf:packet->data=(Data)va_arg(ap,Message);break;
		case enum_bfyes:packet->data=(Data)va_arg(ap,Message);break;
		case regsuc:break;
		case logsuc:break;
		case falpwd:break;
		case falacc:break;
		default:return -1;
	}
	va_end(ap);	//释放指针
	return 0;
}
int parse_packet(Packet packet,Kind *kind,Data *data){
	*kind=packet.kind;
	*data=packet.data;
	return 0;
}
