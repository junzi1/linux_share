
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
   int fd;
   char buf[20];
   char ch;
   
   fd = open("/dev/junzi_ds18b20",O_RDWR);
   if(fd<0)
   {
      printf("open /dev/junzi_ds18b20 failed\n");
	  return -1;
   }

   printf("Press 'n' to dispaly the present temperature\n");   
   printf("Press 'q' to leave application\n");
   while(1)
   {
      read(fd,buf,20);
	  do
	  {	     
		 ch = getchar();
	  }while(ch != 'n' && ch != 'q');
	  if(ch == 'q')
	  	break;
	  printf("Now the temperature is %s\n",buf);
	  sleep(1);
   }

   close(fd);
}




