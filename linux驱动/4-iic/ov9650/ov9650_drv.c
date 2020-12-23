
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/delay.h>

static int major;
static struct class *my_class;
static struct i2c_client *ov9650_client;
static struct i2c_device_id ov9650_idtable[]={
   	{"junzi_ov9650",0	},
   	{},
};
ssize_t ov9650_read(struct file *file, char __user *buf, size_t count, loff_t *off)
{
   return 0;
}

ssize_t ov9650_write(struct file *file, const char __user *buf, size_t count, loff_t *off)
{
   return 0;
}
static struct file_operations ov9650_fops={
   .owner = THIS_MODULE,
   .read  = ov9650_read,
   .write = ov9650_write,
};
static int ov9650_probe(struct i2c_client *client, const struct i2c_device_id *id_table)
{
   int data=0;

   printk("i2c probe:%s\n",client->name);

   ov9650_client = client;

   mdelay(1000);

   data = i2c_smbus_read_byte_data(ov9650_client,0x0A);  //返回值为负值说明错误
   data <<= 8;
   data |= i2c_smbus_read_byte_data(ov9650_client,0x0B);
   if(data<0)
   { 
     printk("i2c_smbus_read_byte_data failed\n");
   	 return -1;
   }
   printk("read the data is:%04x\n",data);
   
   major = register_chrdev(0,"junzi_dev",&ov9650_fops);
   
   my_class = class_create(THIS_MODULE,"junzi_class");
   device_create(my_class,NULL,MKDEV(major,0),0,"junzi_ov9650");
   
   return 0; //返回值?
}
int ov9650_remove(struct i2c_client *client)
{
   printk("i2c remove:%s\n",client->name);
   
   device_destroy(my_class,MKDEV(major,0));
   class_destroy(my_class);
   unregister_chrdev(major,"junzi_dev");

   return 0;
}
static struct i2c_driver my_i2cdrv={
   .driver ={
   	   .name = "junzi_ov9650_drv",
	   .owner = THIS_MODULE,
   	},
   .probe  = ov9650_probe,
   .remove = ov9650_remove,
   .id_table = ov9650_idtable,
   
};

static int ov9650_drv_init(void)
{
   i2c_add_driver(&my_i2cdrv);
   return 0;

}
static int ov9650_drv_exit(void)
{
   i2c_del_driver(&my_i2cdrv);
}

module_init(ov9650_drv_init);
module_exit(ov9650_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");





