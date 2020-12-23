
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/route.h>
#include <errno.h>
#include "tools.h"
#include "netif_event.h"
#include "netif_list.h"

#if 0
struct rtattr {
	unsigned short	rta_len;
	unsigned short	rta_type;
};

/* Macros to handle rtattributes */

#define RTA_ALIGNTO	4U
#define RTA_ALIGN(len) ( ((len)+RTA_ALIGNTO-1) & ~(RTA_ALIGNTO-1) )
#define RTA_OK(rta,len) ((len) >= (int)sizeof(struct rtattr) && \
			 (rta)->rta_len >= sizeof(struct rtattr) && \
			 (rta)->rta_len <= (len))
#define RTA_NEXT(rta,attrlen)	((attrlen) -= RTA_ALIGN((rta)->rta_len), \
				 (struct rtattr*)(((char*)(rta)) + RTA_ALIGN((rta)->rta_len)))
#define RTA_LENGTH(len)	(RTA_ALIGN(sizeof(struct rtattr)) + (len))
#define RTA_SPACE(len)	RTA_ALIGN(RTA_LENGTH(len))
#define RTA_DATA(rta)   ((void*)(((char*)(rta)) + RTA_LENGTH(0)))
#define RTA_PAYLOAD(rta) ((int)((rta)->rta_len) - RTA_LENGTH(0))

#endif

#if 0
struct sockaddr_nl {
	__kernel_sa_family_t	nl_family;	/* AF_NETLINK	*/
	unsigned short	nl_pad;		/* zero		*/
	__u32		nl_pid;		/* port ID	*/
       	__u32		nl_groups;	/* multicast groups mask */
};

struct nlmsghdr {
	__u32		nlmsg_len;	/* Length of message including header */
	__u16		nlmsg_type;	/* Message content */
	__u16		nlmsg_flags;	/* Additional flags */
	__u32		nlmsg_seq;	/* Sequence number */
	__u32		nlmsg_pid;	/* Sending process port ID */
};

/* Flags values */

#define NLM_F_REQUEST		0x01	/* It is request message. 	*/
#define NLM_F_MULTI		0x02	/* Multipart message, terminated by NLMSG_DONE */
#define NLM_F_ACK		0x04	/* Reply with ack, with zero or error code */
#define NLM_F_ECHO		0x08	/* Echo this request 		*/
#define NLM_F_DUMP_INTR		0x10	/* Dump was inconsistent due to sequence change */
#define NLM_F_DUMP_FILTERED	0x20	/* Dump was filtered as requested */

/* Modifiers to GET request */
#define NLM_F_ROOT	0x100	/* specify tree	root	*/
#define NLM_F_MATCH	0x200	/* return all matching	*/
#define NLM_F_ATOMIC	0x400	/* atomic GET		*/
#define NLM_F_DUMP	(NLM_F_ROOT|NLM_F_MATCH)

/* Modifiers to NEW request */
#define NLM_F_REPLACE	0x100	/* Override existing		*/
#define NLM_F_EXCL	0x200	/* Do not touch, if it exists	*/
#define NLM_F_CREATE	0x400	/* Create, if it does not exist	*/
#define NLM_F_APPEND	0x800	/* Add to end of list		*/

/* Modifiers to DELETE request */
#define NLM_F_NONREC	0x100	/* Do not delete recursively	*/

/* Flags for ACK message */
#define NLM_F_CAPPED	0x100	/* request was capped */
#define NLM_F_ACK_TLVS	0x200	/* extended ACK TVLs were included */


#define NLMSG_ALIGNTO	4U
#define NLMSG_ALIGN(len) ( ((len)+NLMSG_ALIGNTO-1) & ~(NLMSG_ALIGNTO-1) )
#define NLMSG_HDRLEN	 ((int) NLMSG_ALIGN(sizeof(struct nlmsghdr)))
#define NLMSG_LENGTH(len) ((len) + NLMSG_HDRLEN)
#define NLMSG_SPACE(len) NLMSG_ALIGN(NLMSG_LENGTH(len))
#define NLMSG_DATA(nlh)  ((void*)(((char*)nlh) + NLMSG_LENGTH(0)))
#define NLMSG_NEXT(nlh,len)	 ((len) -= NLMSG_ALIGN((nlh)->nlmsg_len), \
				  (struct nlmsghdr*)(((char*)(nlh)) + NLMSG_ALIGN((nlh)->nlmsg_len)))
#define NLMSG_OK(nlh,len) ((len) >= (int)sizeof(struct nlmsghdr) && \
			   (nlh)->nlmsg_len >= sizeof(struct nlmsghdr) && \
			   (nlh)->nlmsg_len <= (len))
#define NLMSG_PAYLOAD(nlh,len) ((nlh)->nlmsg_len - NLMSG_SPACE((len)))

#define NLMSG_NOOP		0x1	/* Nothing.		*/
#define NLMSG_ERROR		0x2	/* Error		*/
#define NLMSG_DONE		0x3	/* End of a dump	*/
#define NLMSG_OVERRUN		0x4	/* Data lost		*/

#define NLMSG_MIN_TYPE		0x10	/* < 0x10: reserved control messages */

#endif

#if 0
struct ifinfomsg {
    unsigned char	ifi_family;
    unsigned char	__ifi_pad;
    unsigned short	ifi_type;		/* ARPHRD_* */
    int		ifi_index;		/* Link index	*/
    unsigned	ifi_flags;		/* IFF_* flags	*/
    unsigned	ifi_change;		/* IFF_* change mask */
};

struct ifaddrmsg {
	__u8		ifa_family;
	__u8		ifa_prefixlen;	/* The prefix length		*/
	__u8		ifa_flags;	/* Flags			*/
	__u8		ifa_scope;	/* Address scope		*/
	__u32		ifa_index;	/* Link index			*/
};

struct rtmsg {
	unsigned char		rtm_family;
	unsigned char		rtm_dst_len;
	unsigned char		rtm_src_len;
	unsigned char		rtm_tos;

	unsigned char		rtm_table;	/* Routing table id */
	unsigned char		rtm_protocol;	/* Routing protocol; see below	*/
	unsigned char		rtm_scope;	/* See below */	
	unsigned char		rtm_type;	/* See below	*/

	unsigned		rtm_flags;
};


#endif

/*
    内核广播到应用层的netlink的消息
    nlmsghdr + data
    具体如下:
    nlmsghdr + ifaddrmsg/ifinfomsg/rtmsg + rtattr + rtattr实际消息,其中rtattr->rta_len指明了消息长度
*/


extern NET_MANAGE_HEAD netif_head;

NETINFO_MANAGE netinfo_manage = {0};
IF_ALLINFO ifall = {0};

unsigned short lfla_array[] = {IFLA_UNSPEC,IFLA_ADDRESS,IFLA_BROADCAST,IFLA_IFNAME,IFLA_MTU,IFLA_LINK,IFLA_QDISC,IFLA_STATS};
unsigned short lfa_array[] = {IFA_UNSPEC,IFA_ADDRESS,IFA_LOCAL,IFA_LABEL,IFA_BROADCAST,IFA_ANYCAST,IFA_CACHEINFO};

void parse_rtattr(struct rtattr **tb, int max, struct rtattr *attr, int len)
{
    for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
        if (attr->rta_type <= max) {
            tb[attr->rta_type] = attr;
        }
    }
}
/**************************************debug相关*********************************************/
/*
enum {
	RTN_UNSPEC,
	RTN_UNICAST,		// Gateway or direct route
	RTN_LOCAL,		// Accept locally	
	RTN_BROADCAST,		// Accept locally as broadcast,
				   //send as broadcast 
	RTN_ANYCAST,		// Accept locally as broadcast,
				   //but send as unicast 
	RTN_MULTICAST,		// Multicast route	
	RTN_BLACKHOLE,		// Drop				
	RTN_UNREACHABLE,	// Destination is unreachable   
	RTN_PROHIBIT,		// Administratively prohibited	
	RTN_THROW,		// Not in this table		
	RTN_NAT,		// Translate this address	
	RTN_XRESOLVE,		// Use external resolver	
	__RTN_MAX
};
*/
static void debug_print_route_type(short type) 
{
    switch(type) {
        case RTN_UNSPEC:
            log_debug("route type:%s","RTN_UNSPEC");
            break;
         case RTN_UNICAST:
            log_debug("route type:%s","RTN_UNICAST");
            break;
         case RTN_LOCAL:
            log_debug("route type:%s","RTN_LOCAL");
            break;
        case RTN_BROADCAST:
            log_debug("route type:%s","RTN_BROADCAST");
            break;
        case RTN_ANYCAST:
            log_debug("route type:%s","RTN_ANYCAST");
            break;
        case RTN_MULTICAST:
            log_debug("route type:%s","RTN_MULTICAST");
            break;
        case RTN_BLACKHOLE:
            log_debug("route type:%s","RTN_BLACKHOLE");
            break;
        case RTN_UNREACHABLE:
            log_debug("route type:%s","RTN_UNREACHABLE");
            break;
        case RTN_PROHIBIT:
            log_debug("route type:%s","RTN_PROHIBIT");
            break;
        case RTN_THROW:
            log_debug("route type:%s","RTN_THROW");
            break;
        case RTN_NAT:
            log_debug("route type:%s","RTN_NAT");
            break;
        case RTN_XRESOLVE:
            log_debug("route type:%s","RTN_XRESOLVE");
            break;
    }
        
}

void debug_rtmsg_handle(struct nlmsghdr *nlh)
{
    int len;
    struct rtattr *tb[RTA_MAX + 1];
    struct rtmsg *rt;
    char tmp[256];

    bzero(tb, sizeof(tb));
    rt = NLMSG_DATA(nlh);
    len = nlh->nlmsg_len - NLMSG_SPACE(sizeof(*rt));
    parse_rtattr(tb, RTA_MAX, RTM_RTA(rt), len);
    debug_print_route_type(rt->rtm_type);
    log_debug("%d %s: ",rt->rtm_type ,(nlh->nlmsg_type==RTM_NEWROUTE)?"NEWROUT":"DELROUT");
    if (tb[RTA_DST] != NULL) {
        inet_ntop(rt->rtm_family, RTA_DATA(tb[RTA_DST]), tmp, sizeof(tmp));
        log_debug("DST: %s ", tmp);
    }
    if (tb[RTA_SRC] != NULL) {
        inet_ntop(rt->rtm_family, RTA_DATA(tb[RTA_SRC]), tmp, sizeof(tmp));
        log_debug("SRC: %s ", tmp);
    }
    if (tb[RTA_GATEWAY] != NULL) {
        inet_ntop(rt->rtm_family, RTA_DATA(tb[RTA_GATEWAY]), tmp, sizeof(tmp));
        log_debug("GATEWAY: %s ", tmp);
    }
    
    
    log_debug("\n");
}

static void debug_rtattr_to_lfla(struct rtattr **rta, IFLA_ATTR *ifla)
{
    memset(ifla,0,sizeof(IFLA_ATTR));  
    int i = 0;
    unsigned char *data = NULL;
    unsigned short datalen = 0;

    for(i = 0; i < (sizeof(lfla_array)/sizeof(short)); i++)
        if(rta[lfla_array[i]]) {
            data = RTA_DATA(rta[lfla_array[i]]);
            datalen = rta[lfla_array[i]]->rta_len-sizeof(struct rtattr);     
            //print_ifla_data(data,datalen);
            if(data && datalen > 0) {
                switch(lfla_array[i]) {
                    case IFLA_UNSPEC:   
                    case IFLA_STATS:
                        break;                    
                    case IFLA_ADDRESS:
                        memcpy(ifla->l2_local_addr,data,datalen);   
                        break;
                    case IFLA_BROADCAST:                        
                        memcpy(ifla->l2_broadcast_addr,data,datalen);   
                        break;
                    case IFLA_IFNAME:                        
                        memcpy(ifla->ifname,data,datalen);   
                        break;
                    case IFLA_MTU:
                        memcpy(&ifla->mtu,data,datalen);
                        break;
                    case IFLA_LINK:                        
                        memcpy(&ifla->link_type,data,datalen);
                        break;
                    case IFLA_QDISC:
                        memcpy(ifla->qdisc,data,datalen);
                        break;
                    default:
                        break;
                }
            }
        }
}

static void debug_ifla_info_print(IF_ALLINFO *ifi)
{   
    IFLA_ATTR *ifla = &ifi->lfla;
    log_debug("############# %s  : %s",ifla->ifname,ifi->if_up?"up":"down");            
    log_debug("############# %s portlink : %s",ifla->ifname,ifi->link_up?"up":"down");        
    hwaddr_print("local l2 addr",ifla->l2_local_addr); 
    hwaddr_print("local l2 braddr",ifla->l2_broadcast_addr);
    log_debug("ifname:%s",ifla->ifname);    
    log_debug("mtu:%lu",ifla->mtu);
    log_debug("link type:%d",ifla->link_type);    
    log_debug("qdisc:%s",ifla->qdisc);
    log_debug("############# end");
}

static void debug_ifa_info_print(IF_ALLINFO *ifi)
{   
    IFA_ATTR *ifa = &ifi->lfa;
    
    log_debug("############# %s protocal addr info:%s#########",ifa->ifname,ifi->new_addr?"NewAddr":"DelAddr");            
    ipaddr_print("interface addr",ifa->protocal_interface_addr);     
    ipaddr_print("local addr",ifa->protocal_local_addr);     
    ipaddr_print("broadcast addr",ifa->protocal_broadcast_addr);     
    ipaddr_print("anycast addr",ifa->protocal_anycast_addr); 
    log_debug("############# end");
}

//IFF_LOWER_UP
void debug_ifinfomsg_handle(struct nlmsghdr *nlh)
{
    int len;
    struct rtattr *tb[IFLA_MAX + 1];
    struct ifinfomsg *ifinfo;

    bzero(tb, sizeof(tb));
    ifinfo = NLMSG_DATA(nlh);
    len = nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifinfo));
    parse_rtattr(tb, IFLA_MAX, IFLA_RTA (ifinfo), len);

    ifall.if_up = (ifinfo->ifi_flags & IFF_UP)?1:0;
    ifall.link_up = (ifinfo->ifi_flags & IFF_LOWER_UP)?1:0;
    //log_debug("%s: %s\n", (nlh->nlmsg_type==RTM_NEWLINK) ? "NEWLINK" : "DELLINK", (ifinfo->ifi_flags & IFF_UP) ? "up" : "down");
    debug_rtattr_to_lfla(tb,&ifall.lfla);
    debug_ifla_info_print(&ifall);  
}

static void debug_rtattr_to_lfa(struct rtattr **rta, IFA_ATTR *ifa)
{
    memset(ifa,0,sizeof(IFA_ATTR));
    
    int i = 0;
    unsigned char *data = NULL;
    unsigned short datalen = 0;

    for(i = 0; i < (sizeof(lfa_array)/sizeof(short)); i++)
        if(rta[lfa_array[i]]) {
            data = RTA_DATA(rta[lfa_array[i]]);
            datalen = rta[lfa_array[i]]->rta_len-sizeof(struct rtattr);     
            //print_ifla_data(data,datalen);
            if(data && datalen > 0) {
                switch(lfa_array[i]) {
                    case IFA_UNSPEC:   
                    case IFA_CACHEINFO:
                        break;                    
                    case IFA_ADDRESS:
                        memcpy(ifa->protocal_interface_addr,data,datalen);   
                        break;
                    case IFA_LOCAL:                        
                        memcpy(ifa->protocal_local_addr,data,datalen);   
                        break;
                    case IFA_LABEL:                        
                        memcpy(ifa->ifname,data,datalen);   
                        break;
                    case IFA_BROADCAST:
                        memcpy(&ifa->protocal_broadcast_addr,data,datalen);
                        break;
                    case IFA_ANYCAST:                        
                        memcpy(&ifa->protocal_anycast_addr,data,datalen);
                        break;
                    default:
                        break;
                }
            }
        }
}

void debug_ifaddrmsg_handle(struct nlmsghdr *nlh)
{
    int len;
    struct rtattr *tb[IFA_MAX + 1];
    struct ifaddrmsg *ifaddr;
    char tmp[256];

    bzero(tb, sizeof(tb));
    ifaddr = NLMSG_DATA(nlh);
    len =nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifaddr));
    parse_rtattr(tb, IFA_MAX, IFA_RTA (ifaddr), len);

    ifall.new_addr =  (nlh->nlmsg_type == RTM_NEWADDR);

    debug_rtattr_to_lfa(tb,&ifall.lfa);
    debug_ifa_info_print(&ifall);
}
/**************************************debug end*********************************************/

#define dispatch_rta(rta,type,buf)\
do{\
unsigned char *data = NULL;\
unsigned short datalen = 0;\
if(rta[type]) {\
    data = RTA_DATA(rta[type]);\
    datalen = rta[type]->rta_len-sizeof(struct rtattr);\
    if(data && datalen > 0) {\
        memcpy(buf,data,datalen);\
    }\
}\
}while(0)

typedef enum {
    IF_DOWN = 1,    
    IF_UP = 2,    
    IF_DELADDR = 3,
    IF_NEWADDR = 4,    
}NETIF_EVENT_TYPE;

static void net_event_notify(char *ifname, NETIF_EVENT_TYPE type)
{
    NETIF_NODE *node = netif_node_find(ifname);
    if(!node)
        return;
        
    if(node->attr.current_state != type) {  
        switch(type) {
            case IF_UP:                
                log_debug("%s:%s",ifname,"up");
                break;
            case IF_DOWN:                
                log_debug("%s:%s",ifname,"down");
                break;
            case IF_NEWADDR:                
                ipaddr_print("new addr",node->attr.ipaddr);
                break;
            case IF_DELADDR:                
                log_debug("%s:%s",ifname,"del addr");
                pthread_mutex_lock(&netif_head.mutex);
                memset(&node->attr.ipaddr,0,sizeof(node->attr.ipaddr));
                pthread_mutex_unlock(&netif_head.mutex);                
                ipaddr_print("now addr",node->attr.ipaddr);
                break;
        }
    }

    pthread_mutex_lock(&netif_head.mutex);
    node->attr.current_state = type;
    pthread_mutex_unlock(&netif_head.mutex);
    
}

//IFF_LOWER_UP
static void ifinfomsg_handle(unsigned short type ,struct nlmsghdr *nlh)
{
    int len;
    struct rtattr *tb[IFLA_MAX + 1];
    struct ifinfomsg *ifinfo;

    bzero(tb, sizeof(tb));
    ifinfo = NLMSG_DATA(nlh);
    len = nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifinfo));
    parse_rtattr(tb, IFLA_MAX, IFLA_RTA (ifinfo), len);

    unsigned char ifname[16] = {0};
    dispatch_rta(tb,IFLA_IFNAME,ifname);
    if(strlen(ifname) == 0)
        return;

    NETIF_NODE *node;
    int ret = 0;
    node = netif_node_find(ifname);
    if(!node) {
        node = netif_node_add(ifname);
        if(!node)
            return;        
    }

    pthread_mutex_lock(&netif_head.mutex);
    node->attr.up = (ifinfo->ifi_flags & IFF_UP)?1:0;
    node->attr.port_link = (ifinfo->ifi_flags & IFF_LOWER_UP)?1:0;
    dispatch_rta(tb,IFLA_MTU,&node->attr.mtu);    
    dispatch_rta(tb,IFLA_ADDRESS,node->attr.hwaddr);
    pthread_mutex_unlock(&netif_head.mutex);

    //notify
    switch(type) {
        case RTM_NEWLINK:
            log_debug("################%s RTM_NEWLINK:%s",ifname,node->attr.up?"up":"down");
            net_event_notify(ifname,node->attr.up?IF_UP:IF_DOWN);
            break;
        case RTM_DELLINK:
            log_debug("################%s RTM_DELLINK:%s",ifname,node->attr.up?"up":"down");
            net_event_notify(ifname,IF_DOWN);
            break;
    }
    
}

void ifaddrmsg_handle(unsigned short type,struct nlmsghdr *nlh)
{
    int len;
    struct rtattr *tb[IFA_MAX + 1];
    struct ifaddrmsg *ifaddr;

    bzero(tb, sizeof(tb));
    ifaddr = NLMSG_DATA(nlh);
    len =nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifaddr));
    parse_rtattr(tb, IFA_MAX, IFA_RTA (ifaddr), len);

    unsigned char ifname[16] = {0};
    dispatch_rta(tb,IFA_LABEL,ifname);
    if(strlen(ifname) == 0)
        return;

    NETIF_NODE* node;
    int ret = 0;
    node = netif_node_find(ifname);
    if(!node) {
        node = netif_node_add(ifname);
        if(!node)
            return;        
    }

    pthread_mutex_lock(&netif_head.mutex);
    dispatch_rta(tb,IFA_ADDRESS,node->attr.ipaddr);
    pthread_mutex_unlock(&netif_head.mutex);

    //notify
    switch(type) {
        case RTM_NEWADDR:            
            log_debug("################%s RTM_NEWADDR",ifname);            
            net_event_notify(ifname,IF_NEWADDR);
            break;
        case RTM_DELADDR:
            log_debug("################%s RTM_DELADDR",ifname);        
            net_event_notify(ifname,IF_DELADDR);
            break;
    }
}

static void netlink_event_handle(int fd)
{
    int r_size;
    socklen_t len = 0;
    char buff[2048] = {0};
    struct sockaddr_nl addr;
    struct nlmsghdr *nlh;

    while(1)
    {
        len = sizeof(addr);
        r_size = recvfrom(fd, (void *)buff, sizeof(buff), 0, (struct sockaddr *)&addr, &len);
        nlh = (struct nlmsghdr *)buff;
        for(; NLMSG_OK(nlh, r_size); nlh = NLMSG_NEXT(nlh, r_size))
        {
            switch(nlh->nlmsg_type) {
            case NLMSG_DONE:
            case NLMSG_ERROR:
                break;
            case RTM_NEWLINK:     /* */
            case RTM_DELLINK:
                //debug_ifinfomsg_handle(nlh);
                ifinfomsg_handle(nlh->nlmsg_type,nlh);
                break;
            case RTM_NEWADDR:
            case RTM_DELADDR:    /* */
                //debug_ifaddrmsg_handle(nlh);
                ifaddrmsg_handle(nlh->nlmsg_type,nlh);            
                //netif_list_print();
                break;
            case RTM_NEWROUTE:
            case RTM_DELROUTE:   /* */
                //debug_rtmsg_handle(nlh);
                break;
            default:
                break;
            }
        }
    }
}

static int epoll_add_fd(int epollfd,int fd)
{
    struct epoll_event ev;

    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
        log_error("epoll add fd error");
        return -1;
    }

    return 0;
}

static int create_netlink_socket()
{
    int ret = 0;
    int fd = -1;
    struct sockaddr_nl sa;

    /* open a netlink fd */
    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        log_error("create netlink socket failure");
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    //sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE | RTMGRP_IPV6_IFADDR | RTMGRP_IPV6_ROUTE;
    
    sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

    /* bind netlink */
    ret = bind(fd, (struct sockaddr *)&sa, sizeof(sa));
    if (ret < 0) {
        log_error("bind fd error");
        close(fd);
        return -1;
    }
    
    return fd;    
}

static void* rcv_netinfo_process(void *arg)
{    
    int i = 0;
    int fd_cnt = 0;
    int sfd;
    struct epoll_event events[EPOLL_LISTEN_MAX_CNT];

    memset(events, 0, sizeof(events));
    while(1)
    {
        /* wait epoll event */
        fd_cnt = epoll_wait(netinfo_manage.epollfd, events, EPOLL_LISTEN_MAX_CNT, -1);
        for(i = 0; i < fd_cnt; i++)
        {
            sfd = events[i].data.fd;
            if(events[i].events & EPOLLIN)
            {
                if (sfd == netinfo_manage.nlfd)
                {
                    netlink_event_handle(sfd);
                }
            }
        }
    }    
    
    return (void*)0;
}

static int netif_coldboot(int family, int type, int sock_fd){

    int ret;
    struct {
        struct nlmsghdr n;
        struct ifaddrmsg i;
    }req;

    if(sock_fd < 0){
        return -1;
    }

    memset(&req,0,sizeof(req));

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    req.n.nlmsg_type = type;
    req.i.ifa_family = AF_INET;

    struct nlmsghdr *nh = &req.n;
    struct sockaddr_nl nladdr = { .nl_family = AF_NETLINK };
    struct iovec iov = {
        .iov_base = nh,
        .iov_len = nh->nlmsg_len,
    };

    nh->nlmsg_seq = 1;
    struct msghdr msg = {
        .msg_name = &nladdr,
        .msg_namelen = sizeof(nladdr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };

    ret = sendmsg(sock_fd, &msg, 0);
    if(ret < 0){
        return -1;
    }

    return 0;
}


int netif_event_server_init()
{
    int ret = 0;    
    
    func_exec(epoll_create(EPOLL_LISTEN_MAX_CNT), netinfo_manage.epollfd);
    func_exec(create_netlink_socket(), netinfo_manage.nlfd);
    func_exec(epoll_add_fd(netinfo_manage.epollfd,netinfo_manage.nlfd), ret);    
    func_exec(netif_coldboot(AF_INET, RTM_GETADDR,netinfo_manage.nlfd), ret);
    pthread_t rcv_pth;
    func_exec(common_create_thread(&rcv_pth,rcv_netinfo_process,NULL,SCHED_OTHER,0),ret);

    func_exec(pthread_mutex_init(&netif_head.mutex,NULL),ret);
    COMMON_INIT_LIST_HEAD(&netif_head.listHead);
    return 0;    
}


int main(int argc, char **argv)
{
    int ret = 0;

    func_exec(netif_event_server_init(), ret);
    while(1) {
        sleep(10000);
    }
    return 0;
}

