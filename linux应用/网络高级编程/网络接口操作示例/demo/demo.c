#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "tools.h"

#define calc_ms(tm) (tm.tv_sec*1000+tm.tv_nsec/1000000)
static void get_current_clkms(unsigned long *time)
{
    struct timespec tm;
    int ret = clock_gettime(CLOCK_MONOTONIC,&tm);
    if(ret == 0) {
        *time = calc_ms(tm);
    }
}


