
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <linux/mtd/partitions.h>

#include <asm/io.h>

#include <plat/regs-nand.h>
#include <plat/nand.h>

static struct nand_chip *my_nand;
static struct mtd_info *my_mtd;

struct my_nand_regs{
    unsigned long nfconf  ;
	unsigned long nfcont  ;
	unsigned long nfcmd   ;
	unsigned long nfaddr  ;
	unsigned long nfdata  ;
	unsigned long nfeccd0 ;
	unsigned long nfeccd1 ;
	unsigned long nfeccd  ;
	unsigned long nfstat  ;
	unsigned long nfestat0;
	unsigned long nfestat1;
	unsigned long nfmecc0 ;
	unsigned long nfmecc1 ;
	unsigned long nfsecc  ;
	unsigned long nfsblk  ;
	unsigned long nfeblk  ;  
};
struct my_nand_regs *my_nand_regs;

static struct mtd_partition my_nand_part[] = {
	[0] = {
		.name	= "my_boot",
		.size	= SZ_1M,
		.offset	= 0,
	},
	[1] = {
		.name	= "my_kernel",
		.offset = SZ_1M+SZ_128K,
		.size	= SZ_4M,
	},
	[2] = {
		.name	= "my_yaffs2",
		.offset = MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static void my_select_chip(struct mtd_info *mtd, int chip)
{
   if(chip==-1)  //没有选中
   {
      my_nand_regs->nfcont|= (1<<1);
   }
   else
   {
      my_nand_regs->nfcont&= (~(1<<1));
   }
}
static void my_cmd_strl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
   if(ctrl&NAND_CLE)   //如果是命令
   {
      my_nand_regs->nfcmd= dat;
   }
   else
   {
      my_nand_regs->nfaddr= dat;
   }
}
static int my_dev_ready(struct mtd_info *mtd)
{
   return (my_nand_regs->nfstat& 1);
}

static int my_nand_init(void)
{
   struct clk *my_clk;
   my_clk=clk_get(NULL,"nand");
   clk_enable(my_clk);
/*-----------分配struct nand_chip和struct mtd_info----------*/

   my_nand = kzalloc(sizeof(struct nand_chip),GFP_KERNEL);
   my_mtd  = kzalloc(sizeof(struct mtd_info),GFP_KERNEL);

/*------------填充struct nand_chip结构体--------------------*/ 

   my_nand_regs          =ioremap(0x4E000000,sizeof(struct my_nand_regs));
  
   my_nand->select_chip  =my_select_chip;
   my_nand->cmd_ctrl     =my_cmd_strl;
   my_nand->IO_ADDR_R    =&my_nand_regs->nfdata;
   my_nand->IO_ADDR_W    =&my_nand_regs->nfdata;
   my_nand->dev_ready    =my_dev_ready;
   my_nand->ecc.mode     =NAND_ECC_SOFT;

/*------------填充struct mtd_info结构体---------------------*/ 

   my_mtd->priv  =my_nand;
   my_mtd->owner =THIS_MODULE;
/*-----------------硬件设置---------------------------------*/
   
#define TACLS    0
#define TWRPH0   1
#define TWRPH1   0
	my_nand_regs->nfconf = (TACLS<<12) | (TWRPH0<<8) | (TWRPH1<<4);

	/* NFCONT: 
	 * BIT1-设为1, 取消片选 
	 * BIT0-设为1, 使能NAND FLASH控制器
	 */
	my_nand_regs->nfcont = (1<<1) | (1<<0);

/*---------------------------------------------------------*/

   nand_scan(my_mtd,1);

   add_mtd_partitions(my_mtd,my_nand_part,3);
   return 0;
}
static void my_nand_exit(void)
{
   del_mtd_partitions(my_mtd);
   iounmap(my_nand_regs);
   kfree(my_nand);
   kfree(my_mtd);
}

module_init(my_nand_init);
module_exit(my_nand_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");

























