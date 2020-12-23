#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "tools.h"

struct interface {
    int     index;
    int     flags;      /* IFF_UP etc. */
    long    speed;      /* Mbps; -1 is unknown */
    int     duplex;     /* DUPLEX_FULL, DUPLEX_HALF, or unknown */
    char    name[IF_NAMESIZE + 1];
};

static int get_interface_common(const int fd, struct ifreq *const ifr, struct interface *const info)
{
    struct ethtool_cmd  cmd;
    int                 result;

    /* Interface flags. */
    if (ioctl(fd, SIOCGIFFLAGS, ifr) == -1)
        info->flags = 0;
    else
        info->flags = ifr->ifr_flags;

    ifr->ifr_data = (void *)&cmd;
    cmd.cmd = ETHTOOL_GSET; /* "Get settings" */
    if (ioctl(fd, SIOCETHTOOL, ifr) == -1) {
        /* Unknown */
        info->speed = -1L;
        info->duplex = DUPLEX_UNKNOWN;
    } else {
        info->speed = ethtool_cmd_speed(&cmd);
        info->duplex = cmd.duplex;
    }

    do {
        result = close(fd);
    } while (result == -1 && errno == EINTR);
    if (result == -1)
        return errno;

    return 0;
}

#define ethname "eth2"
#define new_ethname "eth2"

static int fd = 0;
void demo_SIOCGIFNAME()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));
    
    int i;
    for(i = 1;;i++) {
        req.ifr_ifindex = i;
        if(ioctl(fd,SIOCGIFNAME,&req) == -1) {
            //perror("ioctl");
            break;
        }
        log_debug("index:%d  ifname:%s",i,req.ifr_name);
        
    }
}

void demo_SIOCGIFINDEX()
{
     struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFINDEX,&req) == -1) {
        perror("ioctl");
        return;
    }
    log_debug("eth2 index:%d",req.ifr_ifindex);
}


void demo_SIOCGIFFLAGS()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFFLAGS,&req) == -1) {
        perror("ioctl");
        return;
    }

/*
SIOCGIFFLAGS, SIOCSIFFLAGS
Get or set the active flag word of the device.  ifr_flags con‐
tains a bit mask of the following values:

                      Device flags
IFF_UP            Interface is running.
IFF_BROADCAST     Valid broadcast address set.
IFF_DEBUG         Internal debugging flag.
IFF_LOOPBACK      Interface is a loopback interface.

IFF_POINTOPOINT   Interface is a point-to-point link.
IFF_RUNNING       Resources allocated.
IFF_NOARP         No arp protocol, L2 destination address not
                set.
IFF_PROMISC       Interface is in promiscuous mode.
IFF_NOTRAILERS    Avoid use of trailers.
IFF_ALLMULTI      Receive all multicast packets.
IFF_MASTER        Master of a load balancing bundle.
IFF_SLAVE         Slave of a load balancing bundle.
IFF_MULTICAST     Supports multicast
IFF_PORTSEL       Is able to select media type via ifmap.
IFF_AUTOMEDIA     Auto media selection active.
IFF_DYNAMIC       The addresses are lost when the interface
                goes down.
IFF_LOWER_UP      Driver signals L1 up (since Linux 2.6.17)
IFF_DORMANT       Driver signals dormant (since Linux 2.6.17)
IFF_ECHO          Echo sent packets (since Linux 2.6.25)

Setting the active flag word  is  a  privileged  operation,  but  any
process may read it.
*/
#define ifflag_print(flag)\
do{\
    if(req.ifr_flags & flag){\
        log_debug("%s:1",#flag);\
    }else{\
        log_debug("%s:0",#flag);\
    }\
}while(0)

    ifflag_print(IFF_UP);
    ifflag_print(IFF_BROADCAST);
    ifflag_print(IFF_DEBUG);
    ifflag_print(IFF_LOOPBACK);
    ifflag_print(IFF_POINTOPOINT);
    ifflag_print(IFF_RUNNING);
    ifflag_print(IFF_NOARP);
    ifflag_print(IFF_PROMISC);
    ifflag_print(IFF_NOTRAILERS);
    ifflag_print(IFF_ALLMULTI);
    ifflag_print(IFF_MASTER);
    ifflag_print(IFF_SLAVE);
    ifflag_print(IFF_MULTICAST);
    ifflag_print(IFF_PORTSEL);
    ifflag_print(IFF_AUTOMEDIA);
    ifflag_print(IFF_DYNAMIC);
    //ifflag_print(IFF_LOWER_UP);
    //ifflag_print(IFF_DORMANT);
    //ifflag_print(IFF_ECHO);
    
}

//一般不支持这个
void demo_SIOCGIFPFLAGS()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFPFLAGS,&req) == -1) {
        perror("ioctl");
        return;
    }
    log_debug("%x",req.ifr_flags);

/*
Get or set extended (private) flags for the device.  ifr_flags
contains a bit mask of the following values:

                     Private flags
IFF_802_1Q_VLAN      Interface is 802.1Q VLAN device.
IFF_EBRIDGE          Interface is Ethernet bridging device.
IFF_SLAVE_INACTIVE   Interface is inactive bonding slave.
IFF_MASTER_8023AD    Interface is 802.3ad bonding master.
IFF_MASTER_ALB       Interface is balanced-alb bonding master.
IFF_BONDING          Interface is a bonding master or slave.
IFF_SLAVE_NEEDARP    Interface needs ARPs for validation.
IFF_ISATAP           Interface is RFC4214 ISATAP interface.

Setting the extended (private) interface flags is a privileged opera‐
tion.
*/
}

void demo_SIOCGIFADDR()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFADDR,&req) == -1) {
        perror("ioctl");
        return;
    }
    struct sockaddr_in *addr = (struct sockaddr_in*)&req.ifr_addr;
    struct in_addr ipaddr = addr->sin_addr;
    log_debug("eth2 ip addr:%s",inet_ntoa(ipaddr));
}

void demo_SIOCGIFDSTADDR()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFDSTADDR,&req) == -1) {
        perror("ioctl");
        return;
    }
    struct sockaddr_in *addr = (struct sockaddr_in*)&req.ifr_dstaddr;
    struct in_addr ipaddr = addr->sin_addr;
    log_debug("eth2 ip dst addr:%s",inet_ntoa(ipaddr));
}

void demo_SIOCGIFBRDADDR()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFBRDADDR,&req) == -1) {
        perror("ioctl");
        return;
    }
    struct sockaddr_in *addr = (struct sockaddr_in*)&req.ifr_broadaddr;
    struct in_addr ipaddr = addr->sin_addr;
    log_debug("eth2 ip broadcast addr:%s",inet_ntoa(ipaddr));
}

void demo_SIOCGIFNETMASK()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFNETMASK,&req) == -1) {
        perror("ioctl");
        return;
    }
    struct sockaddr_in *addr = (struct sockaddr_in*)&req.ifr_netmask;
    struct in_addr ipaddr = addr->sin_addr;
    log_debug("eth2 ip netmask addr:%s",inet_ntoa(ipaddr));
}

void demo_SIOCGIFMETRIC()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFMETRIC,&req) == -1) {
        perror("ioctl");
        return;
    }
    log_debug("eth2 ifr_metric:%d",req.ifr_metric);
}

void demo_SIOCGIFMTU()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFMTU,&req) == -1) {
        perror("ioctl");
        return;
    }
    log_debug("eth2 mtu:%d",req.ifr_mtu);
}

void demo_SIOCGIFHWADDR()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFHWADDR,&req) == -1) {
        perror("ioctl");
        return;
    }
    
    log_debug("eth2 addr type:%d",req.ifr_hwaddr.sa_family);
    hwaddr_print("eth2 l2 addr",(unsigned char)(req.ifr_hwaddr.sa_data));
}

void demo_SIOCGIFMAP()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFMAP,&req) == -1) {
        perror("ioctl");
        return;
    }

    log_debug("mem_start:%lu",req.ifr_map.mem_start);
    log_debug("mem_end:%lu",req.ifr_map.mem_end);
    log_debug("base_addr:%u",req.ifr_map.base_addr);
    log_debug("irq:%d",req.ifr_map.irq);
    log_debug("dma:%d",req.ifr_map.dma);
    log_debug("port:%d",req.ifr_map.port);
}

void demo_SIOCGIFTXQLEN()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFTXQLEN,&req) == -1) {
        perror("ioctl");
        return;
    }

    log_debug("eth2 ifr_qlen:%luMbps",req.ifr_qlen);
}

void demo_SIOCGIFCONF()
{
/*
struct ifconf {
    int                 ifc_len; // size of buffer 
    union {
        char           *ifc_buf; // buffer address 
        struct ifreq   *ifc_req; // array of structures 
    };
};
*/
    struct ifconf conf;   
    struct ifreq req[10] = {0};
    
    memset(&conf,0,sizeof(struct ifconf));
    conf.ifc_buf = (char*)req;
    conf.ifc_len = sizeof(req);
    
    log_debug("ifreq size:%d  config buflen:%d",sizeof(struct ifreq),conf.ifc_len);
    if(ioctl(fd,SIOCGIFCONF,&conf) == -1) {
        perror("ioctl");
        return;
    }
    
    log_debug("return config buflen:%d",conf.ifc_len);
    int i;
    for(i = 0; i < conf.ifc_len/sizeof(struct ifreq); i++) {
        log_debug("############ifname:%s",req[i].ifr_name);        
        struct sockaddr_in *addr = (struct sockaddr_in*)&req[i].ifr_addr;
        struct in_addr ipaddr = addr->sin_addr;
        log_debug("ip addr:%s",inet_ntoa(ipaddr));
    }
}

void demo_about_get()
{
    demo_SIOCGIFNAME();
    demo_SIOCGIFINDEX();
    demo_SIOCGIFFLAGS();
    demo_SIOCGIFPFLAGS();
    //ip地址相关 操作类似
    demo_SIOCGIFADDR();
    demo_SIOCGIFDSTADDR();
    demo_SIOCGIFBRDADDR();
    demo_SIOCGIFNETMASK();
    //metric mtu
    demo_SIOCGIFMETRIC();
    demo_SIOCGIFMTU();
    //l2 addr
    demo_SIOCGIFHWADDR();
    //ifmap
    demo_SIOCGIFMAP();

    //传输速率 1000Mbps
    demo_SIOCGIFTXQLEN();
    //获取所有配置
    demo_SIOCGIFCONF();    
}


void demo_SIOCSIFFLAGS(int up)
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFFLAGS,&req) == -1) {
        perror("ioctl");
        return;
    }

    if(!up)
        req.ifr_flags &= (~IFF_UP);
    else
        req.ifr_flags |= (IFF_UP); 
        
    req.ifr_flags |= (IFF_ALLMULTI);    
    req.ifr_flags |= (IFF_PROMISC);
    if(ioctl(fd,SIOCSIFFLAGS,&req) == -1) {
        perror("ioctl");
        return;
    }

/*
SIOCGIFFLAGS, SIOCSIFFLAGS
Get or set the active flag word of the device.  ifr_flags con‐
tains a bit mask of the following values:

                      Device flags
IFF_UP            Interface is running.
IFF_BROADCAST     Valid broadcast address set.
IFF_DEBUG         Internal debugging flag.
IFF_LOOPBACK      Interface is a loopback interface.

IFF_POINTOPOINT   Interface is a point-to-point link.
IFF_RUNNING       Resources allocated.
IFF_NOARP         No arp protocol, L2 destination address not
                set.
IFF_PROMISC       Interface is in promiscuous mode.
IFF_NOTRAILERS    Avoid use of trailers.
IFF_ALLMULTI      Receive all multicast packets.
IFF_MASTER        Master of a load balancing bundle.
IFF_SLAVE         Slave of a load balancing bundle.
IFF_MULTICAST     Supports multicast
IFF_PORTSEL       Is able to select media type via ifmap.
IFF_AUTOMEDIA     Auto media selection active.
IFF_DYNAMIC       The addresses are lost when the interface
                goes down.
IFF_LOWER_UP      Driver signals L1 up (since Linux 2.6.17)
IFF_DORMANT       Driver signals dormant (since Linux 2.6.17)
IFF_ECHO          Echo sent packets (since Linux 2.6.25)

Setting the active flag word  is  a  privileged  operation,  but  any
process may read it.
*/    
}

void demo_SIOCSIFADDR()
{
    struct ifreq req;    
    struct sockaddr_in *addr = (struct sockaddr_in*)&req.ifr_addr;

    memset(&req,0,sizeof(struct ifreq));    
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr("192.168.13.253");
    strcpy(req.ifr_name,ethname);
    
    if(ioctl(fd,SIOCSIFADDR,&req) == -1) {
        perror("ioctl");
        return;
    }
}

void demo_SIOCSIFDSTADDR()
{
    struct ifreq req;    
    struct sockaddr_in *addr = (struct sockaddr_in*)&req.ifr_dstaddr;

    memset(&req,0,sizeof(struct ifreq));    
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr("192.168.13.111");
    strcpy(req.ifr_name,ethname);
    
    if(ioctl(fd,SIOCSIFDSTADDR,&req) == -1) {
        perror("ioctl");
        return;
    }
}

void demo_SIOCSIFBRDADDR()
{
    struct ifreq req;    
    struct sockaddr_in *addr = (struct sockaddr_in*)&req.ifr_broadaddr;

    memset(&req,0,sizeof(struct ifreq));    
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr("192.168.13.240");
    strcpy(req.ifr_name,ethname);
    
    if(ioctl(fd,SIOCSIFBRDADDR,&req) == -1) {
        perror("ioctl");
        return;
    }
}

void demo_SIOCSIFNETMASK()
{
    struct ifreq req;    
    struct sockaddr_in *addr = (struct sockaddr_in*)&req.ifr_netmask;

    memset(&req,0,sizeof(struct ifreq));    
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr("255.255.255.240");
    strcpy(req.ifr_name,ethname);
    
    if(ioctl(fd,SIOCSIFNETMASK,&req) == -1) {
        perror("ioctl");
        return;
    }
}


void demo_SIOCSIFMETRIC()
{
    //不支持
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    req.ifr_metric = 5;

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCSIFMETRIC,&req) == -1) {
        perror("ioctl");
        return;
    }
}


void demo_SIOCSIFMTU()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));
    req.ifr_mtu =500;

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCSIFMTU,&req) == -1) {
        perror("ioctl");
        return;
    }
}

void demo_SIOCSIFTXQLEN()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    req.ifr_qlen = 100;//100Mbps

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCSIFTXQLEN,&req) == -1) {
        perror("ioctl");
        return;
    }
}

void demo_SIOCSIFHWADDR()
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ethname);
    if(ioctl(fd,SIOCGIFHWADDR,&req) == -1) {
        perror("ioctl");
        return;
    }

    req.ifr_hwaddr.sa_data[5] = 0x88;    
    req.ifr_hwaddr.sa_data[4] = 0x87;

    if(ioctl(fd,SIOCSIFHWADDR,&req) == -1) {
        perror("ioctl");
        return;
    }
}

void demo_SIOCSIFNAME()
{
    struct ifreq req;    

    strcpy(req.ifr_name,ethname);
    strcpy(req.ifr_newname,new_ethname);
    
    if(ioctl(fd,SIOCSIFNAME,&req) == -1) {
        perror("ioctl");
        return;
    }
}

void demo_about_set()
{
    demo_SIOCSIFFLAGS(1);   
    
    //ip地址相关 操作类似
    demo_SIOCSIFADDR();    
    demo_SIOCSIFDSTADDR();    
    demo_SIOCSIFBRDADDR();    
    demo_SIOCSIFNETMASK();

    //mtu
    //demo_SIOCSIFMETRIC();
    demo_SIOCSIFMTU();
    
    //传输速率 100Mbps
    demo_SIOCSIFTXQLEN();
    //l2 addr
    demo_SIOCSIFHWADDR();
    //set new name
 #if 0
    demo_SIOCGIFNETMASK();

#endif
}

/*
struct ifreq {
#define IFHWADDRLEN	6
	union
	{
		char	ifrn_name[IFNAMSIZ];		
	} ifr_ifrn;
	
	union {
		struct	sockaddr ifru_addr;
		struct	sockaddr ifru_dstaddr;
		struct	sockaddr ifru_broadaddr;
		struct	sockaddr ifru_netmask;
		struct  sockaddr ifru_hwaddr;
		short	ifru_flags;
		int	ifru_ivalue;
		int	ifru_mtu;
		struct  ifmap ifru_map;
		char	ifru_slave[IFNAMSIZ];
		char	ifru_newname[IFNAMSIZ];
		void __user *	ifru_data;
		struct	if_settings ifru_settings;
	} ifr_ifru;
};
#endif //__UAPI_DEF_IF_IFREQ 

sockaddr_in


#define ifr_name	ifr_ifrn.ifrn_name	// interface name
#define ifr_hwaddr	ifr_ifru.ifru_hwaddr	// MAC addres
#define	ifr_addr	ifr_ifru.ifru_addr	// address
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr	// other end of p-p lnk	
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	// broadcast address	
#define	ifr_netmask	ifr_ifru.ifru_netmask	// interface net mask
#define	ifr_flags	ifr_ifru.ifru_flags	// flags		
#define	ifr_metric	ifr_ifru.ifru_ivalue	// metric		
#define	ifr_mtu		ifr_ifru.ifru_mtu	// mtu			
#define ifr_map		ifr_ifru.ifru_map	// device map		
#define ifr_slave	ifr_ifru.ifru_slave	// slave device		
#define	ifr_data	ifr_ifru.ifru_data	// for use by interface	
#define ifr_ifindex	ifr_ifru.ifru_ivalue	// interface index
#define ifr_bandwidth	ifr_ifru.ifru_ivalue    // link bandwidth	
#define ifr_qlen	ifr_ifru.ifru_ivalue	// Queue length 	
#define ifr_newname	ifr_ifru.ifru_newname	// New name		
#define ifr_settings	ifr_ifru.ifru_settings	// Device/proto settings

struct ifmap {
    unsigned long   mem_start;
    unsigned long   mem_end;
    unsigned short  base_addr;
    unsigned char   irq;
    unsigned char   dma;
    unsigned char   port;
};

*/

//参考 https://man7.org/linux/man-pages/man7/netdevice.7.html
//未实现 SIOCSIFHWBROADCAST

int main()
{
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    demo_about_set();
    demo_about_get();

    //设置新名字 首先应该down
    //demo_SIOCSIFFLAGS(0);
    //demo_SIOCSIFNAME();    
   
}


