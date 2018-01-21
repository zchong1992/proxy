
#include "com_struct.h"


using namespace baseservice;
using namespace std;



void initLog()
{
    znlog::getInstance()->Init();
    znlog::getInstance()->set_level(INFO,INFO);
    znlog::getInstance()->set_log_file("server_offline_log.txt");
}

int createZopenConnect(VSOCK & list,int needSockNum,const char *host,int port)
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
    VSOCK zopenList;
    VSP connectedList;
	
	int server_port;
	char server_ip[1024];
	int zopen_port;
	char zopen_ip[1024];
    struct sockaddr_in server_sockaddr;
    struct sockaddr_in zopen_sockaddr;
    connectedList.clear();
    zopenList.clear();
    if(argc<5)
    {
        SYS_LOG(INFO,"need server ip and port zopen ip and port\n",argc);
        return 0;
    }
    strcpy(server_ip,argv[1]);
    server_port=atoi(argv[2]);
    strcpy(zopen_ip,argv[3]);
    zopen_port=atoi(argv[4]);
    SYS_LOG(INFO,"remote port [%s:%d] connect to [%s:%d]\n",server_ip,server_port,zopen_ip,zopen_port);
    
    char remote_ipaddr[1024];
    char local_ipaddr[1024];


    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(server_port);
    server_sockaddr.sin_addr.s_addr = inet_addr(remote_ipaddr);

    zopen_sockaddr.sin_family = AF_INET;
    zopen_sockaddr.sin_port = htons(zopen_port);
    zopen_sockaddr.sin_addr.s_addr = inet_addr(local_ipaddr);
    

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
        tv.tv_sec = 1;
        tv.tv_usec = 0;
       
        int maxIndex=0;
        VSPI it;
        for(it=connectedList.begin();it!=connectedList.end();it++)
        {
            FD_SET((*it).clientFd, &efdset);
            FD_SET((*it).clientFd, &rfdset);
            FD_SET((*it).zopenFd, &efdset);
            FD_SET((*it).zopenFd, &rfdset);
            maxIndex=max(maxIndex,(*it).clientFd);
            maxIndex=max(maxIndex,(*it).zopenFd);
        }
        VSOCKI vit;
        for(vit=zopenList.begin();vit!=zopenList.end();vit++)
        {
            FD_SET((*vit), &efdset);
            FD_SET((*vit), &rfdset);
            maxIndex=max(maxIndex,(*vit));
        }
        if(zopenList.size()<5)
        {
            int needSock=5-zopenList.size();
            ret=createZopenConnect(zopenList,needSock,zopen_ip,zopen_port);
            if(needSock==ret)
            {
                SYS_LOG(INFO,"createRConnect success need %d created:%d\n",needSock,ret);
            }
            else
            {
                SYS_LOG(INFO,"createRConnect fail need %d created:%d\n",needSock,ret);
            }
			usleep(100000); //slepp 100 ms
        }
        
        maxIndex+=1;
        int nready = select(maxIndex, &rfdset, NULL, &efdset, &tv);
       // int nready = select(maxIndex, &rfdset, NULL, &efdset, 0);
        if(nready==0)
        {
            usleep(10);
            continue;
        }
        
        for(it=connectedList.begin();it!=connectedList.end();)
        {
            if(FD_ISSET((*it).clientFd,&rfdset))
            {
                readlen=recv((*it).clientFd, writebuf, 10240,0);
                (*it).clientData[0]+=readlen;
                if(readlen<=0)
                {
                    it=closeFd(connectedList,it);
                    continue;
                }
                int ret=send((*it).zopenFd,writebuf,readlen,0);
                (*it).zopenData[1]+=ret;
                if(ret<=0)
                {
                    it=closeFd(connectedList,it);
                    continue;
                }
            }
            if(FD_ISSET((*it).zopenFd,&rfdset))
            {
                readlen=recv((*it).zopenFd, writebuf, 10240,0);
                (*it).zopenData[0]+=readlen;
                if(readlen<=0)
                {
                    it=closeFd(connectedList,it);
                    continue;
                }
                int ret=send((*it).clientFd,writebuf,readlen,0);
                (*it).clientData[1]+=ret;
                if(ret<=0)
                {
                    it=closeFd(connectedList,it);
                    continue;
                }
            }
			it++;
        }
        
        for(vit=zopenList.begin();vit!=zopenList.end();)
        {
            if(FD_ISSET((*vit),&rfdset))
            {
                int clientSock=*vit;
                int localSock=createConnect(server_ip,server_port);
                if(localSock!=0)
                {
                    SPair unit;
                    unit.clientFd=clientSock;
                    unit.zopenFd=localSock;
                    vit=zopenList.erase(vit);
                    connectedList.push_back(unit);
					SYS_LOG(INFO,"connect client current connectedList size %d\tzopenList size %d\n"
					,connectedList.size()
					,zopenList.size());
					continue;
                }
            }
			vit++;
        }
		
        for(it=connectedList.begin();it!=connectedList.end();)
        {
            if(FD_ISSET((*it).zopenFd,&efdset))
            {
            	it=closeFd(connectedList, it);
				SYS_LOG(INFO,"disconnect client current connectedList size %d\tzopenList size %d\n"
					,connectedList.size()
					,zopenList.size());
				continue;
                
            }
            if(FD_ISSET((*it).clientFd,&efdset))
            {
            	it=closeFd(connectedList, it);
				SYS_LOG(INFO,"disconnect client current connectedList size %d\tzopenList size %d\n"
					,connectedList.size()
					,zopenList.size());
				continue;
                
            }
            it++;
        }
		for(vit=zopenList.begin();vit!=zopenList.end();)
        {
            if(FD_ISSET((*vit),&efdset))
            {
            	close(*vit);
                vit=zopenList.erase(vit);
				SYS_LOG(INFO,"connect client current connectedList size %d\tzopenList size %d\n"
				,connectedList.size()
				,zopenList.size());
				continue;
                
            }
			vit++;
        }
		
    }
    END:
    //close(server_sockfd);
    return 0;
}

