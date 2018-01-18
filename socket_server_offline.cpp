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
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //for exit(0);
#include<sys/socket.h>
#include<errno.h> //For errno - the error number
#include<netdb.h> //hostent
#include<arpa/inet.h>

#include "libbaseservice.h"

#include <vector>
#include "libbaseservice.h"
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

int g_local_port;
int g_remote_port;
char g_local_ip[1024];
char g_remote_ip[1024];
using namespace baseservice;


inline int max(int a,int b)
{
	return a>b?a:b;
}

int check_is_ip(const char*str)
{
	int len=strlen(str);
	int index=0;
	for(index=0;index<len;index++)
	{
		if(str[index]=='.')
			continue;
		if(str[index]>='0'&&str[index]<='9')
			continue;
		return 0;
	}
	return 1;
}
void initLog()
{
	znlog::getInstance()->Init();
	znlog::getInstance()->set_level(INFO,INFO);
	znlog::getInstance()->set_log_file("server_offline_log.txt");
}
int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
          
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
  
    addr_list = (struct in_addr **) he->h_addr_list;
      
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 1;
    }
      
    return 0;
}
int createRConnect(VSOCK & list,int needSockNum,const char *host,int port)
{
	char remote_ipaddr[1024];
	if(check_is_ip(host))
    {   
    	if(!hostname_to_ip(g_remote_ip,remote_ipaddr))
    	{
            SYS_LOG(INFO,"resolved %s to %s fail\n",g_remote_ip,remote_ipaddr);
    		return 0;
    	}
    }


}
int createLConnect(const char *host,int port)
{
	char remote_ipaddr[1024];
	if(!hostname_to_ip(host,remote_ipaddr))
	{
        SYS_LOG(INFO,"resolved %s to %s fail\n",g_remote_ip,remote_ipaddr);
		return 0;
	}
}
int main(int argc,char * argv[])
{

	initLog();
    VSOCK resList;
    VSP connectList;
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
		for(it=resList.begin();it!=resList.end();it++)
		{
		    FD_SET((*it), &efdset);
		    FD_SET((*it), &rfdset);
			maxIndex=max(maxIndex,(*it));
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
		int nready = select(maxIndex, &rfdset, NULL, &efdset, &tv);
		if(nready==0)
		{
			//SYS_LOG(INFO,"select timeout %d\n",nready);
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
		    if(FD_ISSET((*vit).clientFd,&rfdset))
			{
			    int clientSock=*vit;
                int localSock=createLConnect(g_local_ip,g_local_port);
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
    close(server_sockfd);
    return 0;
}

