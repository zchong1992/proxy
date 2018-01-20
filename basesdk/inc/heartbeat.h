
#ifndef __HEARTBEAT_H__
#define __HEARTBEAT_H__

#include "global.h"
#include "log.h"

namespace baseservice
{

enum service_type{
     SERVER=10,
    CLIENT
};
class znheartbeat
{
public:
     znheartbeat(service_type);
    ~znheartbeat();
    int setlocaladdr(char*ip,unsigned short port);
    int setremoteaddr(char*ip,unsigned short port);
    int connectremoteaddr();
    static void* listenthread(void* member);
    static void* sendthread(void*member);
    static void Init(){}
    static int start_thread(void* member);
    int check_alive(void);
private:
    int m_type;
    int m_last_update_time_s;
    int m_max_time_s;
    int m_is_connect;
     int m_socket;
    int r_socket;
    int m_port;
public:
    int m_debug;
     struct sockaddr_in m_localaddr;
     struct sockaddr_in m_remoteaddr;
};

}
#endif


