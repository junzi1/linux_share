
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>

#include <asm/io.h>
#include <asm/div64.h>

#include<linux/timer.h>

#include <asm/mach/map.h>
#include <linux/input.h>
#include <mach/regs-gpio.h>
//#include <asm/arch/regs-adc.h>

struct input_dev *my_ts;
struct clk *clk;
static unsigned long xp=0;
static unsigned long yp=0;
static int count=0;

struct myts_regs{
    unsigned long adccon;
	unsigned long adctsc;
	unsigned long adcdly;
	unsigned long adcdat0;
	unsigned long adcdat1;
};
static volatile struct myts_regs *myts_regs;

static void myts_start_adc(void)
{
   myts_regs->adccon |= (1<<0);
}
static void myts_wait_down(void)
{
   myts_regs->adctsc =0xd3;
}
static void myts_wait_up(void)
{
   myts_regs->adctsc =0x1d3;
}
static void myts_auto_change(void)
{
   myts_regs->adctsc =0x0c;
}
static void my_timer_func(unsigned long data)
{
   if(!(myts_regs->adcdat0 & (1<<15))) //if down
   {
       myts_auto_change();
	   myts_start_adc();
   }
   else
   {       
	   myts_wait_down();
   }
}
struct timer_list my_timer=TIMER_INITIALIZER(my_timer_func,0,0);

static irqreturn_t myts_ts_irq(int irq,void *dev_id)
{
   if(!(myts_regs->adcdat0 & (1<<15))) //if down
   {
       //printk("down\n");	
	   myts_auto_change();
	   myts_start_adc();
   }
   else
   {
       //printk("up\n");
       input_report_abs(my_ts,ABS_PRESSURE,0); 
	   input_report_key(my_ts, BTN_TOUCH, 0);
	   input_sync(my_ts);
	   myts_wait_down();
   }
   return IRQ_HANDLED;
}
static irqreturn_t myts_adc_irq(int irq,void *dev_id)
{
   if(!(myts_regs->adcdat0 & (1<<15)))  //if down
   {
      if(count==4)
      {
          //printk("xp:%d, yp%d\n",xp/4,yp/4);
          input_report_abs(my_ts,ABS_X,xp/4);
	      input_report_abs(my_ts,ABS_Y,yp/4);
	      input_report_abs(my_ts,ABS_PRESSURE,1); 
		  input_report_key(my_ts, BTN_TOUCH, 1);
		  input_sync(my_ts);
          count=0;
		  xp=yp=0;

		  mod_timer(&my_timer,HZ/100);
		  myts_wait_up();
      }
	  else
	  {
          xp += (myts_regs->adcdat0 & 0x3ff);
          yp += (myts_regs->adcdat1 & 0x3ff);
		  count++;
		  
		  myts_auto_change();
		  myts_start_adc();
	  }
   }
   else
   {   
       input_report_abs(my_ts,ABS_PRESSURE,0); 
	   input_report_key(my_ts, BTN_TOUCH, 0);
	   input_sync(my_ts);   
	   myts_wait_down();
   }
   return IRQ_HANDLED;
   
}
static int myts_init(void)
{
/*---------------input_device_register---------------------------*/
   my_ts=input_allocate_device();
   
   set_bit(EV_KEY,my_ts->evbit);
   set_bit(EV_ABS,my_ts->evbit);

   set_bit(BTN_TOUCH,my_ts->keybit);

   
   input_set_abs_params(my_ts, ABS_X, 0, 0x3FF, 0, 0);
   input_set_abs_params(my_ts, ABS_Y, 0, 0x3FF, 0, 0);
   input_set_abs_params(my_ts, ABS_PRESSURE, 0, 1, 0, 0);

   input_register_device(my_ts);
/*---------------get_adc_clock-----------------------------------*/
   clk=clk_get(NULL,"adc");
   clk_enable(clk);
/*---------------set_regs_and_irq--------------------------------*/
   myts_regs=ioremap(0x58000000,sizeof(struct myts_regs));

   myts_regs->adccon  = (1<<14) | (49<<6);
   myts_regs->adcdly  =0xffff;
   request_irq(IRQ_TC, myts_ts_irq, IRQF_SAMPLE_RANDOM, "ts_pen", NULL);
   request_irq(IRQ_ADC, myts_adc_irq, IRQF_SAMPLE_RANDOM, "adc", NULL);
/*---------------------------------------------------------------*/
   add_timer(&my_timer);
   myts_wait_down();
   
   return 0;
}
static void myts_exit(void)
{
   del_timer(&my_timer);
   free_irq(IRQ_TC,NULL);   
   free_irq(IRQ_ADC,NULL);
   iounmap(myts_regs);
   clk_disable(clk);
   clk_put(clk);   
   input_unregister_device(my_ts);
   input_free_device(my_ts);
}

module_init(myts_init);
module_exit(myts_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");


























