#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>

#include "tools.h"

static int log_str_find_last_ch(const char *str,const int revr_index,const char ch)
{
    if(NULL == str) {
        return -1;
    }

    int len = strlen(str);
    if(revr_index >= len) {
        return -1;
    }

    int i = 0;
    for(i = (len-1-revr_index);i >= 0;i--) {
        if(str[i] == ch) {
            return i;
        }
    }

    return -2;
}

#define MAX_LOG_LENGTH 1024
int common_log_print(const char *flag_str, char *file, const char *func, unsigned int line, char *fmt, ...)
{
     char *pTmpFilename = NULL;
    // 优化默认文件名路径
    if(NULL == file) { 
        pTmpFilename = "Null";
    }else {
        int pos = 0;
        pTmpFilename = file;
        pos = log_str_find_last_ch(file,0,'/');
        if(pos < 0) {
            pos = log_str_find_last_ch(file,0,'\\');
            if(pos >= 0) {
                pTmpFilename = file + pos + 1;
            }
        }else {
            pTmpFilename = file + pos + 1;
        }
    }
    
    va_list alist;
    va_start(alist,fmt);

    char str_tmp[MAX_LOG_LENGTH] = {0};
    vsnprintf(str_tmp,MAX_LOG_LENGTH,fmt,alist);

    char str_all[MAX_LOG_LENGTH] = {0};
    snprintf(str_all,MAX_LOG_LENGTH,"%s:[%s]:[%s:%d]:%s",flag_str,pTmpFilename,func,line,str_tmp);
    log_output(str_all);

    return 0;
}


int common_create_thread(pthread_t *thrid, thread_process thr_process, void *tharg, \
                                    int sched_policy, int sched_priority)
{
    if(!thrid || !thr_process) {
        return -1;
    }
    
    int ret = 0;
    pthread_attr_t attr;    
    pthread_attr_init(&attr);
    
    struct sched_param param;         
    switch(sched_policy) {
        case SCHED_RR:  
        case SCHED_FIFO:
            pthread_attr_setschedpolicy(&attr, sched_policy);             
            pthread_attr_getschedparam(&attr, &param); 
            if(sched_priority>0 && sched_priority<100)                
                param.sched_priority = sched_priority; 
            else                
                param.sched_priority = 99; 
            
            pthread_attr_setschedparam(&attr, &param);
            break;
        
    }
    
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    ret = pthread_create(thrid,&attr, thr_process, tharg);
    pthread_attr_destroy(&attr);
    if(ret != 0)
    {
        printf("Create Thread Fails %d\n", ret);
        return -1;
    }  
    
    return 0;
}

int common_cancel_thread(pthread_t thrid)
{
    return pthread_cancel(thrid);    
}

//自身保证result足够result_len
int common_popen_return_result(const char *pCmd, char *result, int result_len)
{
    if(!result || !pCmd || result_len<=0)
        return -1;
        
    FILE *stream;
    int ret;
    int len;

    stream = popen(pCmd, "r");
    if (NULL == stream)
    {
        return -1;
    }

    //set no block
    int fd = fileno(stream);
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= (O_NONBLOCK);

    if (fcntl(fd,F_SETFL,flags) < 0) {
        perror("fcntl:");
        goto end_handle;
    }

    //select read
    fd_set readfds;
    struct timeval timeout;    

    FD_ZERO(&readfds);

    timeout.tv_sec = 5;    
    timeout.tv_usec = 0;

    FD_SET(fd,&readfds);
    
    //处理错误
    ret = select(fd+1,&readfds,NULL,NULL,&timeout);                
    if(ret < 0) {
        printf("%s:select failed\n",pCmd);
        goto end_handle;
        
    }else if(ret == 0) {
        printf("%s select timeout\n",pCmd);
        goto end_handle;
    }else {
        if(FD_ISSET(fd,&readfds))
            fread(result, result_len, 1, stream);                           
    }
    
end_handle:
    ret = pclose(stream);
    if (-1 == ret)
    {
        return -1;
    }      
    return 0;
}

int common_popen(const char *pCmd)
{
    FILE *stream;
    int ret;
    int len;

    stream = popen(pCmd, "r");
    if (NULL == stream)
    {
        return -1;
    }
    
    ret = pclose(stream);
    if (-1 == ret)
    {
        return -1;
    }      
    return 0;
}




