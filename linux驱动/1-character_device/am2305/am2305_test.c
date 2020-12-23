
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
   int fd;
   char buf[10];
   char ch;
   unsigned int hum,temp;
   unsigned int temp_big,temp_little;
   unsigned int hum_big,hum_little;
   
   fd = open("/dev/junzi_am2305",O_RDWR|O_NONBLOCK);
   if(fd<0)
   {
      printf("open /dev/junzi_am2305 failed\n");
	  return -1;
   }
   printf("Press 'n' to dispaly the present temperature and hum\n");   
   printf("Press 'q' to leave application\n");
   /*
      第一次读的数据不准确
   */
   sleep(2);
   while(1)
   {
      read(fd,buf,5);
	  hum = buf[0]*256+buf[1];
	  temp = buf[2]*256+buf[3];

	  temp_big = temp/10;
	  temp_little = temp%10;

	  hum_big = hum/10;
	  hum_little = hum%10;
	  do
	  {	     
		 ch = getchar();
	  }while(ch != 'n' && ch != 'q' && ch!='t');
	  if(ch == 'q')
	  	break;
	  if(ch == 'n')
	    printf("Now the temperature is %d.%d℃\nhum is %d.%d%\n",temp_big,temp_little,hum_big,hum_little);
	  sleep(1);
   }

   close(fd);
}




