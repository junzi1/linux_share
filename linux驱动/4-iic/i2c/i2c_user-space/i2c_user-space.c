
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "i2c-dev.h"

static void print_usage()
{
   printf("write usage: ./i2c_test w addr data \n");   
   printf("read usage: ./i2c_test r addr \n");
}

int main(int argc,char **argv)
{
   int fd;
   unsigned char addr,data;

   if((argc!=3)&&(argc!=4))  //判断参数个数
   {	
      print_usage();
	  return -1;
   }
   
   fd=open("/dev/i2c-0",O_RDWR);  //打开设备文件
   if(fd<0)
   {
      printf("can't open the dev file\n");
	  return -1;
   }
   
   addr = strtoul(argv[2],0,10);
   
   if (ioctl(fd, I2C_SLAVE, 0x50) < 0)  //连接i2c设备的地址
   {
      printf("i2c transfer error\n");
	  close(fd);
	  return -1;
   }

   if(strcmp(argv[1],"w")==0)  //写
   {      
	  data = strtoul(argv[3],0,10);
	  if(i2c_smbus_write_byte_data(fd,addr,data))
	  	printf("write error\n");
	  else
	  	printf("has write \n");
   }
   else if(strcmp(argv[1],"r")==0)  //读
   {      
      data = i2c_smbus_read_byte_data(fd,addr);
	  if((data<0))
	  	printf("read error\n");
	  else
	  	printf("has read %d from %d\n",data,addr);
   }
   else
     print_usage();

   close(fd);
   
   return 0;
}




















