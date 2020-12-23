/*
    *鏈┍鍔ㄦ槸鍏夌収浼犳劅鍣˙H1750 (I2C鎬荤嚎) 鐨勯┍鍔ㄩ儴鍒嗭紝 浣跨敤杩炵画杞寲妯″紡
*/


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
#include <linux/i2c.h>


#define BH1750_major		105

static struct i2c_device_id my_i2c_idtable[]={
   	{"bh1750",0	},
   	{},
};


volatile struct class		*BH1750_class;
volatile struct class_device	*BH1750_drv_class_device;
static struct i2c_client *bh1750_client;

static int bh1750_data[2];
static unsigned char bh1750_cmd[] = {0x01,0x10};    //0x01閲嶇疆
                                                                            //0x11鍚姩杞寲

static int BH1750_open(struct inode *inode, struct file *file)
{
    printk("open the bh1750 success\n");
    return 0;
}


static ssize_t BH1750_read(struct file *file, char __user *buff, size_t count, loff_t *offp)
{
    char data_buf[5]; 
	int ret;
	
	ret=i2c_smbus_write_byte(bh1750_client,0x01);    
	ret=i2c_smbus_write_byte(bh1750_client,0x10);
	if(ret)
		printk("i2c failed write\n");
	mdelay(120);

	bh1750_data[0]=i2c_smbus_read_byte(bh1750_client);  //返回值为负值说明错误	
	bh1750_data[1]=i2c_smbus_read_byte(bh1750_client);  //返回值为负值说明错误

    if(bh1750_data[0]<0)
		printk("read failed\n");
	data_buf[0] = bh1750_data[0];	
	data_buf[1] = bh1750_data[1];
	
	printk("%02x %02x\n",bh1750_data[0],bh1750_data[1]);
    copy_to_user(buff, data_buf, 2);
	return 0;
}


static struct file_operations BH1750_drv_fops = {
    .owner  =   THIS_MODULE,    /* 杩欐槸涓�涓畯锛屾帹鍚戠紪璇戞ā鍧楁椂鑷姩鍒涘缓鐨刜_this_module鍙橀噺 */
    .open   =   BH1750_open,     
    .read   =   BH1750_read,
   // .release = BH1750_release,
};

static int BH1750_probe(struct i2c_client *i2c_client)
{ 
       bh1750_client = i2c_client;
        
	   printk("has found\n");
	   
	   register_chrdev(BH1750_major, "bh1750", &BH1750_drv_fops);
       BH1750_class = class_create(THIS_MODULE, "bh1750s");
       BH1750_drv_class_device= device_create(BH1750_class,NULL,MKDEV(BH1750_major, 1),NULL,"bh1750");
       return 0;
}


static struct i2c_driver BH1750_driver = {
	.driver = {
		 .owner	= THIS_MODULE,
		 .name	= "bh1750",
	},		
	.probe  = BH1750_probe,
	.id_table = my_i2c_idtable,	
};

static int __init BH1750_drv_init(void)
{
    int res;
	res = i2c_add_driver(&BH1750_driver);
	if(res == 0)
	{
            printk("i2c add driver success\n");
	}
	else
	{
            printk("i2c add driver failed\n");
	}
	return res;
}

static void __exit BH1750_drv_exit(void)
{        
	printk("bh1750 exit\n");
    i2c_del_driver(&BH1750_driver);
}  

module_init(BH1750_drv_init);
module_exit(BH1750_drv_exit);
MODULE_LICENSE("Dual BSD/GPL");



