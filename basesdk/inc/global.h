

 /*!
* \file global.h
* \brief 被引用的标准库函数放在这里,为后期做编译优化而做的集中管理
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__    

#ifdef  WIN32
#define ZWINDOWS
#else
#define  ZLINUX
#endif

#ifdef ZWINDOWS
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <cassert>
#include <stdarg.h>  
#include <time.h>   
#include <map>
#include <list>
#include <vector>
typedef int LOCKER;
#define PTHREAD_LOCK(locker) 
#define PTHRAED_INIT(locker)     
#define PTHREAD_UNLOCK(locker)      
#define PTHRAED_TRYLOCK(locker) 
#endif
#ifdef ZLINUX

#include <sys/types.h>
#include <sys/syscall.h>
#include <arpa/inet.h> 
#include <dirent.h> 
#include <errno.h>
#include <execinfo.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>  
#include <string>
#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/prctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <cassert>
#include <stdarg.h>  
#include <time.h>
#include <unistd.h>
#include <map>
#include <list>
#include <vector>
typedef pthread_mutex_t LOCKER;

#define PTHRAED_INIT(locker)        pthread_mutex_init(locker,NULL)
#define PTHREAD_LOCK(locker)        pthread_mutex_lock(locker)
#define PTHREAD_UNLOCK(locker)      pthread_mutex_unlock(locker)
#define PTHRAED_TRYLOCK(locker)     pthread_mutex_trylock(locker)

#endif

typedef int pid_t;
#define SOCKET_ERROR            (-1)
#define THIS_VERSION "2.0.9"
#ifdef WIN32
#include <Winsock2.h>
#include <windows.h>
#define   socklen_t int
typedef SOCKET os_socket;


#else
#define Sleep(a) usleep(a*1000)
#define closesocket close
#define SOCKADDR sockaddr

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

typedef unsigned int  os_socket;
typedef struct sockaddr_in SOCKADDR_IN;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned short WORD;
typedef unsigned char UCHAR;
#endif



namespace baseservice
{

#pragma pack(1)
typedef struct mytagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} myBITMAPINFOHEADER;
typedef struct myBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} myBITMAPFILEHEADER;
typedef struct mytagBITMAP_FILE{
 
      myBITMAPFILEHEADER header;
      myBITMAPINFOHEADER infoheader;
      //PALETTEENTRY palette[256];
      UCHAR buffer[0];   //UCHAR 大小1字节(同BYTE), 在VC6下
 
} BITMAP_FILE;
typedef struct myhead{
     unsigned char name[4];
     unsigned short type;
     unsigned int len;
     unsigned char data[0];
}HEAD;
#pragma pack()




typedef unsigned long long ZUINT8;
typedef unsigned int      ZUINT4;
typedef unsigned short   ZUINT2;
typedef unsigned char    ZUINT1;
typedef long long     ZINT8;
typedef int           ZINT4;
typedef short           ZINT2;
typedef char           ZINT1;
typedef  void*(* createdthread)(void*);
void parseint(char *buf,int value);
void parsestr(char *buf,char *str,int length);
void parsebin(char *buf,unsigned char *bin,int length);
ZUINT8 GetCurTimems();
ZUINT8 GetCurTimeS();
ZUINT8 GetCurTimeUs();
const char *GetTimeString();
const char *GetTimeStringOfMs();
const char *GetTimeString2();
const char *GetTimeStringofday();
const std::string GetString(const int value);
const std::string GetString(const float value);
const std::string GetString(const double value);
const std::string GetString(const long value);
const std::string GetString(const long int value);
const std::string GetString(const long long value);

int GetFileSize(FILE *fp);
void munllfuncLOHIhVASASD32dre90();

//void resort(ZUINT1 * start, ZUINT1 *end);
//void resort(INT1 * start, INT1 *end);
/**
* \brief resort 模板函数,做数据倒序
*/
template <typename Type>  void resort(Type * _start, Type *_end)
{
     int size=sizeof(Type);
     if(size<=0)
          return;
    
    ZUINT1 _t=0;
    ZUINT1 *start=(ZUINT1*)_start;
    ZUINT1 *end=(ZUINT1*)_end;
     end-=size;
     while(start<end)
     {
          int i=0;
          for(i=0;i<size;i++)
          {
               _t=*(start+i);
               *(start+i)=*(end+i);
               *(end+i)=_t;
          }
          start+=size;
          end-=size;
     }
}
/**
* \brief resort 模板函数,做数据倒序
*/
template <typename Type>  void resort(Type * _start, Type *_end,int size)
{
     
     if(size<=0)
          return;
    
    ZUINT1 _t=0;
    ZUINT1 *start=(ZUINT1*)_start;
    ZUINT1 *end=(ZUINT1*)_end;
     end-=size;
     while(start<end)
     {
          int i=0;
          for(i=0;i<size;i++)
          {
               _t=*(start+i);
               *(start+i)=*(end+i);
               *(end+i)=_t;
          }
          start+=size;
          end-=size;
     }
}
#ifndef USE_VAR
#define USE_VAR(a) if(&a==&a){}
#endif
/**
* \brief cutOffTailOf_R_N_Space 删除字符串中首尾空格换行制表符
*/
void cutOffTailOf_R_N_Space(char *str);
/**
* \brief cutOffTailOf_R_N_Space 删除字符串中首尾空格换行制表符
*/
void cutOffTailOf_R_N_Space(char *str,int len);

/**
* \brief createthread 创建线程
*/
void *createthread(createdthread thread,void *mPara);

/**
* \brief set_thread_title 设置线程标题
*/
int set_thread_title(const char* fmt,...);
long int gettid();
#ifdef ZWINDOWS
void usleep(int);
#endif

/**
* \brief autolock 自动锁,构造时锁住,析构时解锁
*/
class autolock
{
public:
     autolock(LOCKER *mlock);
    ~autolock();
private:
    LOCKER *m_lock;
     
};
}
#endif

