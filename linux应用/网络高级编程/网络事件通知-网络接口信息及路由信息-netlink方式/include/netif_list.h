#ifndef _NETIF_LIST_H_
#define _NETIF_LIST_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include <pthread.h>
#include <stdbool.h>
#include "list_op.h"

typedef struct  {
    COMMON_LIST_HEAD listHead;
    unsigned int list_sum; //节点总和        
    pthread_mutex_t mutex;
}NET_MANAGE_HEAD;

typedef struct  {
    char ifname[16];
    bool up;
    bool port_link;    
    unsigned int mtu;
    int current_state;
    unsigned char hwaddr[6];
    unsigned char ipaddr[4];
}NETIF_ATTR;

typedef struct  {
    COMMON_LIST_HEAD node;
    NETIF_ATTR attr;
}NETIF_NODE;

NETIF_NODE* netif_node_add(char *ifname);
int netif_node_del(char *ifname);
NETIF_NODE* netif_node_find(char *ifname);
void netif_list_print();

#ifdef __cplusplus
}
#endif

#endif

