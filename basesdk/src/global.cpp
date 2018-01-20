#include "global.h"
using namespace std;
namespace baseservice{
    int set_thread_title(const char* fmt,...)
    {
        char title [16] ={0};
        va_list ap;
        va_start(ap, fmt);
        vsnprintf (title, sizeof (title) , fmt, ap);
        va_end (ap);
       return prctl(PR_SET_NAME,title);
    }
    void *createthread(createdthread thread,void *mPara)
    {
#ifdef WIN32 
        DWORD  tid;
        CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)thread,mPara,0,&tid);
        
#else
        pthread_t tid;
        pthread_create(&tid,0,thread,mPara);
        pthread_detach(tid);
#endif
        return 0;
    }


	
	
	void cutOffTailOf_R_N_Space(char *str)
	{
		if(!str)
			return ;
		int size=strlen(str);
		cutOffTailOf_R_N_Space(str,size);
		
	}
	void cutOffTailOf_R_N_Space(char *str,int len)
	{	
		int end=len;
		while(end>0)
		{
			if(str[end]=='\0'||str[end]=='\r'||str[end]=='\n')
			{
				str[end]=0;
				end--;
				continue;
			}
			break;
			
		}
	}

    UINT8 GetCurTimems()
    {
        timeval tm;
        gettimeofday(&tm,0);
        return (tm.tv_sec*1000+tm.tv_usec/1000);
    }
    UINT8 GetCurTimeS()
    {
        timeval tm;
        gettimeofday(&tm,0);
        return (tm.tv_sec);
    }
    UINT8 GetCurTimeUs()
    {
        timeval tm;
        gettimeofday(&tm,0);
        return (tm.tv_sec*1000000+tm.tv_usec);
    }
    const char *GetTimeString()
    {
         time_t now;
         struct tm *timenow;
         time(&now);
         static char strtime[100];
         timenow = localtime(&now);
         strftime(strtime,100,"[%Y-%d-%m %H:%M:%S]",timenow);
        return strtime;
    }
    const char *GetTimeString2()
    {
         time_t now;
         struct tm *timenow;
         time(&now);
         static char strtime[100];
         timenow = localtime(&now);
         strftime(strtime,100,"%04Y%02m%02d%02H%02M%02S",timenow);
        return strtime;
    }
    const char *GetTimeStringOfMs()
    {
         time_t now;
         struct tm *timenow;
         time(&now);
         static char strtime[100];
         timenow = localtime(&now);
         strftime(strtime,100,"%04Y%02m%02d%02H%02M%02S",timenow);
        timeval mtm;
        gettimeofday(&mtm,0);
        UINT4 ms=mtm.tv_usec/1000;
        char strtimems[10];
        sprintf(strtimems,"%04d",ms);
        strcat(strtime,strtimems);
        return strtime;
    }
    const char *GetTimeStringofday()
    {
         time_t now;
         struct tm *timenow;
         time(&now);
         static char strtime[100];
         timenow = localtime(&now);
         strftime(strtime,100,"%04Y%02m%02d",timenow);
        return strtime;
    }
    void parseint(char *buf,int value)
    {
         *(int*)buf=htonl(value);
    }
    void parsestr(char *buf,char *str,int length)
    {
         memcpy(buf,str,length);
    }
    void parsebin(char *buf,unsigned char *bin,int length)
    {
         memcpy(buf,bin,length);
    }


     const string GetString(const int value)
     {
          string _t="";
          char buf[30];
          sprintf(buf,"%d",value);
          _t=buf;
          return _t;
     }
     const string GetString(const float value)
     {
          string _t="";
          char buf[30];
          sprintf(buf,"%0.4f",value);
          _t=buf;
          return _t;
     }
     const string GetString(const double value)
     {
          string _t="";
          char buf[30];
          sprintf(buf,"%0.4lf",value);
          _t=buf;
          return _t;
     }
     
     const string GetString(const long value)
     {
          string _t="";
          char buf[30];
          sprintf(buf,"%d",value);
          _t=buf;
          return _t;
     }
     const string GetString(const long long value)
     {
          string _t="";
          char buf[30];
          sprintf(buf,"%d",value);
          _t=buf;
          return _t;
     }
//      void resort(UINT1 * start, UINT1 *end)
//     {
//          UINT1 _t;
//          end--;
//          while(start<end)
//          {
//               _t=*start;
//               *start=*end;
//               *end=_t;
//               start++;
//               end--;
//          }
//     }
//     void resort(INT1 * start, INT1 *end)
//     {
//          INT1 _t;
//          end--;
//          while(start<end)
//          {
//               _t=*start;
//               *start=*end;
//               *end=_t;
//               start++;
//               end--;
//          }
//     }
     int GetFileSize(FILE *fp)
     {
          int num = 0;
          int pos = ftell(fp);
          fseek(fp, 0, SEEK_END);
          num = ftell(fp);
          fseek(fp, pos, SEEK_SET);
          return num;
     }
     autolock::autolock(LOCKER *mlock)
     {
          m_lock=mlock;
          PTHRAED_LOCK(m_lock);
     }
    autolock::~autolock()
     {
          PTHRAED_UNLOCK(m_lock);
     }

}


