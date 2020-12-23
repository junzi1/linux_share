
#include <linux/major.h>
#include <linux/vmalloc.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/bitops.h>
#include <linux/hdreg.h>

#include <asm/setup.h>
//#include <asm/amigahw.h>
#include <asm/pgtable.h>

//#include <linux/zorro.h>

static struct gendisk *my_disk;
static struct request_queue *my_queue;
static int my_major;
static char *my_diskbuffer;

/*不要忘了这个头文件#include <linux/hdreg.h>要么my_getgeo会编译出错，原因是struct hd_geometry
  没有定义

  注意set_capacity(my_disk,1024*1024*2/512);第二个参数是扇区数 不是字节
*/

static int my_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
	geo->heads =4;
	geo->sectors = 32;
	geo->cylinders = 32;
	return 0;
}

static struct block_device_operations my_ops={
   .owner=THIS_MODULE,
   .getgeo=my_getgeo,
};
static int my_make_queue(struct request_queue *q,struct bio *bio)
{
   int i;           //段号
   char *my_base_addr;
   void *my_phya_addr;
   struct bio_vec *my_vec;
   
   my_base_addr=my_diskbuffer+(bio->bi_sector<<9); 

   bio_for_each_segment(my_vec,bio,i)
   {
      my_phya_addr=kmap(my_vec->bv_page)+my_vec->bv_offset;

	  switch(bio_rw(bio))
	  {
	      case READA:
          case READ:
		  	        memcpy(my_phya_addr,my_base_addr,my_vec->bv_len);
					kunmap(my_vec->bv_page);
		  	        break;
		  case WRITE:
		  	        memcpy(my_base_addr,my_phya_addr,my_vec->bv_len);
					kunmap(my_vec->bv_page);
		  	        break;
		  default:
		  	        bio_endio(bio,-EIO);
					return 0;
	  }
	  my_base_addr=my_base_addr+my_vec->bv_len;
   }
   bio_endio(bio,0);
   return 0;
}

static int my_blk_init(void)
{
   my_disk =alloc_disk(16);
 /*---------设置gendisk结构体---------------------*/

   my_major=register_blkdev(0,"junzi_blkdev");
   my_queue=blk_alloc_queue(GFP_KERNEL);
   blk_queue_make_request(my_queue,my_make_queue);

   my_disk->major      =my_major;
   my_disk->first_minor=0;
   my_disk->minors     =16;
   strcpy(my_disk->disk_name,"junzi_blkdev");
   my_disk->fops       =&my_ops;
   my_disk->queue      =my_queue;

   set_capacity(my_disk,1024*1024*2/512);
   my_diskbuffer       =kzalloc(1024*1024*2,GFP_KERNEL);
   
 /*---------注册gendisk结构体---------------------*/

   add_disk(my_disk);
   return 0;
}
static void my_blk_exit(void)
{
   del_gendisk(my_disk);
   put_disk(my_disk);

   register_blkdev(my_major,"junzi_blkdev");
   kfree(my_diskbuffer);
} 

module_init(my_blk_init);
module_exit(my_blk_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");




























