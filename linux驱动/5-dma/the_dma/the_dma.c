
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <mach/regs-gpio.h>
//#include <asm/hardware.h>
#include <linux/poll.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#define MEM_NO_DMA 0
#define MEM_DMA    1

#define TRANS_SIZE (200*1024)  //每块内存大小

struct mydma_regs{
   unsigned long disrc;
   unsigned long disrcc;
   unsigned long didst;
   unsigned long didstc;
   unsigned long dcon;
   unsigned long dstat;
   unsigned long dcsrc;
   unsigned long dcdst;   
   unsigned long dimasktrig;
      
};

unsigned long my_dma_channel[4]={0x4B000000,0x4B000040,0x4B000080,0x4B0000C0}; //DMA通道0-3

static DECLARE_WAIT_QUEUE_HEAD(my_queue);
static unsigned int major;
static struct class *my_class;
static char *my_src,*my_dst;
static dma_addr_t my_src_phys,my_dst_phys;
static volatile struct mydma_regs *mydma_regs;
static volatile int expert=0;     //期望值 用于等待队列

static irqreturn_t my_dma_func(int irq,void *dev_id)
{
   expert=1;
   wake_up_interruptible(&my_queue); //唤醒
   return IRQ_HANDLED;
}
static int my_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long data)
{
   int i;
   memset(my_src,0x11,TRANS_SIZE);
   memset(my_dst,0xff,TRANS_SIZE);

   expert=0;
   switch(cmd)
   {
      case MEM_NO_DMA:
	                  for(i=0;i<TRANS_SIZE;i++)
					  	my_dst[i] = my_src[i];
					  if(memcmp(my_src, my_dst,TRANS_SIZE) == 0)
					  {
					     printk("NO DMA trans OK\n");
					  }
					  else
					  {
					     printk("NO DMA trans ERROR\n");
					  }
	  	              break;
	  case MEM_DMA:   
	  	              mydma_regs->disrc  = my_src_phys;  //源物理地址
	  	              mydma_regs->disrcc = (0<<0) | (0<<1); //AHB 地址递增
	  	              mydma_regs->didst  = my_dst_phys;  //目的物理地址
	  	              mydma_regs->didstc = (0<<0) | (0<<1) | (0<<2); //当TC为0时触发中断 AHB 地址递增
	  	              mydma_regs->dcon   = (0<<31) | (1<<30) | (1<<29) | (0<<28) | (1<<27) | (0<<23) | (1<<22) | (0<<20) | (TRANS_SIZE);
					  	                  /*demand   AHB clk   INT       一次传输  whole     软件触发  noreload   byte       TC
					  	                  * 模式               eable     一个单元  service
					  	                  */
					  mydma_regs->dimasktrig = (1<<1);  //打开通道 接受请求
					  mydma_regs->dimasktrig |= (1<<0); //发送一个请求REQ
					  
					  /*此时等待dma传输完毕发出中断*/
					  wait_event_interruptible(my_queue,expert);

					  if(memcmp(my_src, my_dst,TRANS_SIZE) == 0)
					  {
					     printk("DMA trans OK\n");
					  }
					  else
					  {
					     printk("DMA trans ERROR\n");
					  }
	  	              break;
	   default:
	 	             printk("the cmd is error\n");
					 return -ENOTTY;
	 	             break;
   }
   return 0;
}

static struct file_operations my_ops={
   .owner = THIS_MODULE,
   .ioctl = my_ioctl,
};
static int my_dma_init(void)
{
   int ret;
/*-----------------------注册设备，创建设备结点----------------------------------*/
   major = register_chrdev(0,"junzi_dma",&my_ops); //注册字符设备并申请主设备号

   my_class = class_create(THIS_MODULE,"junzi_class");
   device_create(my_class,NULL,MKDEV(major,0),NULL,"junzi_dmadev");

/*-----------------分配两块内存 对比DMA传输和cpu传输操作------------------------*/
   my_src = dma_alloc_writecombine(NULL,TRANS_SIZE,&my_src_phys,GFP_KERNEL);
   my_dst = dma_alloc_writecombine(NULL,TRANS_SIZE,&my_dst_phys,GFP_KERNEL);
   
/*-------------申请dma中断(DMA传输完毕后会发出一个中断通知系统完成了)------------*/
/*
#define IRQ_DMA0		16
#define IRQ_DMA1		17
#define IRQ_DMA2		18
#define IRQ_DMA3		19
*/
   ret=request_irq(IRQ_DMA2,my_dma_func,NULL,"junzi_dma",&my_dma_func);
   if(ret)
   {
      printk("Request irq failed\n");
	  goto my_out;
   }
/*--------------------ioremap DMA的物理地址，操作其寄存器-----------------------*/ 
   mydma_regs = ioremap(my_dma_channel[2],sizeof(struct mydma_regs));
   if(!mydma_regs)
   {
      printk("ioremap DMA address ERROR\n");
	  goto my_out;
   }
   return 0;
   	
my_out:
   dma_free_writecombine(NULL,TRANS_SIZE,my_src,my_src_phys);
   dma_free_writecombine(NULL,TRANS_SIZE,my_dst,my_dst_phys);
   device_destroy(my_class,MKDEV(major,0));
   class_destroy(my_class);
   unregister_chrdev(major,"junzi_dma");
   return -EBUSY; 
}

static void my_dma_exit(void)
{
   iounmap(mydma_regs);
   free_irq(IRQ_DMA2,&my_dma_func);
   dma_free_writecombine(NULL,TRANS_SIZE,my_src,my_src_phys);
   dma_free_writecombine(NULL,TRANS_SIZE,my_dst,my_dst_phys);
   device_destroy(my_class,MKDEV(major,0));
   class_destroy(my_class);
   unregister_chrdev(major,"junzi_dma");
}

module_init(my_dma_init);
module_exit(my_dma_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");





























