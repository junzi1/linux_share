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
#include <linux/poll.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define low  0
#define high 1

static int major;
static struct class *my_class;

/*
	S3C2410_GPF(5)  : 风扇
	S3C2410_GPF(3)  : 加热板	
	S3C2410_GPF(4)  : 水泵
	S3C2410_GPF(6)  : 光照灯
*/
/*
	默认应该是低电平，因为高电平触发硬件
*/

static int hw_control_open(struct inode *inode, struct file *filep)
{
	int i;
	
	for( i=3; i<7; i++){		
   		s3c2410_gpio_cfgpin(S3C2410_GPF(i),S3C2410_GPIO_OUTPUT); 
   		s3c2410_gpio_setpin(S3C2410_GPF(i),low);
	}
	return 0;
}

static ssize_t hw_control_write(struct file *file, const char __user *buf, size_t size, loff_t *loff)
{
   char data_buf[10];
   int i,j;
   copy_from_user(data_buf,buf,4);   

   for( i=3,j=0; i<7; i++,j++){	
   		if(data_buf[j])
   			s3c2410_gpio_setpin(S3C2410_GPF(i),high);
		else			
			s3c2410_gpio_setpin(S3C2410_GPF(i),low);
	}
   
   return 4;
}
static int hw_control_close(struct inode *inode, struct file *filep)
{
	int i;
	for( i=2; i<8; i++){		
   		s3c2410_gpio_cfgpin(S3C2410_GPF(i),S3C2410_GPIO_INPUT); 
	}
	return 0;	
}

static struct file_operations hw_control_ops={
   .owner   = THIS_MODULE,
   .open    = hw_control_open,
   .write   = hw_control_write,
   .release = hw_control_close,
};

static int __init hw_control_init()
{
   major = register_chrdev(0,"junzi_hwcontrol",&hw_control_ops);
   if(major<0)
   {
	  printk("alloc major number failed\n");
	  return -ENOMEM;
   }
   
   my_class = class_create(THIS_MODULE,"junzic_hwcontrol");
   device_create(my_class,NULL,MKDEV(major,0),NULL,"junzi_hw_control");

   return 0;
}
static void __exit hw_control_exit()
{
   device_destroy(my_class,MKDEV(major,0));
   class_destroy(my_class);
   unregister_chrdev(major,"junzi_hwcontrol");
}

module_init(hw_control_init);
module_exit(hw_control_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");		


