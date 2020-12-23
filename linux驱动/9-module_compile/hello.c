#include <linux/module.h>

//echo 8 > /proc/sys/kernel/printk
//ubuntu dmesg -c 才能看到printk看不到的
static int __init hello_init(void)
{
	printk(KERN_DEBUG"~hello world\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_DEBUG"~bye world\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("LICENSE");
MODULE_AUTHOR("junzi");












