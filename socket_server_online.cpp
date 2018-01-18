#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //for exit(0);
#include <sys/socket.h>
#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent
#include <arpa/inet.h>
#include <vector>
#include "libbaseservice.h"
#define MYPORT  8887
#define QUEUE   20
#define BUFFER_SIZE 1024
int g_listen_port;
int g_remote_port;
using namespace baseservice;
using namespace std;
typedef struct socketPair
{
	int clientFd;
	int remoteFd;
	int clientData[2];
	int remoteData[2];
	socketPair(){memset(this,0,sizeof(socketPair));}
	socketPair(socketPair &_A){memcpy(this,&_A,sizeof(socketPair));}
}SPair;

typedef vector<SPair> VSP;
typedef vector<SPair>::iterator VSPI;
typedef vector<int> VSOCK;
typedef vector<int>::iterator VSOCKI;

void initLog()
{
	znlog::getInstance()->Init();
	znlog::getInstance()->set_level(INFO,INFO);
	znlog::getInstance()->set_log_file("server_online_log.txt");
}
inline int max(int a,int b)
{
	return a>b?a:b;
}

void closeFd(VSP &A,VSPI it)
{
}

int main(int argc,char * argv[])
{
	vector<SPair> sockList;
    VSOCK  clientList;
    VSOCK  mList;

	
	initLog();
    if(argc<3)
	{
		SYS_LOG(INFO,"need listen port mlisten port\n",argc);
    	return 0;
	}
	g_listen_port=atoi(argv[1]);
	g_remote_port=atoi(argv[2]);
	SYS_LOG(INFO,"local port %d connect to remote port %d\n",g_listen_port,g_remote_port);
	

    int client_listen_sockfd = socket(AF_INET,SOCK_STREAM, 0);
    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);

	int opt=1;
	setsockopt(client_listen_sockfd, SOL_SOCKET, SO_REUSEADDR,(void *)&opt, sizeof(opt));
	setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR,(void *)&opt, sizeof(opt));

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(g_listen_port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(client_listen_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("client_listen_sockfd");
        exit(1);
    }
	
    server_sockaddr.sin_port = htons(g_remote_port);
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("server_sockfd ");
        exit(1);
    }
	

    if(listen(client_listen_sockfd,1024) == -1)
    {
        perror("client_listen_sockfd");
        exit(1);
    }
    if(listen(server_sockfd,1024) == -1)
    {
        perror("server_sockfd");
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
	
    struct timeval tv;
    fd_set rfdset;
    fd_set efdset;
	int tmpSocket=0;
	char writebuf[10240];
	int  readlen=0;
	while(1)
	{
		FD_ZERO(&rfdset);
	    FD_SET(server_sockfd, &rfdset);
		FD_SET(client_listen_sockfd, &rfdset);
	    FD_ZERO(&efdset);
	    FD_SET(server_sockfd, &efdset);
		FD_SET(client_listen_sockfd, &efdset);
		int maxIndex=max(server_sockfd,client_listen_sockfd);
		VSPI it;
		for(it=sockList.begin();it!=sockList.end();it++)
		{
		
		    FD_SET((*it).clientFd, &efdset);
		    FD_SET((*it).clientFd, &rfdset);
		    FD_SET((*it).remoteFd, &efdset);
		    FD_SET((*it).remoteFd, &rfdset);
			maxIndex=max(maxIndex,(*it).clientFd);
			maxIndex=max(maxIndex,(*it).remoteFd);
			
		}
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		int nready = select(maxIndex, &rfdset, NULL, &efdset, &tv);
		if(nready==0)
		{
			//SYS_LOG(INFO,"select timeout %d\n",nready);
			usleep(10);
			continue;
		}
		if(FD_ISSET(client_listen_sockfd,&rfdset))
		{
			int client=accept(client_listen_sockfd, (struct sockaddr*)&client_addr, &length);
        	SYS_LOG(INFO,"new client connected %d\n",nready);
            if(mList.size()>0)
            {
                SPair unit;
                unit.clientFd=client;
                VSOCKI it=mList.begin();
                unit.remoteFd=*it;
                mList.erase(it);
            }
            else
            {
                clientList.push_back(client);
            }
            nready--;
			
		}
		if(FD_ISSET(server_sockfd,&rfdset))
		{
			int remoteSock=accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
        	SYS_LOG(INFO,"FD_ISSET(server_sockfd,&rfdset) %d\n",nready);
            if(clientList.size()>0)
            {
                SPair unit;
                unit.remoteFd=remoteSock;
                VSOCKI it=clientList.begin();
                unit.clientFd=*it;
                clientList.erase(it);
            }
            else
            {
                mList.push_back(remoteSock);
            }
            nready--;
		}
		for(it=sockList.begin();it!=sockList.end();it++)
		{
		
            if(nready==0)
    		{
    			break;
    		}
			if(FD_ISSET((*it).clientFd,&rfdset))
			{
				readlen=recv((*it).clientFd, writebuf, 10240,0);
				(*it).clientData[0]+=readlen;
				if(readlen<=0)
				{
					closeFd(sockList,it);
					continue;
				}
				int ret=send((*it).remoteFd,writebuf,readlen,0);
				(*it).remoteData[1]+=ret;
				if(ret<=0)
				{
					closeFd(sockList,it);
					continue;
				}
			}
			if(FD_ISSET((*it).remoteFd,&rfdset))
			{
				readlen=recv((*it).remoteFd, writebuf, 10240,0);
				(*it).remoteData[0]+=readlen;
				if(readlen<=0)
				{
					closeFd(sockList,it);
					continue;
				}
				int ret=send((*it).clientFd,writebuf,readlen,0);
				(*it).clientData[1]+=ret;
				if(ret<=0)
				{
					closeFd(sockList,it);
					continue;
				}
			}
		}
			
		// do  exception
		for(it=sockList.begin();it!=sockList.end();it++)
		{
            if(nready==0)
    		{
    			break;
    		}
			if(FD_ISSET((*it).clientFd,&efdset))
			{
					closeFd(sockList,it);
					continue;
			}
			if(FD_ISSET((*it).remoteFd,&efdset))
			{
					closeFd(sockList,it);
					continue;
			}
		}
			
	}
	END:
	close(client_listen_sockfd);
    close(server_sockfd);
    return 0;
}


