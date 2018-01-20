#include "log.h"
#include "object.h"
#include "aes.h"

using namespace std;

namespace baseservice{

    znlog* znlog::Instance=0;
    LOCKER znlog::static_log_locker=PTHREAD_MUTEX_INITIALIZER;
    
    void znlog::lock_g_locker()
    {
        PTHRAED_LOCK(&static_log_locker);
    }
    void znlog::init_g_locker()
    {
        PTHRAED_INIT(&static_log_locker);
    }
    void znlog::unlock_g_locker()
    {
        PTHRAED_UNLOCK(&static_log_locker);
    }


    znlog* znlog::getInstance()
    {
        if (Instance == NULL)
        {
            lock_g_locker();
            if (Instance == NULL)
            {
                Instance = new znlog();
                    //createthread(logsockThread,Instance);
            }
            unlock_g_locker();
        }
        return Instance;
    }
    void znlog::SYS_LOG2(const int level,const char *fmt,...)
    {
        znlog * znlogpointer = znlog::getInstance();
        char str[2048];

          const char *plevel="[null]"; 
          switch (level)
          {     
          case INFO:
               plevel="[INFO]";
               break;
          case WARNING:
               plevel="[WARNING]";
               break;
          case ERROR:
               plevel="[ERROR]";
               break;
          }
          strcpy(str,plevel);
          
        const char *timeStr=GetTimeString();

          strcat(str,timeStr);
          
          int use_len=strlen(str);
        va_list args;
        va_start(args,fmt);
        vsprintf(str+use_len,fmt,args);
        va_end(args);
          int str_len=strlen(str);
//          znlogpointer->write(level,timeStr,time_str_len);
//          znlogpointer->SockSendLog(timeStr,time_str_len);
        znlogpointer->write(level,str,str_len);
          znlogpointer->SockSendLog(str,str_len);
    }
     void znlog::TEMPLOG(const char *path,const char *fmt,...)
    {
        char str[2048];
          char time[2048];
        const char *timeStr=GetTimeString();
          strcpy(time,timeStr);
        va_list args;
        va_start(args,fmt);
        vsprintf(str,fmt,args);
        va_end(args);
          strcat(time,str);
          strcat(time,"\n");
         FILE * fp=fopen(path,"a+");
          if(fp)
          {
               fwrite(time,1,strlen(time),fp);
               
               if(fp)
                   fclose(fp);
               fp=0;
          }
          printf("%s",time);
    }
    void znlog::SYS_LOG3(const int level,const char *fmt,...)
    {
        znlog * znlogpointer = znlog::getInstance();
        char str[2048];
        va_list args;
        va_start(args,fmt);
        vsprintf(str,fmt,args);
        va_end(args);
        znlogpointer->write(level,str,strlen(str));
    }
    void znlog::Init()
    {
        //init_g_locker();
        znlog * znlogpointer = znlog::getInstance();
        //PTHRAED_INIT(&znlogpointer->m_locker);
    }

    znlog::znlog()
    {
        m_is_can_write=0;
        m_file=0;
        m_cur_level_write=INFO;
        m_cur_level_print=INFO;
        m_filename[0]=0;
        PTHRAED_INIT(&m_locker);
    }
    znlog::~znlog()
    {
        fflush(this->m_file);
        fclose(this->m_file);
        this->m_file=0;
    }
    void znlog::set_level(int write,int print)
    {
        if(write >=ERROR && write <=INFO)
            this->m_cur_level_write =write;
        if(print >=ERROR && print <=INFO)
            this->m_cur_level_print =print;
        if(write==0)
            this->m_cur_level_write =0;
        if(print ==0)
            this->m_cur_level_print =0;
    }
     
    void znlog::SockSendLog(const char *buf,int len)
     {
          if(m_sock>0)
          {
               int ret=send(m_sock,buf,len,0);
               if(ret<1)
               {
                    PTHRAED_LOCK(&m_locker);
                    closesocket(m_sock);
                    m_sock=-1;
                    PTHRAED_UNLOCK(&m_locker);
               }
          }
     }
     void znlog::set_sock_addr(const char * ip,unsigned short port)
     {
          strcpy(m_ip,"0.0.0.0");
          m_port=port;
     }
          
    void znlog::startSocketThread()
     {
          createthread(logsockThread,this); 
     }
     char *showHex(char * addr,int len)
     {
          static char buf[1024];
          if(len>1024)
          {
          }
          return 0;
     }
     int checkSocketAvialbe(int sock)
     {
          char buf1[128];
          char buf2[128];
          char buf3[128];
          char buf4[128];
          char iv[20]={0};
          char getKey[20];
          char setKey[20];
         memcpy(getKey,"zkzndemoandroids",16);  //
         memcpy(setKey,"zkzndemoandroidg",16);

              AESModeOfOperation maes;
         maes.set_iv((UINT1*)iv);
         maes.set_key((UINT1*)setKey);
         maes.set_mode(MODE_CBC);
         int outlen=0;
          memcpy(buf1,"1234567890\0",11);
         outlen=maes.Encrypt((UINT1*)buf1,16,(UINT1*)buf2);
          SYS_LOG(INFO,"send data %s to  log socket\n",buf1);
          int ret=send(sock,buf2,16,0);
          if(ret<=0)
          {
               SYS_LOG(INFO,"send log socket fail\n");
               return 0;
          }
          int recvlen=0;
         maes.set_iv((UINT1*)iv);
         maes.set_key((UINT1*)setKey);
         maes.set_mode(MODE_CBC);
         outlen=maes.Decrypt((UINT1*)buf2,16,(UINT1*)buf4);
          SYS_LOG(INFO,"should decode data %s \n",buf4);
          while(recvlen<16)
          {
               ret=recv(sock,buf3+recvlen,16-recvlen,0);
               if(ret<=0)
               {
                    SYS_LOG(INFO,"recv log socket fail\n");
                    return 0;
               }
               recvlen+=ret;
          }
         maes.set_iv((UINT1*)iv);
         maes.set_key((UINT1*)getKey);
         maes.set_mode(MODE_CBC);
         outlen=maes.Decrypt((UINT1*)buf3,16,(UINT1*)buf4);
          SYS_LOG(INFO,"recv data %s to  log socket\n",buf4);
          if(memcmp(buf1,buf4,11)==0)
               return 1;
          return 0;
     }
    void * znlog::logsockThread(void * Para)
     {
          set_thread_title("logsockThread");
        SYS_LOG(INFO,"sockThread satart\n");  
          znlog * m_fd=(znlog*)Para;
          os_socket sockSrv = socket( AF_INET , SOCK_STREAM , 0 );
        SOCKADDR_IN addrSrv;
        int flag=1,len=sizeof(int);
        addrSrv.sin_addr.s_addr=0;
        addrSrv.sin_port=htons(m_fd->m_port);
        addrSrv.sin_family=AF_INET;
        if( setsockopt(sockSrv, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1)  
        {  
            SYS_LOG(INFO,"setsockopt fail");  
            exit(0);  
        }  
        if(bind( sockSrv ,(SOCKADDR*)&addrSrv , sizeof(SOCKADDR) ) !=0)
        {
            SYS_LOG(ERROR,"bind %d fail\n",m_fd->m_port);
            closesocket(sockSrv);
            exit(0);
        }
        listen( sockSrv , 1 );
        SOCKADDR_IN addrClient;
        len = sizeof( SOCKADDR_IN );
    
        while(1)
        {
             int tmp_sock=0;
            tmp_sock = accept( sockSrv , (SOCKADDR *)&addrClient , (socklen_t*)&len );
               SYS_LOG(INFO,"recv a log request \n");
               if(checkSocketAvialbe(tmp_sock)==0)
               {
                    SYS_LOG(INFO,"check log socket fail \n");
                    closesocket(tmp_sock);
                    continue;
               }
               SYS_LOG(INFO,"check log socket success\n");
             PTHRAED_LOCK(&m_fd->m_locker);
               if(m_fd->m_sock<=0)
               {
                    m_fd->m_sock=tmp_sock;
               }
               else
               {
                    SYS_LOG(WARNING,"may have two client work togetter m_sock<=0 fail %d\n",m_fd->m_sock);
                    closesocket(tmp_sock);
               }
               PTHRAED_UNLOCK(&m_fd->m_locker);
               while(m_fd->m_sock>0)
               {
                    usleep(100);
               }
          }
               
     }
    int znlog::write(const int level,const char * str,const int len)
    {
         int ret=0;
        if(str==0)
            return -1;
        if(level!=0 && this->m_cur_level_write >=level)
        {
            PTHRAED_LOCK(&this->m_locker);
            if(this->m_file==0)
            {   
                this->m_file=fopen(this->m_filename,"a+");
                if(this->m_file==0)
                 {
                     ret= -2;
                      PTHRAED_UNLOCK(&this->m_locker);
                         goto WRITEEND;
                         
                 }
                         
            }
               int curlen=ftell(this->m_file);
               const int constfileSize=500*(1<<20);
               //printf("curlen log size %d\n",curlen);
               if(curlen>constfileSize)
               {
                    PTHRAED_LOCK(&m_locker_change_file);
                    const int bufferSize=1<<20;
                    int curlen=ftell(this->m_file);
                    if(curlen>constfileSize)
                    {
                         char * buf=(char*)malloc(bufferSize+1);
                         if(buf)
                         {
                              FILE* fp =fopen(this->m_filename,"r");
                              if(fp)
                              {
                                   fseek(fp,-1*(bufferSize),SEEK_END);
                                   fread(buf,1,bufferSize,fp);
                                   fclose(fp);
                              }
                              else
                              {
                                   free(buf);
                                   buf=0;
                              }
                         }
                         fclose(this->m_file);
                      this->m_file=fopen(this->m_filename,"w");
                         if(buf)
                         {
                              fwrite(buf,1,bufferSize,this->m_file);
                              free(buf);
                         }
                         buf=0;
                    }
                    PTHRAED_UNLOCK(&m_locker_change_file);
               }
            fwrite(str,len,1,this->m_file);
            fflush(this->m_file);
            string mystr=str;
            object *m_obj=object::Getobject();
            if(m_obj!=0)
            {
                m_obj->log.setvalue(mystr);
            }
            PTHRAED_UNLOCK(&this->m_locker);
        }
        if(level !=0 && this->m_cur_level_print >=level)
        {
            printf("%s",str);
            fflush(stdout);
        }
     WRITEEND:
        return ret;
        
    }
    int znlog::set_log_file(const char*name)
    {
        if(name ==0)
            return -1;
        int len=strlen(name);
        if(len>1023)
        {
            return -2;
        }
        if(len==0)
            return -3;
        memcpy(this->m_filename,name,len);
        this->m_filename[len]=0;
        if(this->m_file==0)
            this->m_file=fopen(this->m_filename,"a+");
        else
        {
            fclose(this->m_file);
            this->m_file=fopen(this->m_filename,"a+");
        }
        return 0;
        
    }
}

