#include <stdio.h>


#define time_after(unknown,known) ((signed char)((known)-(unknown))<0)



unsigned char known_time = 5;

int main()
{
	if(time_after(255,254))
		printf("255 > 254\n");

	int i;
again:
	for(i = 0;i <= 255; i++) {
		//if(i-known_time > 127)
			//continue;
		if(time_after(i,known_time))
			printf("+++++ %d > %d   %d > %d\n",(signed char)i,(signed char)known_time,i,known_time);
		else
			printf("%d < %d  %d < %d\n",(signed char)i,(signed char)known_time,i,known_time);
	}
	if(known_time == 253)
		return 0;

	known_time = 253;
	printf("##############################\n\n\n");
	goto again;

	//return 0;
}






