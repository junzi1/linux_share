
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

static int major;
static struct class *my_class;
static struct i2c_client *my_client;
static struct i2c_device_id my_i2c_idtable[]={
   	{"junzi_i2c",0	},
   	{},
};
/*
  buf[0] addr
  buf[1] data
*/
ssize_t my_read(struct file *file, char __user *buf, size_t count, loff_t *off)
{
   unsigned char addr,data;
   
   copy_from_user(&addr,buf,1);
   data=i2c_smbus_read_byte_data(my_client,addr);  //返回值为负值说明错误
   if(data<0)
   	 return -1;

   copy_to_user(&buf[1],&data,1);

   return 1;
}
/*
  buf[0] addr
  buf[1] data
*/
ssize_t my_write(struct file *file, const char __user *buf, size_t count, loff_t *off)
{
   unsigned char addr,data;
   int ret;
   
   copy_from_user(&addr,buf,1);

   copy_from_user(&data,&buf[1],1);

   ret=i2c_smbus_write_byte_data(my_client,addr,data); //返回值为0说明成功
   if(ret)
   	 return -1;
   return 1;
}
static struct file_operations my_ops={
   .owner = THIS_MODULE,
   .read  = my_read,
   .write = my_write,
};
static int my_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id_table)
{
   printk("i2c probe:%s\n",client->name);

   my_client = client;
   major = register_chrdev(0,"junzi_dev",&my_ops);
   
   my_class = class_create(THIS_MODULE,"junzi_class");
   device_create(my_class,NULL,MKDEV(major,0),0,"junzi_i2cdev");
   
   return 0; //返回值?
}
int my_i2c_remove(struct i2c_client *client)
{
   printk("i2c remove:%s\n",client->name);
   
   device_destroy(my_class,MKDEV(major,0));
   class_destroy(my_class);
   unregister_chrdev(major,"junzi_dev");

   return 0;
}
static struct i2c_driver my_i2cdrv={
   .driver ={
   	   .name = "junzi_i2cdrv",
	   .owner = THIS_MODULE,
   	},
   .probe  = my_i2c_probe,
   .remove = my_i2c_remove,
   .id_table = my_i2c_idtable,
   
};

static int my_i2cdrv_init(void)
{
   i2c_add_driver(&my_i2cdrv);
   return 0;
}
static int my_i2cdrv_exit(void)
{
   i2c_del_driver(&my_i2cdrv);
}

module_init(my_i2cdrv_init);
module_exit(my_i2cdrv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");





