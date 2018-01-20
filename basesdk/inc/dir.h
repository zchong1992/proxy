#ifndef __DIR_H__
#define __DIR_H__
#include "global.h"

class ZCDIR{
public:
static int checkDirExist(const char *Path);
static int createDir(const char *Path);
static int deleteDir(const char *Path);
};


#endif 

