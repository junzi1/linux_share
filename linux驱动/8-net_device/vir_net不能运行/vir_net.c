
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/in.h>
#include <linux/skbuff.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/delay.h>

#include <asm/system.h>
#include <asm/io.h>
#include <asm/irq.h>

static struct net_device *my_virt;

//将skb中的mac地址，ip地址，type调换一下，返回给dev 造成ping通的假象
static void construct_false_packet(struct sk_buff *skb,struct net_device *dev)
{
   //skb的数据包:第一个为mac地址,其后为ip地址，ip之后为type
   struct ethhdr *my_eth;
   struct iphdr  *my_ip;
   int *type;
   unsigned char my_tmp[ETH_ALEN];
   __be32 my_iptmp;
   struct sk_buff *rx_skb;
   //调换mac地址
   my_eth =(struct ethhdr*)skb->data; 

   memcpy(my_tmp,my_eth->h_dest,ETH_ALEN);
   memcpy(my_eth->h_dest,my_eth->h_source,ETH_ALEN);
   memcpy(my_eth->h_source,my_tmp,ETH_ALEN);
   //调换ip地址
   my_ip  =(struct iphdr*)(skb->data+sizeof(struct ethhdr));

   my_iptmp    = my_ip->daddr;
   my_ip->daddr= my_ip->saddr;
   my_ip->saddr= my_iptmp;
   //改变type
   type   =(int*)(skb->data+sizeof(struct ethhdr)+sizeof(struct iphdr));
   *type  =0;  //0表示返回的skb包a

   my_ip->check = 0;		   /* and rebuild the checksum (ip needs it) */
   my_ip->check = ip_fast_csum((unsigned char *)my_ip,my_ip->ihl);
	
	// 构造一个sk_buff
   rx_skb = dev_alloc_skb(skb->len + 2);
   skb_reserve(rx_skb, 2); /* align IP on 16B boundary */	
   memcpy(skb_put(rx_skb, skb->len), skb->data, skb->len);

	/* Write metadata, and then pass to the receive level */
   rx_skb->dev = dev;
   rx_skb->protocol = eth_type_trans(rx_skb, dev);
   rx_skb->ip_summed = CHECKSUM_UNNECESSARY; /* don't check it */

   dev->stats.rx_bytes   += rx_skb->len;
   dev->stats.rx_packets +=1;

   netif_rx(rx_skb);  //像dev设备提交skb
   
}
static int ndo_start_xmit(struct sk_buff *skb,struct net_device *dev)
{
   static unsigned int count=0;
   printk("count=%d\n",++count);
   
  /*构造假包返回给net_device，这样就可以ping通*/

   netif_stop_queue(dev);
   construct_false_packet(skb,dev);
   dev_kfree_skb(skb);
   netif_wake_queue(dev);
  
  /*------------------------------------------*/

   dev->stats.tx_bytes   += skb->len;
   dev->stats.tx_packets = count;
   
   return NETDEV_TX_OK;
}
static int my_virt_init(void)
{
   //第一个参数为私有数据
   my_virt=alloc_netdev(0,"junzi_net%d",ether_setup); //分配net_device结构体，用ether_setup初始化结构体

   my_virt->netdev_ops->ndo_start_xmit = ndo_start_xmit;  //发送包函数

   my_virt->flags       |= IFF_NOARP;               //设置这两项才能ping通
   my_virt->features    |= my_virt->NETIF_F_NO_CSUM	;	
   
   register_netdev(my_virt);
   return 0;
}
static void my_virt_exit(void)
{
   unregister_netdev(my_virt);
   free_netdev(my_virt);
}

module_init(my_virt_init);
module_exit(my_virt_exit);

MODULE_LICENSE("LICENSE");
MODULE_AUTHOR("junzi");












