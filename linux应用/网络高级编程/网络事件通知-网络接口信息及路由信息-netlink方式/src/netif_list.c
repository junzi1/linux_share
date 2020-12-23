#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tools.h"
#include "netif_list.h"

NET_MANAGE_HEAD netif_head = {0};

NETIF_NODE* netif_node_add(char *ifname)
{
    if(!ifname)
        return NULL;
        
    //malloc
    NETIF_NODE *new_node = NULL;
    COMMON_NEW_LIST_NODE(NETIF_NODE,new_node);
    if(!new_node) {
        return NULL;
    }
    
    //set
    memset(&new_node->attr,0,sizeof(NETIF_ATTR));
    strncpy(new_node->attr.ifname,ifname,sizeof(new_node->attr.ifname));
    
    pthread_mutex_lock(&netif_head.mutex);
    common_list_add(&new_node->node,&netif_head.listHead);
    pthread_mutex_unlock(&netif_head.mutex);
    
    netif_head.list_sum++;
    
    return new_node;
}

int netif_node_del(char *ifname)
{
    COMMON_LIST_HEAD *tmp_node = NULL;    
    NETIF_NODE *netif_node = NULL;
    
    pthread_mutex_lock(&netif_head.mutex);
    common_list_for_each(tmp_node,&netif_head.listHead) {
        netif_node = common_list_entry(tmp_node,NETIF_NODE,node);
        if(strcmp(ifname,netif_node->attr.ifname) == 0) {
            COMMON_DeleteNodeAndFree(netif_node,node);            
            pthread_mutex_unlock(&netif_head.mutex);
            netif_head.list_sum--;
            return 0;
        }
    }    
    pthread_mutex_unlock(&netif_head.mutex);

    return -1;
}

NETIF_NODE* netif_node_find(char *ifname)
{
    COMMON_LIST_HEAD *tmp_node = NULL;    
    NETIF_NODE *netif_node = NULL;
    
    pthread_mutex_lock(&netif_head.mutex);
    common_list_for_each(tmp_node,&netif_head.listHead) {
        netif_node = common_list_entry(tmp_node,NETIF_NODE,node);
        if(strcmp(ifname,netif_node->attr.ifname) == 0) {
            pthread_mutex_unlock(&netif_head.mutex);
            return netif_node;
        }
    }    
    pthread_mutex_unlock(&netif_head.mutex);

    return NULL;
}

void netif_list_print()
{
    log_debug("net node sum %d:\n",netif_head.list_sum);
    if(netif_head.list_sum <= 0)
        return;
        
    COMMON_LIST_HEAD *tmp_node = NULL;    
    NETIF_NODE *netif_node = NULL;
    
    pthread_mutex_lock(&netif_head.mutex);
    common_list_for_each(tmp_node,&netif_head.listHead) {
        netif_node = common_list_entry(tmp_node,NETIF_NODE,node);
        log_debug("############ ifname:%s\n",netif_node->attr.ifname);        
        log_debug("status:%s\n",netif_node->attr.up?"up":"down");        
        log_debug("port link:%s\n",netif_node->attr.port_link?"up":"down");        
        log_debug("mtu:%d\n",netif_node->attr.mtu);        
        hwaddr_print("hwaddr",netif_node->attr.hwaddr);        
        ipaddr_print("ipaddr",netif_node->attr.ipaddr);
    }    
    pthread_mutex_unlock(&netif_head.mutex);
    
    log_debug("\n");
}

