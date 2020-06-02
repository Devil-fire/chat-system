#include "chat.h"

typedef Packet ElementType;

#define SUCCESS 0
#define FAILURE -1

typedef struct QueueInfo
{
    ElementType value;
    struct QueueInfo *next;
}QueueInfo_st;

QueueInfo_st * write_queue;
QueueInfo_st * read_queue;

QueueInfo_st *createQueue(void);
int queue_push(QueueInfo_st *s,ElementType value);
int queue_pop(QueueInfo_st *s,ElementType *value);
int queue_top(QueueInfo_st *s,ElementType *value);
int queue_is_empty(QueueInfo_st *s);
