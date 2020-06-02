#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

QueueInfo_st *createQueue(void)
{
    QueueInfo_st *queue = (QueueInfo_st *)malloc(sizeof(QueueInfo_st));
    if(NULL == queue)
    {
        printf("malloc failed\n");
        return NULL;
    }
    queue->next = NULL;
    return queue;
}
/*入队列，0表示成，非0表示出错*/
int queue_push(QueueInfo_st *s,ElementType value)
{
    QueueInfo_st *temp = (QueueInfo_st *)malloc(sizeof(QueueInfo_st));
    if(NULL == temp)
    {
        printf("malloc failed\n");
        return FAILURE;
    }
    while(s->next != NULL)
    {
        s = s->next;
    }

    temp->value = value;
    temp->next = s->next;
    s->next = temp;

    return SUCCESS;
}
int queue_pop(QueueInfo_st *s,ElementType *value)
{
    if(!queue_is_empty(s))
        return FAILURE;
    *value = s->next->value;
    QueueInfo_st *temp = s->next;
    s->next = s->next->next;
    if(temp!=NULL)
        free(temp);
    temp = NULL;

    return SUCCESS;
}
int queue_top(QueueInfo_st *s,ElementType *value)
{
    if(!queue_is_empty(s))
    {
        return FAILURE;
    }
    *value = s->next->value;
    return SUCCESS;
}
int queue_is_empty(QueueInfo_st *s)
{
    if(s->next == NULL)
    {
        return SUCCESS;
    }

    return FAILURE;
}