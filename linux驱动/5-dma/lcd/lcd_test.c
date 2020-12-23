#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<linux/fb.h>
#include<sys/mman.h>
int main()
{
  int fd;
  struct fb_fix_screeninfo finfo;
  struct fb_var_screeninfo vinfo;
  char *base=0;
  long screensize=0;
  long location=0;
  int x,y; 
  fd=open("/dev/fb0",O_RDWR);
  if(fd<0)
  {
     printf("open failed\n");
     return -1;
  }
  if(ioctl(fd,FBIOGET_FSCREENINFO,&finfo))
  {
     printf("ioctl finfo error\n");
     return -2;
  }
  if(ioctl(fd,FBIOGET_VSCREENINFO,&vinfo))
  {
     printf("ioctl vinfo error\n");
     return -3;
  }
  screensize=vinfo.xres*vinfo.yres*vinfo.bits_per_pixel/8;
  base=(char*)mmap(0,screensize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
  if((int)base==-1)
  {
     printf("get addr error\n");
     return -4;
  }
  for(x=30;x<100;x++)
    for(y=30;y<100;y++)
  {
        if(y==30||x==30||y==99||x==99||x==60||y==60){
         location=x*2+y*finfo.line_length;
         *(base+location)=0x07; 
         *(base+location+1)=0xff; }
  }
   munmap(base,screensize);
   close(fd);
   return 0;
}
