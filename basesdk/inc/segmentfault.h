

 /*!
* \file segmentfault.h
* \brief linux错误捕捉类,程序初始化时调用一次,在程序异常时,打印堆栈
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/
#ifndef __SEGMENTFAULT_H__
#define __SEGMENTFAULT_H__
#include "global.h"
#include "log.h"
namespace baseservice{
        
/**
* \brief 线程信息,包含线程名和线程id
*/
typedef struct thread_info_st
{
        char name[256];
        unsigned int tid;
}TIS;

     
typedef std::vector<TIS> TISL;

 
/**
* \brief 程序异常说明类,可以在开始时调用,捕捉程序异常并在退出之前打印当前线程堆栈
*/
class znsegfault_catch
{
     public :
        static char handlesvaule[100][20];
        static void handle();
        static void catch_sign(void);
        static void no_print_seg_frame(int signal);
        static void print_seg_frame(int signal);
        static void setup_thread_info(unsigned int tid,const char *tname);
        static TIS getThreadInfo();
        static TISL m_thread_info; 

};



};
#endif 

