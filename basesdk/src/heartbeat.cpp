#include "heartbeat.h"
using namespace baseservice;

#define LOG(fmt,...) printf( "[%s:%d]" fmt,__FILE__,__LINE__,##__VA_ARGS__)

znheartbeat::znheartbeat(service_type type)
{
     this->m_type=type;
     this->m_max_time_s=10;
    struct timeval  tv;
    gettimeofday(&tv,NULL);
    this->m_last_update_time_s=tv.tv_sec;
     this->r_socket=0;
     this->m_socket=0;
}
void* znheartbeat::sendthread(void*member)
{
     member=member;

     znheartbeat* pointer=(znheartbeat*)member;
     int sleepTime=pointer->m_max_time_s/3;
    char buf[100];
    int ret;
    int flag=0;
    int len=sizeof(int);
    if(sleepTime<=0)
        sleepTime=1;
    sprintf(buf,"seq");
    while(1)
    {
         pointer->m_socket=socket(AF_INET,SOCK_STREAM,0);
         if(pointer->m_socket <=0)
         {
             LOG("socket fail\n");
            sleep(sleepTime);
            continue;
              
         }
        setsockopt(pointer->m_socket, SOL_SOCKET, SO_REUSEADDR, &flag, len);
         ret=pointer->connectremoteaddr();
        if(ret == 0)
        {
             pointer->m_is_connect=1;
            
        }
        else
        {     
             pointer->m_is_connect =0;
            sleep(sleepTime);
            continue;
        }
         while(pointer->m_is_connect)
        {
             ret=send(pointer->m_socket,buf,3,0);
             if(ret<=0)
            {
                 pointer->m_is_connect=0;
                close(pointer->m_socket);
                pointer->m_socket=0;
            }
             sleep(sleepTime);
        }
    }
     return 0;
}

void* znheartbeat::listenthread(void* member)
{
     member=member;
     
     struct timeval  tv;
     znheartbeat* pointer=(znheartbeat*)member;
    static char buf[100]={0};
    int ret=0;
    socklen_t len=sizeof(struct sockaddr_in);
    listen(pointer->m_socket,1);
    while(1)
    {
         pointer->r_socket=accept(pointer->m_socket,(struct sockaddr*)&pointer->m_remoteaddr,&len);
        LOG("client %s:%u connect !\n",inet_ntoa(pointer->m_remoteaddr.sin_addr),pointer->m_remoteaddr.sin_port);
         while(1)
        {
             ret=recv(pointer->r_socket,buf,100,0);
             if(ret==0)
            {
                 close(pointer->r_socket);
                 pointer->r_socket=0;
                
                    gettimeofday(&tv,NULL);
                pointer->m_last_update_time_s=tv.tv_sec - pointer->m_max_time_s;
                LOG("client %s:%u disconnect !\n",inet_ntoa(pointer->m_remoteaddr.sin_addr),pointer->m_remoteaddr.sin_port);
                   memset(&pointer->m_remoteaddr,0,sizeof(pointer->m_remoteaddr));
                break;
            }
            else if(ret > 0)
            {
                 // ok
                 buf[ret]=0;
                LOG("%s\n",buf);
                buf[0]=0;
                    gettimeofday(&tv,NULL);
                pointer->m_last_update_time_s=tv.tv_sec;
            }
            else 
            {
                 LOG("recv error\n");
            }
        }
    }
     return 0; 
}
int znheartbeat::start_thread(void* member)
{
    znheartbeat* pointer=(znheartbeat*)member;
     pthread_t pid;
     if(pointer->m_type == CLIENT )
     {
         if (pthread_create(&pid, NULL,znheartbeat::sendthread,member))
         {
             return -1;
         }
     }
     else if(pointer->m_type == SERVER )
     {     
          if(pthread_create(&pid,NULL,znheartbeat::listenthread,member))
          {
               return -1;
          }
     }
     else
          return -2;
     return 0;
}

znheartbeat::~znheartbeat()
{
     if(this->m_socket!=0)
          close(this->m_socket);
     this->m_socket=0;
     if(this->r_socket!=0)
          close(this->r_socket);
     this->r_socket=0;
}
int znheartbeat::setremoteaddr(char * ip,unsigned short port)
{
     this->m_remoteaddr.sin_family=AF_INET;  
       this->m_remoteaddr.sin_port=htons(port); 
     if( ip !=0 )
     {
          this->m_remoteaddr.sin_addr.s_addr=inet_addr(ip);
     }
     else
     {
          this->m_remoteaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
     }
     return 0;
}
int znheartbeat::setlocaladdr(char * ip,unsigned short port)
{
     
    int flag=0;
    int len=sizeof(int);
     this->m_localaddr.sin_family=AF_INET;  
    if( this->m_type ==SERVER)
           this->m_localaddr.sin_port=htons(port); 
    else
           this->m_localaddr.sin_port=0; 
     if( ip !=0 )
     {
          this->m_localaddr.sin_addr.s_addr=inet_addr(ip);
     }
     else
     {
          this->m_localaddr.sin_addr.s_addr=0;
     }

    if(this->m_type==SERVER)
    {
         this->m_socket=socket(AF_INET,SOCK_STREAM,0);
         if(this->m_socket <=0)
         {
             LOG("socket fail\n");
              return -1;
              
         }
        setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &flag, len);
         if(0!=bind(this->m_socket,(struct sockaddr*)&this->m_localaddr,sizeof(struct sockaddr)))
         {
             LOG("bind %d fail\n",ntohs(this->m_localaddr.sin_port));
              return -2;
         }
    }
     return 0;
}
int znheartbeat::connectremoteaddr()
{
    if(connect(this->m_socket,(struct sockaddr*)&this->m_remoteaddr,sizeof(this->m_remoteaddr))!=0)
    {
        LOG("connect %s:%u fail\n",inet_ntoa(this->m_remoteaddr.sin_addr),ntohs(this->m_remoteaddr.sin_port));
        return -1;
    }
     return 0;
}

int znheartbeat::check_alive()
{
    
     struct timeval tv;
     gettimeofday(&tv,NULL);
    if( this->m_max_time_s < (tv.tv_sec - this->m_last_update_time_s))
        return 0;
    else
        return 1;
}



