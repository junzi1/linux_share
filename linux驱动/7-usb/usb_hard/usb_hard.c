#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

static int my_maxpack;
static char *my_buffer;
dma_addr_t my_dma;
static struct urb *my_urb;
static struct input_dev *my_input_dev;

static struct usb_device_id my_usb_id [] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_MOUSE) },
	{ }	/* Terminating entry */
};
static void my_usb_complete_fn(struct urb *urb)
{
   /*switch(my_buffer[0])
   {
       case 1:
	   	      input_event(my_input_dev,EV_KEY,KEY_L,1);
			  input_sync(my_input_dev);
	   	      break;
	   case 2:
	   	      input_event(my_input_dev,EV_KEY,KEY_S,1);
			  input_sync(my_input_dev);
	   	      break;
	   case 4:
	   	      input_event(my_input_dev,EV_KEY,KEY_ENTER,1);
			  input_sync(my_input_dev);
	   	      break;
	   default:
	   	      break;
   }*/
   switch(my_buffer[0])
   {
       case 1:
	   	      printk("l");
	   	      break;
	   case 2:
	   	      printk("s");
	   	      break;
	   case 4:
	   	      printk("\n");
	   	      break;
	   default:
	   	      break;
   }
   usb_submit_urb(my_urb,GFP_KERNEL);
}
static int my_usb_probe(struct usb_interface *interface,const struct usb_device_id *id)
{
   struct usb_device *my_dev=interface_to_usbdev(interface); //获取当前设备
   struct usb_host_interface *cur_interface=interface->cur_altsetting;  //获取当前接口设置
   struct usb_endpoint_descriptor *my_endpoint=&cur_interface->endpoint[0].desc;  //获取当前端点描述符
   int my_pipe;

   my_pipe    =usb_rcvintpipe(my_dev,my_endpoint->bEndpointAddress); //源地址

   my_maxpack =my_endpoint->wMaxPacketSize;     //端点大小

   my_buffer  =usb_buffer_alloc(my_dev,my_maxpack,GFP_ATOMIC,&my_dma); //目的地址

   /*--------------urb分配--初始化-----------设置 ---------------------*/

   my_urb=usb_alloc_urb(0,GFP_KERNEL);   //分配

   //初始化中断函数
   usb_fill_int_urb(my_urb,my_dev,my_pipe,my_buffer,my_maxpack,my_usb_complete_fn,NULL,my_endpoint->bInterval);

   //设置地址和标志
   my_urb->transfer_dma=my_dma;
   my_urb->transfer_flags=URB_NO_TRANSFER_DMA_MAP;
   
   /*----------------------输入子系统分配设置---------------------------*/

   my_input_dev=input_allocate_device(); //分配

   set_bit(EV_KEY,my_input_dev->evbit);   //设置按键类事件
   //set_bit(EV_REP,my_input_dev->evbit);

   set_bit(KEY_L,my_input_dev->keybit);  //设置按键
   set_bit(KEY_S,my_input_dev->keybit);
   set_bit(KEY_ENTER,my_input_dev->keybit);

   my_input_dev->name="junzi_key";

   input_register_device(my_input_dev); //注册
   /*----------------------提交urb--------------------------------------*/

   usb_submit_urb(my_urb,GFP_KERNEL);
   
}
static void my_usb_disconnect(struct usb_interface *intf)
{
   struct usb_device *my_dev=interface_to_usbdev(intf); //获取当前设备
   usb_kill_urb(my_urb);
   usb_free_urb(0);
   usb_buffer_free(my_dev,my_maxpack,my_buffer,my_dma);

   input_unregister_device(my_input_dev);
   input_free_device(my_input_dev);
}

static struct usb_driver my_usb_drv={
   .name      ="junzi_usb",
   .probe     =my_usb_probe,
   .disconnect=my_usb_disconnect,
   .id_table  =my_usb_id
};
static int my_usb_init()
{
   usb_register(&my_usb_drv);
   return 0;
}
static void my_usb_exit()
{
   usb_deregister(&my_usb_drv);
}

module_init(my_usb_init);
module_exit(my_usb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");

























