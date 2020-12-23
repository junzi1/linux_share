#ifndef _LIST_OP_H
#define _LIST_OP_H

#ifdef __cplusplus
extern "C" {
#endif

#define IN 
#define OUT

typedef struct list_head 
{
    struct list_head *next, *prev;
}COMMON_LIST_HEAD,*COMMON_P_LIST_HEAD;

#define COMMON_LIST_HEAD_INIT(name) { &(name), &(name) }

// 定义LIST并静态初始化一个空的通用双向链表
#define COMMON_LIST_HEAD(name) \
COMMON_LIST_HEAD name = COMMON_LIST_HEAD_INIT(name)

// 动态初始化一个空的通用双向链表
#define COMMON_INIT_LIST_HEAD(ptr) do { \
(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

// 动态分配一个包含通用双向链表的结构体
#define COMMON_NEW_LIST_NODE(type, node) \
{\
    node = (type *)malloc(sizeof(type));\
}

// 释放链表中的所有节点，使该链表成为空链表
#define COMMON_FREE_LIST(type, p, list_name)\
{\
    type *posnode;\
    while(!common_list_empty(&(p)->list_name)) {\
    posnode = common_list_entry((&(p)->list_name)->next, type, list_name);\
    common_list_del((&(p)->list_name)->next);\
    free(posnode);\
    }\
}

// 获取链表中第一个节点地址(该地址指向其主结构)
#define COMMON_GetFirstNode(type,p,list_name,pGetNode)\
{\
    pGetNode = NULL;\
    while(!common_list_empty(&(p)->list_name)){\
    pGetNode = common_list_entry((&(p)->list_name)->next, type, list_name);\
    break;\
    }\
}

// 从链中删除某节点，并释放该节点所在结构占用的内存
#define COMMON_DeleteNodeAndFree(pDelNode,list_name)\
{\
    common_list_del(&(pDelNode->list_name));\
    free(pDelNode);\
}

// 仅从链中删除某节点
#define COMMON_DeleteNode(pDelNode,list_name)\
{\
    common_list_del(&(pDelNode->list_name));\
}

// 释放链表内存
#define COMMON_FreeNode(pDelNode)\
{\
    free(pDelNode);\
}


// 获取包含该通用链表节点的结构体的首址
#define common_list_entry(ptr, type, member) \
((type *)((char *)(ptr)-(size_t)(&((type *)0)->member)))

// 遍历链表
#define common_list_for_each(pos, head) \
for (pos = (head)->next; pos != (head); pos = pos->next)

/***********************************************************
*************************variable define********************
***********************************************************/

/***********************************************************
*************************function define********************
***********************************************************/

int common_list_empty(IN const COMMON_P_LIST_HEAD pHead);


void common_list_add(IN const COMMON_P_LIST_HEAD pNew, IN const COMMON_P_LIST_HEAD pHead);


void common_list_add_tail(IN const COMMON_P_LIST_HEAD pNew, IN const COMMON_P_LIST_HEAD pHead);

void common_list_splice(IN const COMMON_P_LIST_HEAD pList, IN const COMMON_P_LIST_HEAD pHead);

void common_list_del(IN const COMMON_P_LIST_HEAD pEntry);

void common_list_del_init(IN const COMMON_P_LIST_HEAD pEntry);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

