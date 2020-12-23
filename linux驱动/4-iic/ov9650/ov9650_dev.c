#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>

static struct i2c_client *ov9650_client; 
static struct i2c_adapter *adapter;
/*
  0x60 for write (7-bit address and 1-bit write)
  0x61 for read (7-bit address and 1-bit read)
  0110000 = 0x30
*/
static struct i2c_board_info ov9650_devinfo={
   I2C_BOARD_INFO("junzi_ov9650",0x30),
};

static unsigned short addrlist[]={0x30,I2C_CLIENT_END};

static int ov9650_device_init(void)
{
   adapter=i2c_get_adapter(0);    /*获取这个i2c适配器，检测设备确实存在之后还需要释放他，
                                       在这个函数中用锁占住了，需要释放 切记
                                     */
   ov9650_client=i2c_new_probed_device(adapter,&ov9650_devinfo,addrlist); 

   if(!ov9650_client)
   {
      printk("no such device:ov9650\n");
      goto my_out;
   }
   i2c_put_adapter(adapter);
   return 0;
   
my_out:
   i2c_put_adapter(adapter);
   return -ENODEV;
}
static void ov9650_device_exit(void)
{
   i2c_release_client(ov9650_client);
}

module_init(ov9650_device_init);
module_exit(ov9650_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");


