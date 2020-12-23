#include <linux/module.h>
#include <linux/version.h>

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/irq.h>
#include<linux/jiffies.h>
#include<linux/timer.h>

#include <asm/gpio.h>
#include <asm/io.h>
#include <mach/regs-gpio.h>

static struct input_dev *my_key;

struct key_infos
{
   int irq;
   int pin;
   int pin_set;
   int key_val;
   char *name;
};
struct key_infos key_infos[]={
   	{.irq=IRQ_EINT4,.pin=S3C2410_GPF(4),.pin_set=S3C2410_GPF4_EINT4,.key_val=KEY_L,.name="keyl"},
   	{.irq=IRQ_EINT5,.pin=S3C2410_GPF(5),.pin_set=S3C2410_GPF5_EINT5,.key_val=KEY_S,.name="keys"},
   	{.irq=IRQ_EINT6,.pin=S3C2410_GPF(6),.pin_set=S3C2410_GPF6_EINT6,.key_val=KEY_ENTER,.name="keyenter"},
   	{.irq=IRQ_EINT7,.pin=S3C2410_GPF(7),.pin_set=S3C2410_GPF7_EINT7,.key_val=KEY_LEFTSHIFT,.name="keyleftsh"},
};
static struct key_infos *irq_info;
static void my_timer_func(unsigned long data)
{
    unsigned int push_val;
	struct key_infos *the_irq=irq_info;
	if(!the_irq)
		 return;
	push_val=s3c2410_gpio_getpin(the_irq->pin);
	if(push_val)
	{
       /* input_event(my_key,EV_KEY,the_irq->key_val,0);
		input_sync(my_key);*/
	}
	else
	{
		input_event(my_key,EV_KEY,the_irq->key_val,1);
		input_sync(my_key);
	}
	irq_info=NULL;
}
struct timer_list my_timer=TIMER_INITIALIZER(my_timer_func,0,0);
static irqreturn_t key_input_handleirq(int irq,void *dev_id)
{
    irq_info=(struct key_infos*)dev_id;
    mod_timer(&my_timer,jiffies+(HZ/100));
	return IRQ_RETVAL(IRQ_HANDLED);
}
static int key_input_init(void)
{
    int i;
	for(i=0;i<4;i++)
	{
	   s3c2410_gpio_cfgpin(key_infos[i].pin,key_infos[i].pin_set);
	   request_irq(key_infos[i].irq,key_input_handleirq,NULL,key_infos[i].name,(void*)(&key_infos[i]));
	   //enable_irq(key_infos[i].irq);
	}

	my_key=input_allocate_device();

	set_bit(EV_KEY,my_key->evbit);
    set_bit(EV_REP,my_key->evbit);

	set_bit(KEY_L,my_key->keybit);
	set_bit(KEY_S,my_key->keybit);
	set_bit(KEY_ENTER,my_key->keybit);
	set_bit(KEY_LEFTSHIFT,my_key->keybit);

    my_key->name = "junzi s3c2410";
	input_register_device(my_key);
	add_timer(&my_timer);
	printk("init over\n");
    return 0;
}
static void key_input_exit(void)
{
    int i;
    del_timer(&my_timer);
	input_unregister_device(my_key);
	input_free_device(my_key);
	for(i=0;i<4;i++)
	{
	   free_irq(key_infos[i].irq,&key_infos[i]);
	   disable_irq(key_infos[i].irq);
	}
}


module_init(key_input_init);
module_exit(key_input_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");























