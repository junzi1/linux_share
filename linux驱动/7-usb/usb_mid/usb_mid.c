
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

static int my_maxpack;
static char *my_buffer;
static dma_addr_t my_dma;
static struct urb *my_urb;

static struct usb_device_id my_usb_id [] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_MOUSE) },
	{ }	/* Terminating entry */
};
static void my_usb_complete_fn(struct urb *urb)
{
   static int count=0;
   int i;
   
   printk("count:%d",count++);
   for(i=0;i<my_maxpack;i++)
   {
      printk(" %02x",my_buffer[i]);
   }
   printk("\n");
   usb_submit_urb(my_urb,GFP_KERNEL);
}

int my_usb_probe(struct usb_interface *intf,const struct usb_device_id *id)
{
    //printk("found junzi usb mouse!\n");
    struct usb_device *my_dev=interface_to_usbdev(intf);            //获取当前usb设备
	struct usb_host_interface *cur_interface=intf->cur_altsetting;  //获取当前接口
	struct usb_endpoint_descriptor *my_endpoint= &cur_interface->endpoint[0].desc;; //获取当前端点
	int my_pipe;

	my_pipe   =usb_rcvintpipe(my_dev, my_endpoint->bEndpointAddress); //源

	my_maxpack=my_endpoint->wMaxPacketSize;                        //长度

	my_buffer =usb_buffer_alloc(my_dev,my_maxpack,GFP_ATOMIC,&my_dma);//目的
	
	/*------------------urb的设置------------------------------*/
	my_urb    =usb_alloc_urb(0,GFP_KERNEL);  //分配urb
	   /*
	      urb初始化
	   */
	usb_fill_int_urb(my_urb,my_dev,my_pipe,my_buffer,my_maxpack,my_usb_complete_fn,NULL,my_endpoint->bInterval);
	my_urb->transfer_dma=my_dma;
	my_urb->transfer_flags=URB_NO_TRANSFER_DMA_MAP;

	
	usb_submit_urb(my_urb,GFP_KERNEL);	  //提交urb
    return 0;
}
void my_usb_disconnect (struct usb_interface *intf)
{
    struct usb_device *my_dev=interface_to_usbdev(intf);  
    //printk("junzi usb mouse disconnect\n");
    usb_kill_urb(my_urb);
	usb_free_urb(my_urb);

	usb_buffer_free(my_dev,my_maxpack,my_buffer,my_dma);
	
}
static struct usb_driver my_usb={
  .name      ="junzi_usb",
  .probe     =my_usb_probe,
  .disconnect=my_usb_disconnect,
  .id_table  =my_usb_id
};
static int my_usb_init(void)
{
   usb_register(&my_usb);
   return 0;
}
static void my_usb_exit(void)
{
   usb_deregister(&my_usb);
}
module_init(my_usb_init);
module_exit(my_usb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");



















