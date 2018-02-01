
#include "com_struct.h"


using namespace baseservice;
using namespace std;


inline int max(int a,int b)
{
    return a>b?a:b;
}

inline int min(int a,int b)
{
    return a<b?a:b;
}

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
void fillZopenSock(VSOCK & zopenList,int num,const char * ip,int port)
{
    int res=zopenList.size();
    int ret=res;
    if(ret<num)
    {
        int needSock=num-ret;
        ret=createZopenConnect(zopenList,needSock,ip,port);
        if(needSock==ret)
        {
            SYS_LOG(INFO,"createRConnect success need %d created:%d\n",needSock,ret);
        }
        else
        {
            SYS_LOG(INFO,"createRConnect fail need %d created:%d\n",needSock,ret);
        }
    }
    SYS_LOG(INFO,"zopenList current %d need %d create %d\n",res,num-res,ret);
}
int main(int argc,char * argv[])
{

    initLog();
    VSOCK zopenList;
    VSP connectedList;
	int resConnectNum=5;
	int MaxConnectNum=100;
	
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
        VSOCKI vit;
        fillZopenSock(zopenList,resConnectNum,zopen_ip,zopen_port);

        
        for(it=connectedList.begin();it!=connectedList.end();it++)
        {
            FD_SET((*it).clientFd, &efdset);
            FD_SET((*it).clientFd, &rfdset);
            FD_SET((*it).zopenFd, &efdset);
            FD_SET((*it).zopenFd, &rfdset);
            maxIndex=max(maxIndex,(*it).clientFd);
            maxIndex=max(maxIndex,(*it).zopenFd);
        }
        for(vit=zopenList.begin();vit!=zopenList.end();vit++)
        {
            FD_SET((*vit), &efdset);
            FD_SET((*vit), &rfdset);
            maxIndex=max(maxIndex,(*vit));
        }
        maxIndex+=1;
        int nready=0;
        if(resConnectNum==zopenList.size())
        {
            nready = select(maxIndex, &rfdset, NULL, &efdset, 0);
        }
        else
        {
            nready = select(maxIndex, &rfdset, NULL, &efdset, &tv);
        }
        SYS_LOG(INFO,"nready =%d\n",nready);
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
                SYS_LOG(INFO,"zopenFd read data  connectedList size %d\tzopenList size %d\n",connectedList.size()
				,zopenList.size());
                
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
				SYS_LOG(INFO,"connect client localSock %d connectedList size %d\tzopenList size %d\n",localSock,connectedList.size()
				,zopenList.size());
                if(localSock!=0)
                {
                    SPair unit;
                    unit.clientFd=clientSock;
                    unit.zopenFd=localSock;
                    vit=zopenList.erase(vit);
                    connectedList.push_back(unit);
					resConnectNum=max(resConnectNum,connectedList.size()*2+5);
					resConnectNum=min(resConnectNum,MaxConnectNum);
                    fillZopenSock(zopenList,resConnectNum,zopen_ip,zopen_port);
                    SYS_LOG(INFO,"connectedList size %d zopenList size %d\n",connectedList.size(),zopenList.size());
					continue;
                }
            }
			vit++;
        }
		//catch exception
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

