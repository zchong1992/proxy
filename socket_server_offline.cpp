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
#define MYPORT  8887
#define QUEUE   20
int g_local_port;
int g_remote_port;
char g_local_ip[1024];
char g_remote_ip[1024];
using namespace baseservice;
int check_is_ip(const char*str)
{
	int len=strlen(str);
	int index=0;
	for(index=0;index<len;index++)
	{
		if(str[index]='.')
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
	znlog::getInstance()->set_log_file("log.txt");
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

void *socket_client_thread(void*Para)
{
	int client_fd=*(int*)Para;
	free(Para);
	int remote_fd = socket(AF_INET,SOCK_STREAM, 0);

    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(g_remote_port);
	char remote_ipaddr[1024];
	char local_ipaddr[1024];
	char readbuf[1024];
	int readlen=0;
	int sendret=0;
	int nready =0;
	int peer_size[2][2]={{0}};
	
    struct timeval tv;
    fd_set fdset;
    fd_set efdset;
	if(!check_is_ip(g_remote_ip))
	{
		if(!hostname_to_ip(g_remote_ip,remote_ipaddr))
		{
		    SYS_LOG(INFO,"resolved %s to %s fail\n",g_remote_ip,remote_ipaddr);
			goto END;
		}
	}
	else
	{
		strcpy(remote_ipaddr,g_remote_ip);
	}
	if(!check_is_ip(g_local_ip))
	{
		if(!hostname_to_ip(g_local_ip,local_ipaddr))
		{
		    SYS_LOG(INFO,"resolved %s to %s fail\n",g_local_ip,local_ipaddr);
			goto END;
		}
	}
	else
	{
		strcpy(local_ipaddr,g_local_ip);
	}
    //SYS_LOG(INFO,"resolved %s to %s\n",g_remote_ip,remote_ipaddr);
	
    remote_addr.sin_addr.s_addr = inet_addr(remote_ipaddr);
	
	if(connect(remote_fd,(struct sockaddr *)&remote_addr,sizeof(struct sockaddr))<0)  
    {  
        SYS_LOG(INFO,"connect %s:%d fail\n",g_remote_ip,g_remote_port);
        goto END;
    }
    //SYS_LOG(INFO,"connected %s:%d \n",g_remote_ip,g_remote_port);
	while(1)
	{
	    FD_ZERO(&fdset);
	    FD_SET(client_fd, &fdset);
		FD_SET(remote_fd, &fdset);
	    FD_ZERO(&efdset);
	    FD_SET(client_fd, &efdset);
		FD_SET(remote_fd, &efdset);
     ;
		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		int maxIndex=(client_fd>remote_fd?client_fd:remote_fd )+1;
		nready = select(maxIndex, &fdset, NULL, &efdset, &tv);
		if(nready==0)
		{
			//SYS_LOG(INFO,"select timeout %d\n",nready);
			
			continue;
		}
		if(FD_ISSET(client_fd,&fdset))
		{
        	//SYS_LOG(INFO,"FD_ISSET(client_fd,&fdset) %d\n",nready);
			readlen=recv(client_fd, readbuf, 1024,0);
			peer_size[0][0]+=readlen;
			if(readlen<=0)
				goto END;
			sendret=send(remote_fd,readbuf,readlen,0);
			peer_size[1][1]+=sendret;
			if(sendret<=0)
				goto END;
		}
		if(FD_ISSET(remote_fd,&fdset))
		{
        	//SYS_LOG(INFO,"FD_ISSET(remote_fd,&fdset) %d\n",nready);
			readlen=recv(remote_fd, readbuf, 1024,0);
			peer_size[1][0]+=sendret;
			if(readlen<=0)
				goto END;
			int ret=send(client_fd,readbuf,readlen,0);
			peer_size[0][1]+=sendret;
			if(ret<=0)
				goto END;
		}
        
        if(FD_ISSET(client_fd,&efdset))
		{
			goto END;
		}
		if(FD_ISSET(remote_fd,&efdset))
		{
        	
			goto END;
		}
	}
END:
	
    SYS_LOG(INFO,"END local recv/send %d/%d remote recv/send %d/%d\n"
		,peer_size[0][0]
		,peer_size[0][1]
		,peer_size[1][0]
		,peer_size[1][1]);
	if(client_fd!=0)
		close(client_fd);
	if(remote_fd!=0)
		close(remote_fd);
	
	
	return 0;
}

int main(int argc,char * argv[])
{

	initLog();
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
	
	if(!hostname_to_ip(g_remote_ip,remote_ipaddr))
	{
        SYS_LOG(INFO,"resolved %s to %s fail\n",g_remote_ip,remote_ipaddr);
		goto END;
	}

    int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
	int opt=1;
	setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR,(void *)&opt, sizeof(opt));

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(g_local_port);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind");
        exit(1);
    }

    if(listen(server_sockfd,QUEUE) == -1)
    {
        perror("listen");
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
	while(1)
	{
		int* conn =(int*)malloc(sizeof(int));
		*conn = accept(server_sockfd, (struct sockaddr*)&client_addr, &length);
		SYS_LOG(INFO,"new client connected\n");
		createthread(socket_client_thread,conn);
	}
    close(server_sockfd);
    return 0;
}
