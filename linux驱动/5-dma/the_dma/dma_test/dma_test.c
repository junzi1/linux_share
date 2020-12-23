
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

/* ./dma_test nodma
 * ./dma_test dma
 */
#define MEM_NO_DMA 0
#define MEM_DMA    1

int main(int argc, char **argv)
{
	int fd;
	

	fd = open("/dev/junzi_dmadev", O_RDWR);
	if (fd < 0)
	{
		printf("can't open /dev/junzi_dmadev\n");
		return -1;
	}

	if (strcmp(argv[1], "nodma") == 0)
	{
		
		while(1)
	   {
		   ioctl(fd, MEM_NO_DMA);
	   }
	}
	else if (strcmp(argv[1], "dma") == 0)
	{
	    while(1)
	    {
	     ioctl(fd, MEM_DMA);
	    }
	}
	return 0;
	
	close(fd);
}







