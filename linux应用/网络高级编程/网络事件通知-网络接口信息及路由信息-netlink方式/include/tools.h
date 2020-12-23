#ifndef _TOOLS_H_
#define _TOOLS_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include <pthread.h>



#define log_debug(fmt, ...)  common_log_print("DEBUG", __FILE__,__FUNCTION__,__LINE__,fmt,##__VA_ARGS__)
#define log_error(fmt, ...)  common_log_print("ERR", __FILE__,__FUNCTION__,__LINE__,fmt,##__VA_ARGS__)


#define log_output(a) printf("%s\n",a);


#define func_exec(func,ret)\
do{\
    ret = func;\
    if(ret < 0) {\
        printf("exec %s error:%d\n",#func,ret);\
        return ret;\
    }\
}while(0)

#define hwaddr_print(string,addr)\
do{\
    char tmpstr[128] = {0};\
    sprintf(tmpstr,"%02x:%02x:%02x:%02x:%02x:%02x",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);\
    log_debug("%s:%s",string,tmpstr);\
}while(0)

#define ipaddr_print(string,addr)\
do{\
    char tmpstr[128] = {0};\
    sprintf(tmpstr,"%d:%d:%d:%d",addr[0],addr[1],addr[2],addr[3]);\
    log_debug("%s:%s",string,tmpstr);\
}while(0)



typedef void *(*thread_process) (void *);
int common_log_print(const char *flag_str, char *file, const char *func, unsigned int line, char *fmt, ...);

int common_create_thread(pthread_t *thrid, thread_process thr_process, void *tharg, \
                                    int sched_policy, int sched_priority);
int common_cancel_thread(pthread_t thrid);

int common_popen_return_result(const char *pCmd, char *result, int result_len);
int common_popen(const char *pCmd);


#ifdef __cplusplus
}
#endif

#endif


