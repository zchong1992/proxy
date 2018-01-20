
#ifndef __LOG_H__
#define __LOG_H__

#include "global.h"
namespace baseservice
{
     

     enum level{
          NOLOG=0,
          ERROR,
         WARNING,
         INFO
     };

     class znlog
     {
     public:
		static znlog* Instance;
		static LOCKER static_log_locker;
		static znlog* getInstance();
		static void SYS_LOG2(const int level,const char *fmt,...);
		static void SYS_LOG3(const int level,const char *fmt,...);
		static void TEMPLOG(const char *path,const char *fmt,...);
		static void Init();
		int set_log_file(const char*name);
		void set_level(int write,int print);
		int write(const int level,const char * str,const int len);
		void SockSendLog(const char *buf,int len);
		void set_sock_addr(const char * ip,unsigned short port);
		void startSocketThread();
		static void * logsockThread(void * Para);
     private:
		znlog();
		~znlog();
		void static lock_g_locker();
		void static init_g_locker();
		void static unlock_g_locker();
		int m_is_can_write;
		FILE* m_file;
		int m_cur_level_write;
		int m_cur_level_print;
		char m_ip[32];
		unsigned short m_port;
		int  m_sock;
		char m_filename[1024];
		LOCKER m_locker;
		LOCKER m_locker_change_file;  
     };
#if 1 

#define SYS_LOG(type,fmt,...) znlog::SYS_LOG2(type,"[%s:%s:%d]"  fmt,__FUNCTION__,__FILE__,__LINE__,##__VA_ARGS__)
#define SYS_LOG_NO_TIME znlog::SYS_LOG3
#define TEMP_LOG(FILE,fmt,...) znlog::TEMPLOG(FILE,"[%s:%s:%d]"  fmt,__FUNCTION__,__FILE__,__LINE__,##__VA_ARGS__)

#define SYS_LOG_TIMER(TIMER,time,type,fmt,...) do {     \
    static TIME_T __static_sad##TIMER;                  \
    TIME_T &lastTime= __static_sad##TIMER;              \
    TIME_T curTime;                                     \
    GET_TIME(&curTime);                                 \
    int diff=CAL_TIME(&curTime,&lastTime);              \
    if(diff>time||diff<=0)                              \
    {                                                   \
         __static_sad##TIMER=curTime;                   \
         SYS_LOG(type,fmt,##__VA_ARGS__);               \
    }                                                   \
    }while(0)                                                  


#else
#define SYS_LOG(level,fmt,...)
#define SYS_LOG_NO_TIME(...)
#endif 
}
#endif


