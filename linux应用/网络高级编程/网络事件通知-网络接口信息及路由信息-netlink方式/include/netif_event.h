#ifndef _NETIF_EVENT_H_
#define _NETIF_EVENT_H_


#ifdef __cplusplus
extern "C"
{
#endif

#define EPOLL_LISTEN_MAX_CNT    256


typedef struct  {
    int epollfd;
    int nlfd;
}NETINFO_MANAGE;

typedef struct{
    unsigned char l2_local_addr[6];    
    unsigned char l2_broadcast_addr[6];    
    unsigned char ifname[16];
    unsigned int mtu;
    int link_type;
    unsigned char qdisc[36+1];
}IFLA_ATTR;

typedef struct{
    unsigned char ifname[6];
    unsigned char protocal_interface_addr[4];    
    unsigned char protocal_local_addr[4];    
    unsigned char protocal_broadcast_addr[4];    
    unsigned char protocal_anycast_addr[4];
}IFA_ATTR;

typedef struct{
    int if_up;    
    int link_up;
    int new_addr;
    IFLA_ATTR lfla;
    IFA_ATTR lfa;
}IF_ALLINFO;




#ifdef __cplusplus
}
#endif

#endif

