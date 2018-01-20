

#ifndef __GLOBAL_H__
#define __GLOBAL_H__    
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
#if 0
#define PTHRAED_LOCK(locker) 
#define PTHRAED_INIT(locker)     
#define PTHRAED_UNLOCK(locker)      
#define PTHRAED_TRYLOCK(locker) 
#else
#define PTHRAED_INIT(locker)        pthread_mutex_init(locker,NULL)
#define PTHRAED_LOCK(locker)           pthread_mutex_lock(locker)
#define PTHRAED_UNLOCK(locker)           pthread_mutex_unlock(locker)
#define PTHRAED_TRYLOCK(locker)      pthread_mutex_trylock(locker)
#endif


extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern size_t fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream);
extern FILE *fopen(const char *path, const char *mode);
extern FILE *fdopen(int fd, const char *mode);
extern FILE *freopen(const char *path, const char *mode, FILE *stream);
extern int fclose(FILE *fp);
extern int printf(const char *format, ...);
extern int fprintf(FILE *stream, const char *format, ...);
extern int sprintf(char *str, const char *format, ...);
extern int snprintf(char *str, size_t size, const char *format, ...);
extern int vprintf(const char *format, va_list ap);
extern int vfprintf(FILE *stream, const char *format, va_list ap);
extern int vsprintf(char *str, const char *format, va_list ap);
extern int vsnprintf(char *str, size_t size, const char *format, va_list ap);
extern int fseek(FILE *stream, long offset, int whence);
extern long ftell(FILE *stream);
extern void rewind(FILE *stream);
extern int fgetpos(FILE *stream, fpos_t *pos);
extern int fsetpos(FILE *stream, fpos_t *pos);
extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memset(void *s, int c, size_t n);
extern void *malloc(size_t size);
extern void free(void *ptr);
extern void *calloc(size_t nmemb, size_t size);
extern void *realloc(void *ptr, size_t size);


namespace baseservice
{
    
#define SOCKET_ERROR            (-1)
#define THIS_VERSION "2.0.7"
#ifdef WIN32
#include <Winsock2.h>
#include <wingdi.h>
#include <windows.h>
    
#define   socklen_t int

typedef SOCKET os_socket;

    
#else
#include "global.h"
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




typedef unsigned long long UINT8;
typedef unsigned int      UINT4;
typedef unsigned short   UINT2;
typedef unsigned char    UINT1;
typedef long long     INT8;
typedef int           INT4;
typedef short           INT2;
typedef char           INT1;
typedef  void*(* createdthread)(void*);
void parseint(char *buf,int value);
void parsestr(char *buf,char *str,int length);
void parsebin(char *buf,unsigned char *bin,int length);
UINT8 GetCurTimems();
UINT8 GetCurTimeS();
UINT8 GetCurTimeUs();
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

//void resort(UINT1 * start, UINT1 *end);
//void resort(INT1 * start, INT1 *end);
template <typename Type>  void resort(Type * _start, Type *_end)
{
     
     int size=sizeof(Type);
     if(size<=0)
          return;
    
    UINT1 _t=0;
    UINT1 *start=(UINT1*)_start;
    UINT1 *end=(UINT1*)_end;
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

template <typename Type>  void resort(Type * _start, Type *_end,int size)
{
     
     if(size<=0)
          return;
    
    UINT1 _t=0;
    UINT1 *start=(UINT1*)_start;
    UINT1 *end=(UINT1*)_end;
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

void cutOffTailOf_R_N_Space(char *str);
void cutOffTailOf_R_N_Space(char *str,int len);

void *createthread(createdthread thread,void *mPara);
int set_thread_title(const char* fmt,...);
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

