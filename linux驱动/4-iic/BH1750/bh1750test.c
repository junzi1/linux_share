
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
int main()
{
   int fd;
   char buf[10];
   char ch;
   unsigned int low,high;
   
   fd = open("/dev/bh1750",O_RDWR);
   if(fd<0)
   {
      printf("open /dev/bh1750 failed\n");
	  return -1;

   }
   printf("fd value %d\n",fd);
   printf("Press 'n' to dispaly the present temperature and hum\n");   
   printf("Press 'q' to leave application\n");
   
   while(1)
   {
      
	  read(fd,buf,2);
	  high = buf[0];
	  low = buf[1];
	  do
	  {	     
		 ch = getchar();
	  }while(ch != 'n' && ch != 'q');
	  if(ch == 'q')
	  	break;
	  if(ch == 'n')
	    printf("Now the Light intensity is high %d low %d\n",high,low);
	  sleep(1);
   }

   close(fd);
}

