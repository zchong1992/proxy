 /*!
* \file base64.h
* \brief 该头文件主要是用于base64转码的操作
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/
#ifndef __BASE64_H__
#define __BASE64_H__

#include "global.h"
namespace baseservice{

/**
* \brief base64encode base64转码函数
* \param out  输入缓冲区
* \param in  输入缓冲区
* \param length  输出缓冲区
* \return 输出缓冲区的使用长度
*/
extern int base64encode(unsigned char *out, unsigned char *in, int length);

/**
* \brief base64encode base64解码函数
* \param out  输入缓冲区
* \param in  输入缓冲区
* \param length  输出缓冲区
* \return 输出缓冲区的使用长度
*/
extern int base64decode(unsigned char *out, unsigned char *in, int length);
extern void inittable();
}


#endif 
