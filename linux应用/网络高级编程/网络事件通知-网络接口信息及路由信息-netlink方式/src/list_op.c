#include "list_op.h"


/***********************************************************
*  Function: __list_add 前后两节点之间插入一个新的节点
*  Input: pNew->新加入的节点
*         pPrev->前节点
*         pNext->后节点
*  Output: none
*  Return: none
***********************************************************/
static void __list_add(IN const COMMON_P_LIST_HEAD pNew, IN const COMMON_P_LIST_HEAD pPrev, IN const COMMON_P_LIST_HEAD pNext)
{
    pNext->prev = pNew;
    pNew->next = pNext;
    pNew->prev = pPrev;
    pPrev->next = pNew;
}

/***********************************************************
*  Function: __list_del 将前后两节点之间的节点移除
*  Input: pPrev->前节点
*         pNext->后节点
*  Output: none
*  Return: none
***********************************************************/
static void __list_del(IN const COMMON_P_LIST_HEAD pPrev, IN const COMMON_P_LIST_HEAD pNext)
{
    pNext->prev = pPrev;
    pPrev->next = pNext;
}

/***********************************************************
*  Function: common_list_empty 判断该链表是否为空
*  Input: pHead
*  Output: none
*  Return: none
***********************************************************/
int common_list_empty(IN const COMMON_P_LIST_HEAD pHead)
{
    return pHead->next == pHead;
}

/***********************************************************
*  Function: common_list_add 插入一个新的节点
*  Input: pNew->新节点
*         pHead->插入点
*  Output: none
*  Return: none
***********************************************************/
void common_list_add(IN const COMMON_P_LIST_HEAD pNew, IN const COMMON_P_LIST_HEAD pHead)
{
    __list_add(pNew, pHead, pHead->next);
}

/***********************************************************
*  Function: common_list_add_tail 反向插入一个新的节点
*  Input: pNew->新节点
*         pHead->插入点
*  Output: none
*  Return: none
***********************************************************/
void common_list_add_tail(IN const COMMON_P_LIST_HEAD pNew, IN const COMMON_P_LIST_HEAD pHead)
{
    __list_add(pNew, pHead->prev, pHead);
}

/***********************************************************
*  Function: common_list_splice 接合两条链表
*  Input: pList->被接合链
*         pHead->接合链
*  Output: none
*  Return: none
***********************************************************/
void common_list_splice(IN const COMMON_P_LIST_HEAD pList, IN const COMMON_P_LIST_HEAD pHead)
{
    COMMON_P_LIST_HEAD pFirst = pList->next;

    if (pFirst != pList) // 该链表不为空
    {
        COMMON_P_LIST_HEAD pLast = pList->prev;
        COMMON_P_LIST_HEAD pAt = pHead->next; // list接合处的节点

        pFirst->prev = pHead;
        pHead->next = pFirst;
        pLast->next = pAt;
        pAt->prev = pLast;
    }
}

/***********************************************************
*  Function: common_list_del 链表中删除节点
*  Input: pEntry->待删除节点
*  Output: none
*  Return: none
***********************************************************/
void common_list_del(IN const COMMON_P_LIST_HEAD pEntry)
{
    __list_del(pEntry->prev, pEntry->next);
}

/***********************************************************
*  Function: list_del 链表中删除某节点并初始化该节点
*  Input: pEntry->待删除节点
*  Output: none
*  Return: none
***********************************************************/
void common_list_del_init(IN const COMMON_P_LIST_HEAD pEntry)
{
    __list_del(pEntry->prev, pEntry->next);
    COMMON_INIT_LIST_HEAD(pEntry);
}

