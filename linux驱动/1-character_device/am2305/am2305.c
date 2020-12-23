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

static unsigned int am_data = S3C2410_GPF(0);
unsigned char Sensor_Data[5]={0x00,0x00,0x00,0x00,0x00};

static int am2305_find_success(void)
{
   unsigned int tmp_pin;

   //主机拉低(Min=800US Max=20Ms) 
   s3c2410_gpio_cfgpin(am_data,S3C2410_GPIO_OUTPUT);   
   s3c2410_gpio_setpin(am_data,low);
   mdelay(2);  //延时2Ms
	  
   //释放总线 延时(Min=30us Max=50us)
   //s3c2410_gpio_setpin(am_data,high);
   

   //主机设为输入 判断传感器响应信号 
   s3c2410_gpio_cfgpin(am_data,S3C2410_GPIO_INPUT);   
   udelay(30);//延时30us
   
   tmp_pin = s3c2410_gpio_getpin(am_data);   
   
   if(tmp_pin == low)
   	return 1;

   return 0;
}
static int am2305_open(struct inode *inode, struct file *file)
{
   int ret;

   ret = am2305_find_success();
   if(ret)
   {
      return 0;
   }
   else
   {
      return -1;
   }
}
unsigned char read_SensorData(unsigned int measure_cnt)
{
    unsigned char i,cnt;
	unsigned char buffer,tmp;
	
	buffer = 0;
	for(i=0;i<8;i++)
	{
		cnt=0;
		while(!s3c2410_gpio_getpin(am_data))	//检测上次低电平是否结束
		{	
		}
		//延时Min=26us Max50us 跳过数据"0" 的高电平
		udelay(30);	 //延时30us   
		
		//判断传感器发送数据位
		tmp =0;
		if(s3c2410_gpio_getpin(am_data))	 
		{
		  tmp = 1;
		}  
		
		while(s3c2410_gpio_getpin(am_data))		//等待高电平 结束
		{
		   if(measure_cnt==4 && i==7)
		   {
			  break;
		   }
		}
		buffer <<=1;
		buffer |= tmp;	
	}
	return buffer;
}


static ssize_t am2305_read (struct file *file, char __user *buf, size_t size, loff_t *loff)
{
   unsigned char i;
   char data_buf[20],num_buf;
   
   //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
   if(am2305_find_success())
   {
	  //判断从机是否发出 80us 的低电平响应信号是否结束	 
	  while(!s3c2410_gpio_getpin(am_data))
	  {
	  }
	  
	  //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
	  while(s3c2410_gpio_getpin(am_data))
	  {	     
	  } 
	  
	  // 数据接收	传感器共发送40位数据 
	  // 即5个字节 高位先送  5个字节分别为湿度高位 湿度低位 温度高位 温度低位 校验和
	  // 校验和为：湿度高位+湿度低位+温度高位+温度低位
	  for(i=0;i<5;i++)
	  {
	      Sensor_Data[i] = read_SensorData(i); 
		  data_buf[i]    = (unsigned char)Sensor_Data[i];
	  }
	  
	  num_buf = copy_to_user(buf, data_buf, 5);
	  return num_buf;
  }
  else
  	return -1;
}
static int am2305_close(struct inode *inode, struct file *file)
{
   printk("am2305 close\n");
   return 0;
}
static struct file_operations am2305_ops={
   .owner   = THIS_MODULE,
   .open    = am2305_open,
   .read    = am2305_read,
   .release = am2305_close,
};

static int am2305_init(void)
{  
  
   major = register_chrdev(0,"junzi_am2305",&am2305_ops);
   if(major<0)
   {
	  printk("alloc major number failed\n");
	  return -ENOMEM;
   }

   /*
   创建一个类，这个类存放于sysfs下面，一旦创建好了这个类，再调用device_create(…)函数来在/dev目录下创建相应的设备节点。
   这样，加载模块的时候，用户空间中的udev会自动响应device_create(…)函数，去/sysfs下寻找对应的类从而创建设备节点。
   */
   my_class = class_create(THIS_MODULE,"junzic_am2305");
   device_create(my_class,NULL,MKDEV(major,0),NULL,"junzi_am2305");

   return 0;
}
static void am2305_exit(void)
{
   device_destroy(my_class,MKDEV(major,0));
   class_destroy(my_class);
   unregister_chrdev(major,"junzi_am2305");
}

module_init(am2305_init);
module_exit(am2305_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");		


