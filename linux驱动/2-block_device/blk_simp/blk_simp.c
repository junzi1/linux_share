
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

/*注意set_capacity(my_disk,1024*1024*2/512);第二个参数是扇区数 不是字节*/

static DEFINE_SPINLOCK(my_lock);
static struct gendisk *my_disk;
static struct request_queue *my_queue;
static int my_major;
static char *my_diskbuffer;

static void my_do_queue(struct request_queue *q)
{
   struct request *req;
   while((req=blk_fetch_request(q))!=NULL)
   {
      switch(rq_data_dir(req))
      {
         case READ:
		 	       memcpy(req->buffer,my_diskbuffer+(blk_rq_pos(req)<<9),blk_rq_bytes(req));
				   blk_end_request_all(req,0);
		 	       break;
		 case WRITE:
		 	       memcpy(my_diskbuffer+(blk_rq_pos(req)<<9),req->buffer,blk_rq_bytes(req));
				   blk_end_request_all(req,0);
                   break;
		 default:
        		   blk_end_request_all(req,1);
                   break;
      }
   }
}

struct block_device_operations my_ops={
   .owner=THIS_MODULE,   	
};
static int my_blk_init(void)
{
 /*-----------genddisk结构体设置-----------------------*/
   my_disk =alloc_disk(16);

   my_major=register_blkdev(0,"junzi_blkdev");//获取主设备号
   my_queue=blk_init_queue(my_do_queue,&my_lock); //初始化请求队列
   
   my_disk->major      =my_major;         //成员设置
   my_disk->first_minor=0;
   my_disk->minors     =16;
   strcpy(my_disk->disk_name,"junzi_blkdev");
   my_disk->fops       =&my_ops;
   my_disk->queue      =my_queue;

   set_capacity(my_disk,2*1024*1024/512);  //设置容量
   my_diskbuffer=kzalloc(2*1024*1024,GFP_KERNEL);
 /*--------------注册gendisk结构体---------------------*/

   add_disk(my_disk);
   return 0;
}
static void my_blk_exit(void)
{
   del_gendisk(my_disk);
   put_disk(my_disk);

   blk_cleanup_queue(my_queue);
   unregister_blkdev(my_major,"junzi_blkdev");

   kfree(my_diskbuffer);
}

module_init(my_blk_init);
module_exit(my_blk_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("junzi");

















