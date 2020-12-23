
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

static struct usb_device_id my_usb_id [] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID, USB_INTERFACE_SUBCLASS_BOOT,
		USB_INTERFACE_PROTOCOL_MOUSE) },
	{ }	/* Terminating entry */
};

int my_usb_probe(struct usb_interface *intf,const struct usb_device_id *id)
{
    printk("found junzi usb mouse!\n");
	return 0;
}
void my_usb_disconnect (struct usb_interface *intf)
{
    printk("junzi usb mouse disconnect\n");
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



















