
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

#define low  0              //  �����ƽ�ĸߵ�
#define high 1

#define jump_rom 0xCC
#define convert  0x44
#define read_ram 0xBE



static int major;
static struct class *my_class;
static unsigned int ds_pin = S3C2410_GPB(5);  //DS18B20��io��

static int ds18b20_find_success(void);
static void ds18b20_reset(void);
static void ds18b20_write_data(unsigned char dat);
static void ds18b20_work_init(void);
static void ds18b20_convert(void);
static unsigned char ds18b20_readbit(void);
static unsigned char ds18b20_readbyte(void);

static int ds18b20_find_success(void)
{
   unsigned int tmp_pin;
   
   s3c2410_gpio_cfgpin(ds_pin,S3C2410_GPIO_OUTPUT);
   s3c2410_gpio_setpin(ds_pin,low);
   udelay(600);
   s3c2410_gpio_setpin(ds_pin,high);  
   udelay(100);
   
   s3c2410_gpio_cfgpin(ds_pin,S3C2410_GPIO_INPUT);
   tmp_pin = s3c2410_gpio_getpin(ds_pin);

   if(tmp_pin == low)
   {
      printk("ds18b20 reset success\n");
      return 1;
   }
   else
   {
     printk("ds18b20 reset failed\n");
   	 return 0;
   }
   
}
static void ds18b20_reset(void)
{   
   s3c2410_gpio_cfgpin(ds_pin,S3C2410_GPIO_OUTPUT);
   s3c2410_gpio_setpin(ds_pin,low);
   udelay(600);
   s3c2410_gpio_setpin(ds_pin,high);  
   udelay(100);

   //��Ҫ������������
   s3c2410_gpio_cfgpin(ds_pin,S3C2410_GPIO_INPUT);
   udelay(100);   
}

static void ds18b20_write_data(unsigned char dat)
{
   int i;
   unsigned int tmp;
   
   for(i=0;i<8;i++)
   {
      tmp = (dat>>i)&0x1; 

      if(tmp == 1)
      {
	     s3c2410_gpio_cfgpin(ds_pin,S3C2410_GPIO_OUTPUT);  //�������߱�ʾҪд  д1
         s3c2410_gpio_setpin(ds_pin,low);
         udelay(5);
	  
         s3c2410_gpio_setpin(ds_pin,high);         //�ͷ�����
         udelay(80);
      }
	  else
	  {
	     s3c2410_gpio_cfgpin(ds_pin,S3C2410_GPIO_OUTPUT);  //�������߱�ʾҪд д0
         s3c2410_gpio_setpin(ds_pin,low);
         udelay(80);
	  
         s3c2410_gpio_setpin(ds_pin,high);         //�ͷ�����
         udelay(8);
	  }
	  udelay(3);           //ÿ��д��϶����1us�ļ��
   }
}
static void ds18b20_work_init()
{
    ds18b20_reset();
    ds18b20_write_data(jump_rom);
}
static void ds18b20_convert()
{
    ds18b20_write_data(convert);
	mdelay(1000);    //�ȴ�ds18b20ת�����
}
static unsigned char ds18b20_readbit(void)    
{
	unsigned char  dat;
	s3c2410_gpio_cfgpin(ds_pin, S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_setpin(ds_pin, low);
	udelay(2);
	s3c2410_gpio_setpin(ds_pin, high); //add
	s3c2410_gpio_cfgpin(ds_pin, S3C2410_GPIO_INPUT);
	udelay(10);
	if(s3c2410_gpio_getpin(ds_pin)!=0)
		dat=1;
	else 
		dat=0;
	s3c2410_gpio_cfgpin(ds_pin, S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_setpin(ds_pin, high);//add
	udelay(50);
	return (dat);
}

static unsigned char ds18b20_readbyte(void)   
{
	unsigned char i,j,dat;
	dat=0;
		
	for(i=1;i<=8;i++)
	{
		j=ds18b20_readbit();
		dat=(j<<7)|(dat>>1);   //�������������λ����ǰ�棬�����պ�һ���ֽ���DAT��
	}
	return(dat);
}

int ds18b20_open(struct inode *inode, struct file *file)
{    
    if(ds18b20_find_success())
    {
        printk("find ds18b20 success\n");
		return 0;
    }
	else
	{
	    printk("can not find ds18b20");
		return -1;
	}
}

ssize_t ds18b20_read (struct file *file, char __user *buf, size_t size, loff_t *loff)
{
    unsigned short wendu;
	unsigned char th,tl,tltemp;
	unsigned int fuhao,zheng,xiaoshu;
	char tmp_buf[20];
	unsigned long num_buf;
	
    ds18b20_work_init();   //��ʼת��
	ds18b20_convert();

	ds18b20_work_init();	
    ds18b20_write_data(read_ram);
	udelay(4);

	tl=ds18b20_readbyte();
	th=ds18b20_readbyte();
	wendu=th;
	wendu<<=8;            
	wendu=wendu|tl;

	if((th&0xf8)==0)   //�ж��¶ȷ���λ
		fuhao = 1;
	else
	{
		fuhao = -1;
	    tl=~tl;	 //ȡ��
		th=~th;	  //ȡ��
		tltemp=tl+1; //��λ��1
		tl=tltemp;
		if(tltemp>255) th++;  //�����8λ����255�����8λ��1
	}
	
	 zheng=th*16+tl/16;      //ʵ���¶�ֵ=(TH*256+TL)/16,����TH*16+TL/16
				                  //�����ó������¶ȵ���������,С�����ֱ�������
	 xiaoshu=(tl%16)*10/16;    //�����¶ȵ�С������,����������10�ٳ���16ȡ��

	 if(fuhao == 1)
	 {
	    sprintf(tmp_buf,"%d.%d��",zheng,xiaoshu);
	 }
	 else	    
	    sprintf(tmp_buf,"-%d.%d��",zheng,xiaoshu);
	 
	 num_buf = copy_to_user(buf, tmp_buf, sizeof(tmp_buf));

	 return num_buf;
	
}
int ds18b20_close(struct inode *inode, struct file *file)
{
    printk("ds18b20 close\n");
	return 0;
}
static struct file_operations ds18_ops={
   .owner   = THIS_MODULE,
   .open    = ds18b20_open,
   .read    = ds18b20_read,
   .release = ds18b20_close,
};

static int ds18b20_init(void)
{
   int ret;
   
   ret = ds18b20_find_success();
   if(ret == 0)
   {
      return -ENODEV;
   }
   
   major = register_chrdev(0,"junzi_ds18b20",&ds18_ops);
   if(major<0)
   {
      printk("alloc major number failed\n");
	  return -ENOMEM;
   }

   my_class = class_create(THIS_MODULE,"junzic_ds18b20");
   device_create(my_class,NULL,MKDEV(major,0),NULL,"junzi_ds18b20");


   return 0;
}

static void ds18b20_exit(void)
{
   device_destroy(my_class,MKDEV(major,0));
   class_destroy(my_class);
   unregister_chrdev(major,"junzi_ds18b20");
}

module_init(ds18b20_init);
module_exit(ds18b20_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");

