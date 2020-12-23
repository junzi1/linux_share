
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>


static struct i2c_client *my_client; 
static struct i2c_adapter *my_adapter;
static struct i2c_board_info my_devinfo={
   I2C_BOARD_INFO("bh1750",0x23),
};

static unsigned short my_addrlist[]={0x23,I2C_CLIENT_END};

static int my_i2cdev_init(void)
{
   my_adapter=i2c_get_adapter(0);    /*获取这个i2c适配器，检测设备确实存在之后还需要释放他，
                                       在这个函数中用锁占住了，需要释放 切记
                                     */
   my_client=i2c_new_probed_device(my_adapter,&my_devinfo,my_addrlist); 

   if(!my_client)
   {
      goto my_out;
   }
   i2c_put_adapter(my_adapter);
   return 0;
   
my_out:
   i2c_put_adapter(my_adapter);
   return -ENODEV;
}
static void my_i2cdev_exit(void)
{
   i2c_release_client(my_client);
}

module_init(my_i2cdev_init);
module_exit(my_i2cdev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");
























