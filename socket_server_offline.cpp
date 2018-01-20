
#include "com_struct.h"


using namespace baseservice;
using namespace std;

int g_local_port;
int g_remote_port;
char g_local_ip[1024];
char g_remote_ip[1024];


void initLog()
{
    znlog::getInstance()->Init();
    znlog::getInstance()->set_level(INFO,INFO);
    znlog::getInstance()->set_log_file("server_offline_log.txt");
}
int createConnect(const char *host,int port)
{
    char ipaddr[1024];
    if(!hostname_to_ip(host,ipaddr))
    {
        SYS_LOG(INFO,"resolved %s to %s fail\n",g_remote_ip,ipaddr);
        return 0;
    }
    
    struct sockaddr_in sockaddr;

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = inet_addr(ipaddr);
    int sock=socket(AF_INET,SOCK_STREAM,0);
    
    int ret=connect(sock,(struct sockaddr *)& sockaddr, sizeof(struct sockaddr_in));
    if(ret<0)
    {
        SYS_LOG(INFO,"connect fail ret %d\n",ret);
        close(sock);
        return 0;
    }
    return sock;
    
}

int createRConnect(VSOCK & list,int needSockNum,const char *host,int port)
{
    char remote_ipaddr[1024];
    int num=0;
    while(num<needSockNum)
    {
        int sock=createConnect(host,port);
        if(sock==0)
        {
            return num;
        }
        list.push_back(sock);
        num++;
    }
    return num;
        
    
    
}

int main(int argc,char * argv[])
{

    initLog();
    VSOCK resList;
    VSP connectList;
    resList.clear();
    connectList.clear();
    if(argc<5)
    {
        SYS_LOG(INFO,"need remote ip and port local ip and port\n",argc);
        return 0;
    }
    strcpy(g_remote_ip,argv[1]);
    g_remote_port=atoi(argv[2]);
    strcpy(g_local_ip,argv[3]);
    g_local_port=atoi(argv[4]);
    SYS_LOG(INFO,"remote port [%s:%d] connect to [%s:%d]\n",g_remote_ip,g_remote_port,g_local_ip,g_local_port);
    
    char remote_ipaddr[1024];
    char local_ipaddr[1024];

    struct sockaddr_in server_sockaddr;
    struct sockaddr_in local_sockaddr;

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(g_remote_port);
    server_sockaddr.sin_addr.s_addr = inet_addr(remote_ipaddr);

    local_sockaddr.sin_family = AF_INET;
    local_sockaddr.sin_port = htons(g_local_port);
    local_sockaddr.sin_addr.s_addr = inet_addr(local_ipaddr);
    

    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr); 
    struct timeval tv;
    fd_set rfdset;
    fd_set efdset;
    int ret=0;
    char writebuf[10240];
    int readlen=0;
    while(1)
    {
        FD_ZERO(&rfdset);
        FD_ZERO(&efdset);
        tv.tv_sec = 0;
        tv.tv_usec = 0;
       
        int maxIndex=0;
        VSPI it;
        for(it=connectList.begin();it!=connectList.end();it++)
        {
            FD_SET((*it).clientFd, &efdset);
            FD_SET((*it).clientFd, &rfdset);
            FD_SET((*it).remoteFd, &efdset);
            FD_SET((*it).remoteFd, &rfdset);
            maxIndex=max(maxIndex,(*it).clientFd);
            maxIndex=max(maxIndex,(*it).remoteFd);
        }
        VSOCKI vit;
        for(vit=resList.begin();vit!=resList.end();vit++)
        {
            FD_SET((*vit), &efdset);
            FD_SET((*vit), &rfdset);
            maxIndex=max(maxIndex,(*vit));
        }
        if(resList.size()<5)
        {
            int needSock=5-resList.size();
            ret=createRConnect(resList,needSock,g_remote_ip,g_remote_port);
            if(needSock==ret)
            {
                SYS_LOG(INFO,"createRConnect success need %d created:%d\n",needSock,ret);
            }
            else
            {
                SYS_LOG(INFO,"createRConnect fail need %d created:%d\n",needSock,ret);
            }
        }
        
        maxIndex+=1;
        int nready = select(maxIndex, &rfdset, NULL, &efdset, &tv);
        if(nready==0)
        {
            usleep(10);
            continue;
        }
        
        for(it=connectList.begin();it!=connectList.end();it++)
        {
            if(FD_ISSET((*it).clientFd,&rfdset))
            {
                readlen=recv((*it).clientFd, writebuf, 10240,0);
                (*it).clientData[0]+=readlen;
                if(readlen<=0)
                {
                    closeFd(connectList,it);
                    continue;
                }
                int ret=send((*it).remoteFd,writebuf,readlen,0);
                (*it).remoteData[1]+=ret;
                if(ret<=0)
                {
                    closeFd(connectList,it);
                    continue;
                }
            }
            if(FD_ISSET((*it).remoteFd,&rfdset))
            {
                readlen=recv((*it).remoteFd, writebuf, 10240,0);
                (*it).remoteData[0]+=readlen;
                if(readlen<=0)
                {
                    closeFd(connectList,it);
                    continue;
                }
                int ret=send((*it).clientFd,writebuf,readlen,0);
                (*it).clientData[1]+=ret;
                if(ret<=0)
                {
                    closeFd(connectList,it);
                    continue;
                }
            }
        }
        
        for(vit=resList.begin();vit!=resList.end();it++)
        {
            if(FD_ISSET((*vit),&rfdset))
            {
                int clientSock=*vit;
                int localSock=createConnect(g_local_ip,g_local_port);
                if(localSock!=0)
                {
                    SPair unit;
                    unit.clientFd=clientSock;
                    unit.remoteFd=localSock;
                    resList.erase(vit);
                    connectList.push_back(unit);
                }
            }
        }
    }
    END:
    //close(server_sockfd);
    return 0;
}

