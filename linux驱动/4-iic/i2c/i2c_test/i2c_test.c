
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void print_usage()
{
   printf("write usage: ./i2c_test w addr data \n");   
   printf("read usage: ./i2c_test r addr \n");
}
int main(int argc,char **argv)
{
   int fd;
   unsigned char buf[3];

   fd=open("/dev/junzi_i2cdev",O_RDWR);
   if(fd<0)
   {
      printf("can't open dev file\n");
	  return -1;
   }
   
   if((argc!=3)&&(argc!=4))  
   {	
      print_usage();
	  return -1;
   }
   //�Ƚ��Ƕ�����д
   if(strcmp(argv[1],"w")==0)  //д
   {
      buf[0] = strtoul(argv[2],0,0);  //��ַ
	  buf[1] = strtoul(argv[3],0,0);  //����
	  if(write(fd,buf,2)<0)
	  	printf("write error\n");
   }
   else if(strcmp(argv[1],"r")==0)  //��
   {      
      buf[0] = strtoul(argv[2],0,0);  //��ַ
      if(read(fd,buf,1)<0)
		  printf("read error\n");
	  else
	      printf("has read data :%d from %d\n",buf[1],buf[0]);
   }
   else
     print_usage();
   close(fd);
   return 0;
}


























