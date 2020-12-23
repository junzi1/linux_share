#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "tools.h"

int fd = -1;
int udp_fd = -1;
int ip_fd = -1;

void demo_SO_ACCEPTCONN()
{
    bool onoff = -1;
    unsigned int len;
    if (getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, (void *)&onoff, &len)==-1){
        perror("SO_ACCEPTCONN");
        close(fd);
        return;
    }    
    log_debug("fd is listening? :%d",onoff);
}

void demo_SO_BINDTODEVICE()
{
    struct ifreq req;

    memset(&req,0,sizeof(struct ifreq));
    strcpy(req.ifr_name,"eth0");

    // Note that this works only for some socket types, particularly AF_INET sockets.  
    //It is not supported  for  packet  sockets 
    bool onoff;
    if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&req, sizeof(struct ifreq))==-1){
        perror("SO_BINDTODEVICE");
        close(fd);
        return;
    }    

    //内核3.8之后支持可读
    unsigned int len;
    memset(&req,0,sizeof(struct ifreq));
    if (getsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&req,&len) ==-1){
        perror("SO_BINDTODEVICE");
        close(fd);
        return;
    } 
    log_debug("fd bind to:%s",req.ifr_name);
}

void demo_SO_DOMAIN()
{
    //执行成功，但是没返回值 或者执行失败
    //since Linux 2.6.32
    int domain = -1;
    unsigned int len;
   
    if (getsockopt(fd, SOL_SOCKET, SO_DOMAIN , (void *)&domain, &len)==-1){
        perror("SO_DOMAIN");
        close(fd);
        return;
    }   
    //AF_INET 2
    log_debug("tcp domain :%d",domain);
}

void demo_SO_ERROR()
{
    int error = -1;
    unsigned int len;

   //Get and clear the pending socket error.  This socket option is read-only.  Expects an integer.
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR , (void *)&error, &len)==-1){
        perror("SO_ERROR");
        close(fd);
        return;
    }   
    
    log_debug("tcp error :%d",error);
}

void demo_SO_DONTROUTE()
{
    //Don't  send via a gateway, send only to directly connected hosts.  The same effect can be achieved by setting the MSG_DONTROUTE flag on a socket
    //send(2) operation.  Expects an integer boolean flag.
    int onoff = 1;
    unsigned int len;
    if (setsockopt(fd, SOL_SOCKET, SO_DONTROUTE, (void *)&onoff, sizeof(int))==-1){
        perror("SO_DONTROUTE");
        close(fd);
        return;
    }    

    //#define	MSG_DONTROUTE	0x4		/* send without using routing tables */
    
    log_debug("tcp dont route? :%d",onoff);
}

void demo_SO_KEEPALIVE()
{
    int on= 1;
    unsigned int len;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(int))==-1){
        perror("SO_KEEPALIVE");
        close(fd);
        return;
    }    
    
    log_debug("tcp keep alive? :%d",on);
}

void demo_SO_LINGER()
{
/*
struct linger {
    int l_onoff;    // linger active 
    int l_linger;   // how many seconds to linger for 
};

*/
    struct linger config = {1,5};
    unsigned int len;
    if (setsockopt(fd, SOL_SOCKET, SO_LINGER, (void *)&config, sizeof(struct linger))==-1){
        perror("SO_LINGER");
        close(fd);
        return;
    }    
    
    log_debug("tcp set so linger? :%d, time:%d",config.l_onoff,config.l_linger);
       
}

void demo_SO_MARK()
{
    //Set the mark for each packet sent through this socket (similar to the netfilter MARK target but socket-based).  Changing the mark  can  be  used
    //for mark-based routing without netfilter or for packet filtering.  Setting this option requires the CAP_NET_ADMIN capability.
    //since Linux 2.6.25

    //为报文打个mark 只在本机有用，然后iptables命令设置策略
    //在本机给包设置关联的mark值后，可通过该值对包后续的传输进行控制（排队，流量控制等）
    //iptables -t mangle -A INPUT -m mark --mark 100
    
    int mark = 100;
    unsigned int len;
    if (setsockopt(fd, SOL_SOCKET, SO_MARK, (void *)&mark, sizeof(int))==-1){
        perror("SO_MARK");
        close(fd);
        return;
    }    

    log_debug("mark socket? :%d",mark);
}

void demo_SO_OOBINLINE()
{
    //tcp urg模式相关，使能后从普通输入队列中取紧急数据，默认不使能
    //如果不使能，通过信号SIGURG触发信号回调，然后revc MSG_OOB标志接受
    int on= 1;
    unsigned int len;
    if (setsockopt(fd, SOL_SOCKET, SO_OOBINLINE, (void *)&on, sizeof(int))==-1){
        perror("SO_OOBINLINE");
        close(fd);
        return;
    }    
    
    log_debug("tcp oobinline? :%d",on);
}

void demo_SO_PEEK_OFF()
{
    //设置MSG_PEEK的窥探偏移 since Linux 3.4
    int off = 4;
    unsigned int len;
    if (setsockopt(fd, SOL_SOCKET, SO_PEEK_OFF, (void *)&off, sizeof(int))==-1){
        perror("SO_PEEK_OFF");
        close(fd);
        return;
    }    
    
    log_debug("tcp peek off? :%d",off);
/*
The following example serves to illustrate the use of SO_PEEK_OFF.  
Suppose a stream socket has the following queued input data:
aabbccddeeff

The following sequence of recv(2) calls would have the effect noted in the comments:

int ov = 4;                  // Set peek offset to 4
setsockopt(fd, SOL_SOCKET, SO_PEEK_OFF, &ov, sizeof(ov));

recv(fd, buf, 2, MSG_PEEK);  // Peeks "cc"; offset set to 6
recv(fd, buf, 2, MSG_PEEK);  // Peeks "dd"; offset set to 8
recv(fd, buf, 2, 0);         // Reads "aa"; offset set to 6
recv(fd, buf, 2, MSG_PEEK);  // Peeks "ee"; offset set to 8
*/    
}

void demo_SO_PRIORITY()
{
    //0-6 会影响ip tos     设置没起作用要检查tos
    int priority = 6;
    unsigned int len;
    if (setsockopt(fd, SOL_SOCKET, SO_PRIORITY, (void *)&priority, sizeof(int))==-1){
        perror("SO_PRIORITY");
        close(fd);
        return;
    }    

    int tos = -1;
    int optlen = sizeof(tos);
    if (getsockopt(fd, IPPROTO_IP, IP_TOS, &tos, &optlen) < 0){
        printf("Oh no\n");
    }
    
    log_debug("tcp ip tos? :%d",tos); 
}

void demo_SO_PROTOCOL()
{
    //#define	IPPROTO_TCP		6		/* tcp */
    int val = -1;
    unsigned int len;
    if (getsockopt(fd, SOL_SOCKET, SO_PROTOCOL, (void *)&val,&len)==-1){
        perror("SO_PROTOCOL");
        close(fd);
        return;
    }    
    
    log_debug("tcp protocal? :%d",val); 
}

void demo_SO_RCVBUF()
{
    // 128*2 < buf size < /proc/sys/net/core/rmem_max
    //256是内核乘以2之后的结果 所以设置应该最小为128
    //内核会将设置的这个值乘以2
    int val = -1;
    unsigned int len;
    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *)&val,&len)==-1){
        perror("get SO_RCVBUF");
        close(fd);
        return;
    }    
    
    log_debug("tcp rcvbuf default:%d",val); 

    val = 5000;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *)&val,sizeof(int))==-1){
        perror("set SO_RCVBUF");
        close(fd);
        return;
    }    

    val = -1;
    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *)&val,&len)==-1){
        perror("get SO_RCVBUF");
        close(fd);
        return;
    }    
    //5000*2 =10000;
    log_debug("tcp rcvbuf set:%d",val); 
}

void demo_SO_RCVBUFFORCE()
{
    // 一般 128*2 < buf size < /proc/sys/net/core/rmem_max  since Linux 2.6.14
    //该套接字无视/proc/sys/net/core/rmem_max
    
    int val = -1;
    unsigned int len;

    val = 600000;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUFFORCE, (void *)&val,sizeof(int))==-1){
        perror("set SO_RCVBUF");
        close(fd);
        return;
    }    

    val = -1;
    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (void *)&val,&len)==-1){
        perror("get SO_RCVBUF");
        close(fd);
        return;
    }    
    //5000*2 =10000;
    log_debug("tcp rcvbuf force set:%d",val); 
}

void demo_SO_SNDBUF()
{
    // 1024*2 < buf size < /proc/sys/net/core/wmem_max
    //2048是内核乘以2之后的结果 所以设置应该最小为1024      根据不同内核可能会不一样
    //内核会将设置的这个值乘以2
    //default /proc/sys/net/core/wmem_default
    //tcp 默认为 8192,8192*2=16384 
    int val = -1;
    unsigned int len;
    if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&val,&len)==-1){
        perror("get SO_SNDBUF");
        close(fd);
        return;
    }    
    
    log_debug("tcp sendbuf default:%d",val); 

    val = 5000;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&val,sizeof(int))==-1){
        perror("set SO_SNDBUF");
        close(fd);
        return;
    }    

    val = -1;
    if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&val,&len)==-1){
        perror("get SO_SNDBUF");
        close(fd);
        return;
    }    
    //5000*2 =10000;
    log_debug("tcp sendbuf set:%d",val); 
}

void demo_SO_SNDBUFFORCE()
{
    // 一般 1024*2 < buf size < /proc/sys/net/core/wmem_max  since Linux 2.6.14
    //最小值根据不同内核可能会不一样
    //该套接字无视/proc/sys/net/core/wmem_max
    
    int val = -1;
    unsigned int len;

    val = 600000;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUFFORCE, (void *)&val,sizeof(int))==-1){
        perror("set SO_SNDBUFFORCE");
        close(fd);
        return;
    }    

    val = -1;
    if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (void *)&val,&len)==-1){
        perror("get SO_SNDBUF");
        close(fd);
        return;
    }    
    //5000*2 =10000;
    log_debug("tcp sendbuf force set:%d",val); 
}

void demo_SO_RCVLOWAT()
{
    //default is 1, 从Linux 2.4.之后才可以被设置
    //比如设置为n，代表接收缓冲区至少应该有n个bytes。才通知用户层
    //select和poll不受此值影响,有数据会立刻返回
    int val = 10;
    unsigned int len;

    if (setsockopt(fd, SOL_SOCKET, SO_RCVLOWAT, (void *)&val,sizeof(int))==-1){
        perror("set SO_RCVLOWAT");
        close(fd);
        return;
    }    

    val = -1;
    if (getsockopt(fd, SOL_SOCKET, SO_RCVLOWAT, (void *)&val,&len)==-1){
        perror("get SO_RCVLOWAT");
        close(fd);
        return;
    }    
    log_debug("tcp SO_RCVLOWAT set:%d",val); 
}

void demo_SO_SNDLOWAT()
{
    //default is 1,不允许修改
    //SO_SNDLOWAT is not changeable on Linux (setsockopt(2) fails with the  error  ENOPROTOOPT)

    int val = 1;
    unsigned int len;

    if (setsockopt(fd, SOL_SOCKET, SO_SNDLOWAT, (void *)&val,sizeof(int))==-1){
        perror("set SO_SNDLOWAT");
        return;
    }   

    
    log_debug("tcp sendbuf force set:%d",val); 
}

void demo_SO_RCVTIMEO()
{
    //设置为0永远不会超时
    //如果在该规定时间内没有完成操作，则出错
    /*
Specify  the  receiving  or sending timeouts until reporting an error.  The argument is a struct timeval.  If an input or output function blocks
for this period of time, and data has been sent or received, the return value of that function will be the amount of  data  transferred;  if  no
data has been transferred and the timeout has been reached then -1 is returned with errno set to EAGAIN or EWOULDBLOCK, or EINPROGRESS (for con‐
nect(2)) just as if the socket was specified to be nonblocking.  If the timeout is set to zero (the default) then the operation will never time‐
out.   Timeouts  only  have  effect  for system calls that perform socket I/O (e.g., read(2), recvmsg(2), send(2), sendmsg(2)); timeouts have no
effect for select(2), poll(2), epoll_wait(2), and so on.
    */

    struct timeval tm;
    tm.tv_sec = 5;
    tm.tv_usec = 0;
    
    unsigned int len;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&tm,sizeof(struct timeval))==-1){
        perror("set SO_RCVTIMEO");        
        close(fd);
        return;
    }  
    
    log_debug("tcp rcv timeout set:%d",tm.tv_sec); 
}

void demo_SO_SNDTIMEO()
{
    //设置为0永远不会超时
    //如果在该规定时间内没有完成操作，则出错
    struct timeval tm;
    tm.tv_sec = 5;
    tm.tv_usec = 0;
    
    unsigned int len;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (void *)&tm,sizeof(struct timeval))==-1){
        perror("set SO_SNDTIMEO");        
        close(fd);
        return;
    }  
    
    log_debug("tcp rend timeout set:%d",tm.tv_sec); 
}

void demo_SO_TIMESTAMP()
{
    //使能时间戳功能
    int on= 1;
    unsigned int len;
    if (setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, (void *)&on, sizeof(int))==-1){
        perror("SO_TIMESTAMP");
        close(fd);
        return;
    }    
    
    log_debug("tcp enable SO_TIMESTAMP? :%d",on);
    //将时间戳附加到控制信息 cmsg             
    //https://man7.org/linux/man-pages/man3/cmsg.3.html
}

void demo_SO_TYPE()
{
    //获取不到 执行失败 暂时不用管
    //Gets the socket type as an integer (e.g., SOCK_STREAM).  This socket option is read-only.
    int type= -1;
    unsigned int len;
    if (getsockopt(fd, SOL_SOCKET, SO_TYPE, (void *)&type, &len)==-1){
        perror("SO_TYPE");
        close(fd);
        return;
    }    
    
    log_debug("tcp SO_TYPE :%d",type);
}

//IPPROTO_TCP 层的操作
void demo_TCP_CORK()
{
    //tcp塞子 since linux 2.2，配合nodelay使用，nodelay设置为0
    /*
如果在某个TCP socket上开启了这个选项，那就相当于在这个socket的出口堵上了塞子，往这个socket写入的数据都会聚集起来。
虽然堵上了塞子，但是segment总得发送，不然数据会塞满整个TCP发送缓冲区的，那么什么时候塞子会打开呢？
下面几种情况都会导致这个塞子打开，这样TCP就能继续发送segment出来了。

程序取消设置TCP_CORK这个选项。
socket聚集的数据大于一个MSS的大小。
自从堵上塞子写入第一个字节开始，已经经过200ms。
socket被关闭了。
　　
一旦满足上面的任何一个条件，TCP就会将数据发送出去。对于Server来说，发送HTTP响应既要发送尽量少的segment，同时又要保证低延迟，那么需要在写完数据后显式取消设置TCP_CORK选项，让数据立即发送出去：
    */
    int on = 1;
    unsigned int len;
    if (setsockopt(fd, IPPROTO_TCP, TCP_CORK, (void *)&on,sizeof(int))==-1){
        perror("TCP_CORK");
        close(fd);
        return;
    }    
    
    log_debug("tcp TCP_CORK? :%d",on); 
}

void demo_TCP_DEFER_ACCEPT()
{
    //since Linux 2.4
    //顾名思义，推迟accept,如果在三次握手成功后（实际上客户端的ack会被丢掉），指定时间内没收到数据，就
    //丢弃这个连接，不会唤醒accpet
    int time = 10;
    unsigned int len;
    if (setsockopt(fd, IPPROTO_TCP, TCP_DEFER_ACCEPT, (void *)&time,sizeof(int))==-1){
        perror("TCP_DEFER_ACCEPT");
        close(fd);
        return;
    }    
    
    log_debug("tcp TCP_DEFER_ACCEPT:%d",time); 
}

void demo_TCP_INFO()
{    
    //since Linux 2.4
    /*
    
    struct tcp_info {
        __u8    tcpi_state;
        __u8    tcpi_ca_state;
        __u8    tcpi_retransmits;
        __u8    tcpi_probes;
        __u8    tcpi_backoff;
        __u8    tcpi_options;
        __u8    tcpi_snd_wscale : 4, tcpi_rcv_wscale : 4;
        __u8    tcpi_delivery_rate_app_limited:1, tcpi_fastopen_client_fail:2;
    
        __u32   tcpi_rto;
        __u32   tcpi_ato;
        __u32   tcpi_snd_mss;
        __u32   tcpi_rcv_mss;
    
        __u32   tcpi_unacked;
        __u32   tcpi_sacked;
        __u32   tcpi_lost;
        __u32   tcpi_retrans;
        __u32   tcpi_fackets;
    
        // Times. 
        __u32   tcpi_last_data_sent;
        __u32   tcpi_last_ack_sent;     // Not remembered, sorry. 
        __u32   tcpi_last_data_recv;
        __u32   tcpi_last_ack_recv;
    
        // Metrics. 
        __u32   tcpi_pmtu;
        __u32   tcpi_rcv_ssthresh;
        __u32   tcpi_rtt;
        __u32   tcpi_rttvar;
        __u32   tcpi_snd_ssthresh;
        __u32   tcpi_snd_cwnd;
        __u32   tcpi_advmss;
        __u32   tcpi_reordering;
    
        __u32   tcpi_rcv_rtt;
        __u32   tcpi_rcv_space;
    
        __u32   tcpi_total_retrans;
    
        __u64   tcpi_pacing_rate;
        __u64   tcpi_max_pacing_rate;
        __u64   tcpi_bytes_acked;    // RFC4898 tcpEStatsAppHCThruOctetsAcked 
        __u64   tcpi_bytes_received; // RFC4898 tcpEStatsAppHCThruOctetsReceived 
        __u32   tcpi_segs_out;       // RFC4898 tcpEStatsPerfSegsOut 
        __u32   tcpi_segs_in;        // RFC4898 tcpEStatsPerfSegsIn 
    
        __u32   tcpi_notsent_bytes;
        __u32   tcpi_min_rtt;
        __u32   tcpi_data_segs_in;  // RFC4898 tcpEStatsDataSegsIn 
        __u32   tcpi_data_segs_out; // RFC4898 tcpEStatsDataSegsOut 
    
        __u64   tcpi_delivery_rate;
    
        __u64   tcpi_busy_time;      // Time (usec) busy sending data 
        __u64   tcpi_rwnd_limited;   // Time (usec) limited by receive window 
        __u64   tcpi_sndbuf_limited; // Time (usec) limited by send buffer 
    
        __u32   tcpi_delivered;
        __u32   tcpi_delivered_ce;
    
        __u64   tcpi_bytes_sent;     // RFC4898 tcpEStatsPerfHCDataOctetsOut 
        __u64   tcpi_bytes_retrans;  // RFC4898 tcpEStatsPerfOctetsRetrans 
        __u32   tcpi_dsack_dups;     // RFC4898 tcpEStatsStackDSACKDups 
        __u32   tcpi_reord_seen;     // reordering events seen 
    
        __u32   tcpi_rcv_ooopack;    // Out-of-order packets received 
    
        __u32   tcpi_snd_wnd;        // peer's advertised receive window after
                          * scaling (bytes)
                          
    };
    */
    struct tcp_info info; 
    unsigned int len;
    if (getsockopt(fd, IPPROTO_TCP, TCP_INFO , (void *)&info,&len)==-1){
        perror("TCP_INFO");
        close(fd);
        return;
    }    
    
    log_debug("tcp TCP_INFO: tcpi_snd_ssthresh %lu",info.tcpi_snd_ssthresh);     
    log_debug("tcp TCP_INFO: tcpi_snd_cwnd %lu",info.tcpi_snd_cwnd);     
    log_debug("tcp TCP_INFO: tcpi_rcv_ssthresh %lu",info.tcpi_rcv_ssthresh);     
    log_debug("tcp TCP_INFO: tcpi_rtt %lu",info.tcpi_rtt);     
    log_debug("tcp TCP_INFO: tcpi_rttvar %lu",info.tcpi_rttvar);
}

void demo_TCP_KEEPCNT()
{
    //since Linux 2.4
    //当tcp发现有 idle s没有收到对端时间后，开始以间隔intvl s的频率发送空心跳报文
    //如果发送连续probe次未收到响应，则关闭连接

    /*
    默认值：
     /proc/sys/net/ipv4/tcp_keepalive_time 7200
     /proc/sys/net/ipv4/tcp_keepalive_probes   9
     /proc/sys/net/ipv4/tcp_keepalive_intvl 75
    */    
    int idle = 7200;
    int probe = 5;
    int intvl = 150;
    unsigned int len;

    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&idle,sizeof(int))==-1){
        perror("TCP_KEEPIDLE ");
        close(fd);
        return;
    }    
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, (void *)&probe,sizeof(int))==-1){
        perror("TCP_KEEPCNT");
        close(fd);
        return;
    }    
   
    if (setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, (void *)&intvl,sizeof(int))==-1){
        perror("TCP_KEEPINTVL ");
        close(fd);
        return;
    }    
    log_debug("tcp TCP_KEEPIDLE:%d",idle);     
    log_debug("tcp TCP_KEEPCNT:%d",probe);     
    log_debug("tcp TCP_KEEPINTVL:%d",intvl); 
}

void demo_TCP_LINGER2()
{
    //since Linux 2.4
    //不要与SO_LINGER混淆，指的的FIN_WAIT2的状态的生存期，发出fin，收到ack后进入FIN_WAIT2状态，如果
    //在timeout时间内还未收到fin，则关闭连接

    /*
    默认值：
     /proc/sys/net/ipv4/tcp_fin_timeout 60
    */    
    int timeout = 90;
   
    unsigned int len;

    if (setsockopt(fd, IPPROTO_TCP, TCP_LINGER2, (void *)&timeout,sizeof(int))==-1){
        perror("TCP_LINGER2");
        close(fd);
        return;
    }    
    
    log_debug("tcp TCP_LINGER2 :%d",timeout);     
}

void demo_TCP_MAXSEG()
{
   //The maximum segment size for outgoing TCP packets. 
   //In Linux 2.2 and earlier, and in Linux 2.6.28 and later, if this option is set before connection establishment, 
   //it also changes the MSS value announced to the other end in the initial packet.
   //Values greater than the (eventual) interface MTU have no effect. 
   //TCP will also impose its minimum and maximum bounds over the value provided.

    /*
    默认值：
     /proc/sys/net/ipv4/tcp_fin_timeout 60
    */    
    int maxseg = 1024;
   
    unsigned int len;

    if (setsockopt(fd, IPPROTO_TCP, TCP_MAXSEG, (void *)&maxseg,sizeof(int))==-1){
        perror("TCP_MAXSEG");
        close(fd);
        return;
    }    
    
    log_debug("tcp TCP_MAXSEG :%d",maxseg);
}

void demo_TCP_NODELAY()
{
    //禁用nagle算法
    /*If set, disable the Nagle algorithm. This means that segments are always sent as soon as possible, 
      even if there is only a small amount of data. When not set, data is buffered until there is a 
      sufficient amount to send out, thereby avoiding the frequent sending of small packets, which 
      results in poor utilization of the network. This option is overridden by TCP_CORK; however, 
      setting this option forces an explicit flush of pending output, even if TCP_CORK is currently set.
    */
    int on = 1;
    unsigned int len;

    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY , (void *)&on,sizeof(int))==-1){
        perror("TCP_NODELAY ");
        close(fd);
        return;
    }    
    
    log_debug("tcp TCP_NODELAY  :%d",on);
}

void demo_TCP_QUICKACK()
{
    //since Linux 2.4.4
    //收到数据包立即发送ack，但并不完全是这样，可能会被其他协议层的事物所覆盖
    int on = 1;
    unsigned int len;

    if (setsockopt(fd, IPPROTO_TCP, TCP_QUICKACK , (void *)&on,sizeof(int))==-1){
        perror("TCP_QUICKACK ");
        close(fd);
        return;
    }    
    
    log_debug("tcp TCP_QUICKACK  :%d",on);
}

void demo_TCP_SYNCNT()
{
    //since Linux 2.4
    //syn的重传次数，不能超过255
    //默认值 /proc/sys/net/ipv4/tcp_syn_retries      6
    int cnt = 7;
    unsigned int len;

    if (setsockopt(fd, IPPROTO_TCP, TCP_SYNCNT, (void *)&cnt,sizeof(int))==-1){
        perror("TCP_SYNCNT ");
        close(fd);
        return;
    }    
    
    log_debug("tcp TCP_SYNCNT  :%d",cnt);
}

void demo_TCP_WINDOW_CLAMP()
{
    //since Linux 2.4
    //tcp的广告窗口将设置为此值
    //内核的最小值为SOCK_MIN_RCVBUF /2 = 1152
    /*
#define TCP_SKB_MIN_TRUESIZE	(2048 + SKB_DATA_ALIGN(sizeof(struct sk_buff)))
    
#define SOCK_MIN_SNDBUF		(TCP_SKB_MIN_TRUESIZE * 2)
#define SOCK_MIN_RCVBUF		 TCP_SKB_MIN_TRUESIZE
    */
    int size = 1300;
    unsigned int len;

    if (setsockopt(fd, IPPROTO_TCP, TCP_WINDOW_CLAMP, (void *)&size,sizeof(int))==-1){
        perror("TCP_WINDOW_CLAMP ");
        close(fd);
        return;
    }   

    size = -1;
    if (getsockopt(fd, IPPROTO_TCP, TCP_WINDOW_CLAMP, (void *)&size,&len)==-1){
        perror("TCP_WINDOW_CLAMP ");
        close(fd);
        return;
    }  
    
    log_debug("tcp TCP_WINDOW_CLAMP  :%d",size);
}

//与tcp相关的ioctl操作 SIOCINQ        SIOCOUTQ   SIOCATMARK
//int value;error = ioctl(tcp_socket, ioctl_type, &value);
void demo_SIOCINQ()
{
    //Returns the amount of queued unread data in the receive buffer. The socket must not be in LISTEN state
    int uread_len;
    int error = ioctl(fd, SIOCINQ, &uread_len);

    log_debug("tcp SIOCINQ:%d",uread_len);
}

void demo_SIOCOUTQ()
{
    //Returns the amount of unsent data in the socket send queue. The socket must not be in LISTEN state
    int usend_len;
    int error = ioctl(fd, SIOCOUTQ, &usend_len);

    log_debug("tcp SIOCOUTQ:%d",usend_len);
}

void demo_SIOCATMARK()
{
    /*If the SO_OOBINLINE socket option is set, and SIOCATMARK returns true, then the next read from the socket 
      will return the urgent data. If the SO_OOBINLINE socket option is not set, and SIOCATMARK returns true, 
      then the next read from the socket will return the bytes following the urgent data 
      (to actually read the urgent data requires the recv(MSG_OOB) flag).
    */
    int atmark;
    int error = ioctl(fd, SIOCATMARK, &atmark);

    log_debug("tcp SIOCATMARK:%d",atmark);
}

/*
SIGPIPE 

When writing onto a connection-oriented socket that has been shut down (by the local or the remote end) SIGPIPE is sent  to  the  writing  process  and
EPIPE is returned.  The signal is not sent when the write call specified the MSG_NOSIGNAL flag.
*/
int demo_tcp_init(void)
{
    int ret;
    
    fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(-1 == fd){
        perror("create socket error.");
        return -1;
    }

    //SO_REUSEADDR 1、防止time wait
    //SO_REUSEADDR 2、当有INADDR_ANY绑定了某个端口，则该端口不能绑定任何本地ip地址 
    int onoff = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&onoff, sizeof(onoff))==-1){
        perror("SO_REUSEADDR");
        close(fd);
        return -1;
    }
    
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *)&onoff, sizeof(onoff))==-1){
        perror("SO_REUSEPORT");
    }
    
    demo_SO_ACCEPTCONN();    
    demo_SO_BINDTODEVICE();
    //demo_SO_DOMAIN();
    //demo_SO_ERROR();
    demo_SO_DONTROUTE();
    //使能keep alive
    demo_SO_KEEPALIVE();
    demo_SO_LINGER();
    demo_SO_MARK();
    demo_SO_OOBINLINE();
    //demo_SO_PEEK_OFF();
    demo_SO_PRIORITY();
    demo_SO_PROTOCOL();
    demo_SO_RCVBUF();
    demo_SO_RCVBUFFORCE();
    demo_SO_SNDBUF();
    demo_SO_SNDBUFFORCE();
    demo_SO_RCVLOWAT();
    demo_SO_SNDLOWAT();
    demo_SO_RCVTIMEO();
    demo_SO_SNDTIMEO();
    demo_SO_TIMESTAMP();
    //demo_SO_TYPE();

    demo_TCP_CORK();
    demo_TCP_DEFER_ACCEPT();
    demo_TCP_INFO();
    demo_TCP_KEEPCNT();
    demo_TCP_LINGER2();
    demo_TCP_MAXSEG();
    demo_TCP_NODELAY();
    demo_TCP_QUICKACK();
    demo_TCP_SYNCNT();
    demo_TCP_WINDOW_CLAMP();

    //设置进程不继承fd
    if(fcntl(fd, F_SETFD, FD_CLOEXEC) < 0) {
        perror("FD_CLOEXEC");
    }

    //阻塞
    int flags = fcntl(fd, F_GETFL, 0);
    flags &= (~O_NONBLOCK);
    if (fcntl(fd,F_SETFL,flags) < 0) {
		perror("unw_set_block error.");
        close(fd);
        return -1;
    }

	//bind
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(12130);
    sock_addr.sin_addr.s_addr = htonl(0);    
    ret = bind(fd,(struct sockaddr*)&sock_addr, sizeof(struct sockaddr_in));
	if(ret != 0) {		
		perror("bind error.");
		close(fd);
		return -1;
	}

	//listen 一般为4 新内核5.8.9支持4096
    ret = listen(fd, 4);
    if(0 != ret)
    {
        perror("unw_listen error.");
        close(fd);
        return -1;
    }

    demo_SO_ACCEPTCONN();
    
    return 0;    
}

void demo_SO_BROADCAST()
{
    int onoff = 1;
    unsigned int len;
    if (setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, (void *)&onoff, sizeof(int))==-1){
        perror("SO_BROADCAST");
        close(udp_fd);
        return;
    }    
    
    int on = -1;
    if (getsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, (void *)&on, &len)==-1){
        perror("SO_BROADCAST");
        close(udp_fd);
        return;
    }   
    
    log_debug("udp is broadcast? :%d",on);
}

int demo_udp_init(void)
{
    int ret;

    udp_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(-1 == udp_fd){
       perror("create socket error.");
       return -1;
    }

    demo_SO_BROADCAST();
    return 0;
}

//IPPROTO_IP层的操作
/*
tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
raw_socket = socket(AF_INET, SOCK_RAW, protocol);
*/
static int demo_SIOCGIFINDEX(char *ifname)
{
    struct ifreq req;
    memset(&req,0,sizeof(struct ifreq));

    strcpy(req.ifr_name,ifname);
    if(ioctl(fd,SIOCGIFINDEX,&req) == -1) {
        perror("ioctl");
        return -1;
    }
    log_debug("%s index:%d",ifname,req.ifr_ifindex);
    return req.ifr_ifindex;
}

#define local_ip "192.168.137.2"
#define multi_ip "224.0.0.100"
#define other_ip "192.168.137.5"
#define ethname "eth0"
void demo_IP_ADD_MEMBERSHIP()
{
    /*since Linux 1.2
    Join a multicast group. Argument is an ip_mreqn structure.
    struct ip_mreqn {
        struct in_addr imr_multiaddr; // IP multicast group
                                         //address
        struct in_addr imr_address;   // IP address of local
                                        // interface 
        int            imr_ifindex;   // interface index 
    };
    组播的地址是D类地址，从224.0.0.0—239.255.255.255，而且一些地址有特定的用处如，
    224.0.0.0—244.0.0.255只能用于局域网中zhi路由器，是不会转发的，并且224.0.0.1是所有主机的地址
    ，224.0.0.2所有路由器的地址，224.0.0.5所有ospf路由器的地址，224.0.0.3是PIMv2路由器的地址；
    239.0.0.0—239.255.255.255是私有地址（如192.168.x..x）；224.0.1.0—238.255.255.255可以用与Internet上的。
    */

    struct ip_mreqn mreq;
    memset(&mreq,0,sizeof(struct ip_mreqn));

    int index = demo_SIOCGIFINDEX(ethname);
    
    mreq.imr_address.s_addr = inet_addr(local_ip);    
    mreq.imr_multiaddr.s_addr = inet_addr(multi_ip);    
    mreq.imr_ifindex = (index==-1)?0:index;
    
    if (setsockopt(ip_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&mreq,sizeof(struct ip_mreqn))==-1){
       perror("set IP_ADD_MEMBERSHIP");
       return;
    }   

    log_debug("ip IP_ADD_MEMBERSHIP success:%s",multi_ip);
}

void demo_IP_ADD_SOURCE_MEMBERSHIP()
{
    /*since Linux 2.4.22 / 2.5.68
    Join a multicast group and allow receiving data only from a
      specified source.  Argument is an ip_mreq_source structure.

    struct ip_mreq_source {
       struct in_addr imr_multiaddr;  // IP multicast group
                                         address 
       struct in_addr imr_interface;  // IP address of local
                                         interface 
       struct in_addr imr_sourceaddr; // IP address of
                                         multicast source 
    };
    This option can be used multiple times to allow receiving data
    from more than one source.
    
    */

    struct ip_mreq_source mreq;
    memset(&mreq,0,sizeof(struct ip_mreq_source));
    
    mreq.imr_interface.s_addr = inet_addr(local_ip);
    mreq.imr_multiaddr.s_addr = inet_addr(multi_ip);    
    mreq.imr_sourceaddr.s_addr = inet_addr(other_ip);
    
    if (setsockopt(ip_fd, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, (void *)&mreq,sizeof(struct ip_mreq_source))==-1){
       perror("set IP_ADD_SOURCE_MEMBERSHIP");
       return;
    }   

    log_debug("ip IP_ADD_SOURCE_MEMBERSHIP success:%s",other_ip);
}

void demo_IP_BIND_ADDRESS_NO_PORT()
{
    //低版本不支持此选项
    /*since Linux 4.2
    Inform the kernel to not reserve an ephemeral port when using
    bind(2) with a port number of 0.  The port will later be auto‐
    matically chosen at connect(2) time, in a way that allows
    sharing a source port as long as the 4-tuple is unique.

    bind通过srcip,srcport确定端口是否被占用，容易耗尽。所以将分配端口号移到connet阶段
    */

/*
    int on = 1;
    if (setsockopt(ip_fd, IPPROTO_IP, IP_BIND_ADDRESS_NO_PORT, (void *)&on,sizeof(int))==-1){
       perror("set IP_BIND_ADDRESS_NO_PORT");
       return;
    }   

    log_debug("ip IP_BIND_ADDRESS_NO_PORT success:%d",on);*/
}

void demo_IP_BLOCK_SOURCE()
{
    /*since Linux 2.4.22 / 2.5.68
        Stop receiving multicast data from a specific source in a
        given group.  This is valid only after the application has
        subscribed to the multicast group using either IP_ADD_MEMBER‐
        SHIP or IP_ADD_SOURCE_MEMBERSHIP.
    */

    struct ip_mreq_source mreq;
    memset(&mreq,0,sizeof(struct ip_mreq_source));
    
    mreq.imr_interface.s_addr = inet_addr(local_ip);
    mreq.imr_multiaddr.s_addr = inet_addr(multi_ip);    
    mreq.imr_sourceaddr.s_addr = inet_addr(other_ip);
    
    if (setsockopt(ip_fd, IPPROTO_IP, IP_BLOCK_SOURCE, (void *)&mreq,sizeof(struct ip_mreq_source))==-1){
       perror("set IP_BLOCK_SOURCE");
       return;
    }   

    log_debug("ip IP_BLOCK_SOURCE success:%s",other_ip);
}

void demo_IP_UNBLOCK_SOURCE()
{
    /*since Linux 2.4.22 / 2.5.68
        Stop receiving multicast data from a specific source in a
        given group.  This is valid only after the application has
        subscribed to the multicast group using either IP_ADD_MEMBER‐
        SHIP or IP_ADD_SOURCE_MEMBERSHIP.
    */

    struct ip_mreq_source mreq;
    memset(&mreq,0,sizeof(struct ip_mreq_source));
    
    mreq.imr_interface.s_addr = inet_addr(local_ip);
    mreq.imr_multiaddr.s_addr = inet_addr(multi_ip);    
    mreq.imr_sourceaddr.s_addr = inet_addr(other_ip);
    
    if (setsockopt(ip_fd, IPPROTO_IP, IP_UNBLOCK_SOURCE, (void *)&mreq,sizeof(struct ip_mreq_source))==-1){
       perror("set IP_UNBLOCK_SOURCE");
       return;
    }   

    log_debug("ip IP_UNBLOCK_SOURCE success:%s",other_ip);
}

void demo_IP_DROP_MEMBERSHIP()
{
    /*since Linux 1.2
    Join a multicast group. Argument is an ip_mreqn structure.
    struct ip_mreqn {
        struct in_addr imr_multiaddr; // IP multicast group
                                         //address
        struct in_addr imr_address;   // IP address of local
                                        // interface 
        int            imr_ifindex;   // interface index 
    };
    组播的地址是D类地址，从224.0.0.0—239.255.255.255，而且一些地址有特定的用处如，
    224.0.0.0—244.0.0.255只能用于局域网中zhi路由器，是不会转发的，并且224.0.0.1是所有主机的地址
    ，224.0.0.2所有路由器的地址，224.0.0.5所有ospf路由器的地址，224.0.0.3是PIMv2路由器的地址；
    239.0.0.0—239.255.255.255是私有地址（如192.168.x..x）；224.0.1.0—238.255.255.255可以用与Internet上的。
    */

    struct ip_mreqn mreq;
    memset(&mreq,0,sizeof(struct ip_mreqn));

    int index = demo_SIOCGIFINDEX(ethname);
    
    mreq.imr_address.s_addr = inet_addr(local_ip);    
    mreq.imr_multiaddr.s_addr = inet_addr(multi_ip);    
    mreq.imr_ifindex = (index==-1)?0:index;
    
    if (setsockopt(ip_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void *)&mreq,sizeof(struct ip_mreqn))==-1){
       perror("set IP_DROP_MEMBERSHIP");
       return;
    }   

    log_debug("ip IP_DROP_MEMBERSHIP success:%s",multi_ip);
}

void demo_IP_DROP_SOURCE_MEMBERSHIP()
{
    /*since Linux 2.4.22 / 2.5.68
    Join a multicast group and allow receiving data only from a
      specified source.  Argument is an ip_mreq_source structure.

    struct ip_mreq_source {
       struct in_addr imr_multiaddr;  // IP multicast group
                                         address 
       struct in_addr imr_interface;  // IP address of local
                                         interface 
       struct in_addr imr_sourceaddr; // IP address of
                                         multicast source 
    };
    This option can be used multiple times to allow receiving data
    from more than one source.
    
    */

    struct ip_mreq_source mreq;
    memset(&mreq,0,sizeof(struct ip_mreq_source));
    
    mreq.imr_interface.s_addr = inet_addr(local_ip);
    mreq.imr_multiaddr.s_addr = inet_addr(multi_ip);    
    mreq.imr_sourceaddr.s_addr = inet_addr(other_ip);
    
    if (setsockopt(ip_fd, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP, (void *)&mreq,sizeof(struct ip_mreq_source))==-1){
       perror("set IP_DROP_SOURCE_MEMBERSHIP");
       return;
    }   

    log_debug("ip IP_DROP_SOURCE_MEMBERSHIP success:%s",other_ip);
}

void demo_IP_FREEBIND()
{
    /*since Linux 2.4
    If enabled, this boolean option allows binding to an IP
    address that is nonlocal or does not (yet) exist.  This per‐
    mits listening on a socket, without requiring the underlying
    network interface or the specified dynamic IP address to be up
    at the time that the application is trying to bind to it.
    This option is the per-socket equivalent of the ip_nonlo‐
    cal_bind /proc interface described below.

    允许bind到一个非本或者不存在的ip，并允许监听
    */

    int on = 1;
    if (setsockopt(ip_fd, IPPROTO_IP, IP_FREEBIND, (void *)&on,sizeof(int))==-1){
       perror("set IP_FREEBIND");
       return;
    }   

    log_debug("ip IP_FREEBIND success:%d",on);
}

void demo_IP_HDRINCL()
{
    /*since Linux 2.0
    If enabled, the user supplies an IP header in front of the
    user data.  Valid only for SOCK_RAW sockets;

    应用层添加ip头部 支持tcp/udp/icmp/igmp
    */

    int on = 1;
    if (setsockopt(ip_fd, IPPROTO_IP, IP_HDRINCL, (void *)&on,sizeof(int))==-1){
       perror("set IP_HDRINCL");
       return;
    }   

    log_debug("ip IP_HDRINCL success:%d",on);
}

void demo_IP_MSFILTER()
{
    /*since Linux 2.4.22 / 2.5.68
    过滤某些多播原地址发来的多播报文，imsf_fmode为MCAST_INCLUDE and MCAST_EXCLUDE，
    指定是否接受这些原地址发过来的多播信息

    struct ip_msfilter {
       struct in_addr imsf_multiaddr; // IP multicast group
                                         //address 
       struct in_addr imsf_interface; // IP address of local
                                         //interface 
       uint32_t       imsf_fmode;     // Filter-mode 

       uint32_t       imsf_numsrc;    // Number of sources in
                                        // the following array 
       struct in_addr imsf_slist[1];  // Array of source
                                        // addresses 
    };
    
#define IP_MSFILTER_SIZE(numsrc) \
(sizeof(struct ip_msfilter) - sizeof(__u32) \
+ (numsrc) * sizeof(__u32))

#define MCAST_EXCLUDE	0
#define MCAST_INCLUDE	1
	
    */
    
    demo_IP_ADD_MEMBERSHIP();

    struct ip_msfilter *filter;
    struct in_addr filter_ipaddr[] = {inet_addr(local_ip),inet_addr("192.168.137.8")};

    filter = (struct ip_msfilter *)malloc(IP_MSFILTER_SIZE(2));
    memset(filter,0,IP_MSFILTER_SIZE(2));
    
    filter->imsf_multiaddr.s_addr = inet_addr(multi_ip);
    filter->imsf_interface.s_addr = inet_addr(local_ip);
    filter->imsf_fmode = MCAST_INCLUDE;    
    filter->imsf_fmode = 2;    
    filter->imsf_slist[0] = filter_ipaddr[0];    
    filter->imsf_slist[1] = filter_ipaddr[1];
    
    
    if (setsockopt(ip_fd, IPPROTO_IP, IP_MSFILTER, (void *)filter,IP_MSFILTER_SIZE(2))==-1){
       perror("set IP_MSFILTER");
       return;
    }   

    log_debug("ip IP_MSFILTER success");
}

void demo_IP_MTU()
{
    /*since Linux 2.2
    only when the socket has been connected
    */

    int mtu = -1;
    unsigned int len= 0;
    if (getsockopt(fd, IPPROTO_IP, IP_MTU, (void *)&mtu,&len)==-1){
       perror("set IP_MTU");
       return;
    }   

    log_debug("ip IP_MTU success:%d",mtu);
}

void demo_IP_MTU_DISCOVER()
{
    /*since Linux 2.2

    IP_PMTUDISC_DO 代表始终不分片，用户程序有必要检测每个报文不超过mtu，mtu通过IP_MTU获取
    
    Path MTU discovery value   Meaning
    IP_PMTUDISC_WANT           Use per-route settings.
    IP_PMTUDISC_DONT           Never do Path MTU Discovery.
    IP_PMTUDISC_DO             Always do Path MTU Discovery.
    IP_PMTUDISC_PROBE          Set DF but ignore Path MTU.
    */

    int value = IP_PMTUDISC_DO;
    if (setsockopt(fd, IPPROTO_IP, IP_MTU_DISCOVER, (void *)&value,sizeof(int))==-1){
       perror("set IP_MTU_DISCOVER");
       return;
    }   

    log_debug("ip IP_MTU_DISCOVER success:%d",value);
}

void demo_IP_MULTICAST_ALL()
{
    /*since Linux 2.6.31
    bool值，默认开启，接受所有多播组的报文；关闭则只接受加入的多播组的报文
    */

    int on = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_MULTICAST_ALL, (void *)&on,sizeof(int))==-1){
       perror("set IP_MULTICAST_ALL");
       return;
    }   

    log_debug("ip IP_MULTICAST_ALL success:%d",on);
}

void demo_IP_MULTICAST_IF()
{
    /*since Linux 1.2
    设置组播的默认默认网络接口，会从给定的网络接口发送，另一个网络接口会忽略此数据
    struct ip_mreqn {
        struct in_addr imr_multiaddr; // IP multicast group
                                         //address
        struct in_addr imr_address;   // IP address of local
                                        // interface 
        int            imr_ifindex;   // interface index 
    };
   
    */

    struct ip_mreqn mreq;
    memset(&mreq,0,sizeof(struct ip_mreqn));

    int index = demo_SIOCGIFINDEX(ethname);
    
    mreq.imr_address.s_addr = inet_addr(local_ip);    
    mreq.imr_multiaddr.s_addr = inet_addr(multi_ip);    
    mreq.imr_ifindex = (index==-1)?0:index;
    
    if (setsockopt(ip_fd, IPPROTO_IP, IP_MULTICAST_IF, (void *)&mreq,sizeof(struct ip_mreqn))==-1){
       perror("set IP_MULTICAST_IF");
       return;
    }   

    log_debug("ip IP_MULTICAST_IF success:%s",multi_ip);
}

void demo_IP_MULTICAST_LOOP()
{
    /*since Linux 1.2
    发送出去的数据是否回环到本地接口
    */

    int on = 1;
    if (setsockopt(ip_fd, IPPROTO_IP, IP_MULTICAST_LOOP, (void *)&on,sizeof(int))==-1){
       perror("set IP_MULTICAST_LOOP");
       return;
    }   

    log_debug("ip IP_MULTICAST_LOOP success:%d",on);
}

void demo_IP_MULTICAST_TTL()
{
    /*since Linux 1.2
    默认值为1,表示只在局域网内
    */

    unsigned char on = 255;
    if (setsockopt(ip_fd, IPPROTO_IP, IP_MULTICAST_TTL , (void *)&on,sizeof(char))==-1){
       perror("set IP_MULTICAST_TTL ");
       return;
    }   

    log_debug("ip IP_MULTICAST_TTL  success:%d",on);
}

void demo_IP_OPTIONS()
{
    /*since Linux 2.0
    参数按照ip选项的标准格式，最大不能超过40字节
    type,len offset,data
    具体实现没有实现 根据具体需求
    */

    unsigned char options[40];
    if (setsockopt(fd, IPPROTO_IP, IP_OPTIONS , (void *)&options,8)==-1){
       perror("set IP_OPTIONS ");
       return;
    }   

    log_debug("ip IP_OPTIONS  success");
}

void demo_IP_TTL()
{
    /*since Linux 1.0
    set or get
    */

    int ttl= 100;
    if (setsockopt(ip_fd, IPPROTO_IP, IP_TTL , (void *)&ttl,sizeof(int))==-1){
       perror("set IP_TTL ");
       return;
    }   

    log_debug("ip IP_TTL  success:%d",ttl);
}

void demo_IP_TOS()
{
    /*since Linux 1.0
    set or get  一个字节 前三个bit为0，最后一个bit保留，其他四个bit
    1000 -- minimize delay #最小延迟
    0100 -- maximize throughput #最大吞吐量
    0010 -- maximize reliability #最高可靠性
    0001 -- minimize monetary cost #最小费用
    0000 -- normal service #一般服务
    
#define	IPTOS_LOWDELAY		0x10
#define	IPTOS_THROUGHPUT	0x08
#define	IPTOS_RELIABILITY	0x04
#define	IPTOS_MINCOST		0x02

    */

    int tos= IPTOS_LOWDELAY;
    if (setsockopt(ip_fd, IPPROTO_IP, IP_TOS , (void *)&tos,sizeof(int))==-1){
       perror("set IP_TOS ");
       return;
    }   

    log_debug("ip IP_TOS  success:%d",tos);
}

//有些选项区分stream，dgram或者raw 注意
int demo_ip_init(void)
{
    int ret;

    ip_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(-1 == ip_fd){
       perror("create socket error.");
       return -1;
    }


    demo_IP_ADD_MEMBERSHIP();
    demo_IP_ADD_SOURCE_MEMBERSHIP();
    //demo_IP_BIND_ADDRESS_NO_PORT();
    demo_IP_BLOCK_SOURCE();
    demo_IP_UNBLOCK_SOURCE();

    //这两个执行其一
    //demo_IP_DROP_MEMBERSHIP();
    demo_IP_DROP_SOURCE_MEMBERSHIP();

    demo_IP_FREEBIND();
    demo_IP_HDRINCL();
    demo_IP_MSFILTER();
    demo_IP_MTU();
    demo_IP_MTU_DISCOVER();
    demo_IP_MULTICAST_ALL();
    demo_IP_MULTICAST_IF();
    demo_IP_MULTICAST_LOOP();
    
    demo_IP_MULTICAST_TTL();
    //demo_IP_OPTIONS();
    demo_IP_TTL();
    demo_IP_TOS();
    return 0;
}

int main()
{
    int ret;
    func_exec(demo_tcp_init(), ret);    
    func_exec(demo_udp_init(), ret);    
    func_exec(demo_ip_init(), ret);

}


