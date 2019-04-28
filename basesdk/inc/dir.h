
 /*!
* \file dir.h
* \brief 文件夹操作相关类
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/
#ifndef __DIR_H__
#define __DIR_H__
#include "global.h"

class ZCDIR{
public:

/**
* \brief checkDirExist 检查路径是否存在
*/
static int checkDirExist(const char *Path);
/**
* \brief checkDirExist 创建路径
*/
static int createDir(const char *Path);
/**
* \brief deleteDir 删除路径
*/
static int deleteDir(const char *Path);
};


#endif 

