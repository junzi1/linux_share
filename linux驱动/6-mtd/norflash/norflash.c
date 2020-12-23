
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/mtd/concat.h>
#include <linux/io.h>

static struct map_info *my_map;  //硬件信息设置
static struct mtd_info *my_mtd;  //通用的mtd结构体  会由 do_map_probe函数返回

static struct mtd_partition my_mtd_parts[] = {
	[0] = {
		.name	= "my_first",
		.size	= SZ_1M,
		.offset	= 0,
	},
	[1] = {
		.name	= "my_second",
		.offset = MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};


static int my_nor_init(void)
{
   my_map=kzalloc(sizeof(struct map_info),GFP_KERNEL);

 /*-----------struct map_info的设置及初始化------------------*/

   my_map->name       = "junzi_norflash";
   my_map->size       = 0x1000000;  //大小16m
   my_map->phys       = 0;          //norflash物理地址为0
   my_map->bankwidth  = 2;          //相当于16位
   my_map->virt       = ioremap( my_map->phys,my_map->size);

   simple_map_init(my_map);

/*-----------扫描硬件信息------------------------------------*/

   my_mtd = do_map_probe("cfi_probe",my_map);

   if(!my_mtd)  //如果cfi_probe失败 则调用jedec_probe
   {
      my_mtd = do_map_probe("jedec_probe",my_map);
	  if(!my_mtd)   //如果jedec_probe失败，则返回
	  {
	     printk("failed to probe\n");
		 iounmap(my_map->virt);
	     kfree(my_map);
		 return -EIO; 
	  }
	  printk("use the jedec probe\n");
   }
   printk("use the cfi probe\n");

/*-----------添加分区----------------------------------------*/

   add_mtd_partitions(my_mtd,my_mtd_parts,2);

   return 0;
}
static void my_nor_exit(void)
{
	del_mtd_partitions(my_mtd);
	iounmap(my_map->virt);
	kfree(my_map);
}

module_init(my_nor_init);
module_exit(my_nor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");



















