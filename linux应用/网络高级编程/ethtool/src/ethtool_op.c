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

/*
struct ethtool_cmd {
	__u32	cmd;
	__u32	supported;
	__u32	advertising;
	__u16	speed;
	__u8	duplex;
	__u8	port;
	__u8	phy_address;
	__u8	transceiver;
	__u8	autoneg;
	__u8	mdio_support;
	__u32	maxtxpkt;
	__u32	maxrxpkt;
	__u16	speed_hi;
	__u8	eth_tp_mdix;
	__u8	eth_tp_mdix_ctrl;
	__u32	lp_advertising;
	__u32	reserved[2];
};

#define	ifr_data	ifr_ifru.ifru_data	// for use by interface	

static inline __u32 ethtool_cmd_speed(const struct ethtool_cmd *ep)
{
	return (ep->speed_hi << 16) | ep->speed;
}


// Duplex, half or full.
#define DUPLEX_HALF		0x00
#define DUPLEX_FULL		0x01
#define DUPLEX_UNKNOWN		0xff


*/
int fd = -1;
void demo_SIOCETHTOOL()
{
    struct ethtool_cmd ecmd;
    struct ifreq ifr;
         
    memset(&ecmd, 0, sizeof(ecmd));
    memset(&ifr, 0, sizeof(ifr));

    //ETHTOOL_GSET
    ecmd.cmd = ETHTOOL_GSET;
    //ecmd.cmd = ETHTOOL_GLINKSETTINGS;
    ifr.ifr_data = (char*)&ecmd;
    
    strcpy(ifr.ifr_name,"eth0");
    if (ioctl(fd, SIOCETHTOOL, &ifr) == -1) {
        perror("ETHTOOL_GSET");
        return;
    }

    log_debug("speed:%dMbps",ethtool_cmd_speed(&ecmd));    
    log_debug("duplex:%s",ecmd.duplex?"DuplexFull":"DuplexHalf");    
    //log_debug("port:%d",ecmd.port);    
    //log_debug("maxrxpkt:%d",ecmd.maxrxpkt);
    //log_debug("maxtxpkt:%d",ecmd.maxtxpkt);

    //sys/class/net/eth0/carrier
    //ETHTOOL_GLINK    
    struct ethtool_value evalue;    
    
    ifr.ifr_data = (char*)&evalue;    
    evalue.cmd = ETHTOOL_GLINK;
    if (ioctl(fd, SIOCETHTOOL, &ifr) == -1) {
        perror("ETHTOOL_GLINK");
        return;
    }
    log_debug("link status:%s",evalue.data?"up":"down");    
    
}

int main()
{
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (fd == -1) {
        perror("socket");
        return -1;
    }

    demo_SIOCETHTOOL();

}


