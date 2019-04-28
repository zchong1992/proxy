
 /*!
* \file cthread.h
* \brief 线程类,虚基类,用于新启动线程时调用
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/
#ifndef __ZKHX_THREAD_H__
#define __ZKHX_THREAD_H__

#include "global.h"

/**
* \brief cThread 线程基类
*/
class cThread
{
public:
    virtual ~cThread(){};
    cThread(){};
 
/**
* \brief start 启动线程的run函数
*/
    virtual void start();
    
/**
* \brief getThreadName 线程名,可以被重载
*/
    virtual std::string getThreadName();
private:
/**
* \brief startThread 无需调用
*/
    static void *startThread(void*);

/**
* \brief run 纯虚函数 ,需要被重载
*/
    virtual int run()=0;
};

#endif