
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

#include <asm/mach/map.h>
#include <mach/regs-lcd.h>
#include <mach/regs-gpio.h>
#include <mach/fb.h>

struct fb_info *mylcd_info;

static volatile unsigned long *gpccon;
static volatile unsigned long *gpdcon;
static volatile unsigned long *gpgcon;
static volatile unsigned long *gpgup;
static u32 my_palette[16];
struct mylcd_regs{
     unsigned long lcdcon1;
	 unsigned long lcdcon2;
	 unsigned long lcdcon3;
	 unsigned long lcdcon4;
	 unsigned long lcdcon5;
	 unsigned long lcdsaddr1;
	 unsigned long lcdsaddr2;
	 unsigned long lcdsaddr3;
	 unsigned long redlut;
	 unsigned long greenlut;
	 unsigned long bluelut;
	 unsigned long the_tmp[9];
	 unsigned long dithmode;
	 unsigned long tpal;
	 unsigned long lcdintpnd;
	 unsigned long lcdsrcpnd;
	 unsigned long lcdintmsk;
	 unsigned long tconsel;
};
static volatile struct mylcd_regs *mylcd_regs;
static inline unsigned int chan_to_field(unsigned int chan,
					 struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}
static int mylcd_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
   
     unsigned int val;
	 if(regno>16)
	 	return 1;
	 val  = chan_to_field(red,   &info->var.red);
	 val |= chan_to_field(green, &info->var.green);
     val |= chan_to_field(blue,  &info->var.blue);

	my_palette[regno] = val;
	return 0;
}


static struct fb_ops mylcd_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= mylcd_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};

static int mylcd_init(void)
{
   mylcd_info=framebuffer_alloc(0,NULL);

   mylcd_info->pseudo_palette=my_palette;
/*------------fb_fix_screeninfo----------------------------*/
   strcpy(mylcd_info->fix.id,"jun_lcd");
   //mylcd_info->fix.smem_start=;
   mylcd_info->fix.smem_len      =480*272*16/2;
   mylcd_info->fix.type          =FB_TYPE_PACKED_PIXELS;
   mylcd_info->fix.visual        =FB_VISUAL_TRUECOLOR;
   mylcd_info->fix.line_length   =480*16/2;
   //mylcd_info->fix.mmio_len=;
/*----------- fb_var_screeninfo---------------- -----------*/
   mylcd_info->var.xres          =480;
   mylcd_info->var.yres          =272;
   mylcd_info->var.bits_per_pixel=16;
   //mylcd_info->var.grayscale   =;

   mylcd_info->var.red.length   =5;
   mylcd_info->var.red.offset   =11;
   
   mylcd_info->var.green.length =6;
   mylcd_info->var.green.offset =5;

   mylcd_info->var.blue.length  =5;
   mylcd_info->var.blue.offset  =0;

   mylcd_info->var.nonstd        =0;
   mylcd_info->var.activate      =FB_ACTIVATE_NOW;
  
  /*----------- ---------------- -------------------------*/
   mylcd_info->fbops=&mylcd_ops;        //fbops
   //mylcd_info->screen_base=;
   //mylcd_info->screen_size=;
 
 /*-----------set_gpio----------------------- -----------*/
   //我猜测tx2440背光电路已经由GPGDAT4主动给予高电平,所以GPGCON4禁止上拉
   gpccon   =ioremap(0x56000020,4);
   gpdcon   =ioremap(0x56000030,4);
   gpgup    =ioremap(0x56000068,4);
   gpgcon   =ioremap(0x56000060,4);
   
   *gpccon  =0xaaaaaaaa;
   *gpdcon  =0xaaaaaaaa;
   *gpgup   &=~(1<<4)|(1<<4);  //disable up
   *gpgcon  |=(3<<8);
/*-----------set_lcd_controllers--------------------------*/
   mylcd_regs=ioremap(0X4D000000,sizeof(struct mylcd_regs));
   /*
     LCDCON1:
   */
   mylcd_regs->lcdcon1   = (4<<8) | (3<<5) | (0xc<<1) | (0<<0);
   /*
     LCDCON2:
   */
   mylcd_regs->lcdcon2   = (2<<24) | (271<<14) | (4<<6) | (8<<0);
   /*
     LCDCON3:
   */
   mylcd_regs->lcdcon3   = (10<<19) | (479<<8) | (19<<0);
   /*
     LCDCON4:
   */
   mylcd_regs->lcdcon4   = (29<<0);
   /*
     LCDCON5:
   */
   mylcd_regs->lcdcon5   = (1<<11) | (0<<10) | (1<<9) | (1<<8) | (0<<3) | (1<<0);

   /*
     LCDSADDRn:
   */
   mylcd_info->screen_base=dma_alloc_writecombine(NULL,mylcd_info->fix.smem_len,&mylcd_info->fix.smem_start,GFP_KERNEL);
   //在这里,视口和帧内存的地址是相同的
   mylcd_regs->lcdsaddr1 =(mylcd_info->fix.smem_start>>1)&(~(3<<30));
   mylcd_regs->lcdsaddr2 =((mylcd_info->fix.smem_start+mylcd_info->fix.smem_len)>>1)&0x1fffff;
   mylcd_regs->lcdsaddr3 =480*16/16;
   /*
     enable lcd
   */
   mylcd_regs->lcdcon1   |=(1<<0);
   mylcd_regs->lcdcon5   |=(1<<3);
/*-----------register_framebuffer--------------------------*/
   register_framebuffer(mylcd_info);
 
   return 0;
}
static void mylcd_exit(void)
{
  unregister_framebuffer(mylcd_info);
  mylcd_regs->lcdcon1	&=(~(1<<0));
  dma_free_writecombine(NULL,mylcd_info->fix.smem_len,&mylcd_info->fix.smem_start,GFP_KERNEL);
  iounmap(mylcd_regs);
  iounmap(gpccon);
  iounmap(gpdcon);
  iounmap(gpgup);
  iounmap(gpgcon);
  framebuffer_release(mylcd_info);
}

module_init(mylcd_init);
module_exit(mylcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");

























