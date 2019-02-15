
#ifndef __COMSTRUCT_H__
#define __COMSTRUCT_H__
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




typedef struct socketPair
{
    int clientFd;
    int zopenFd;
    int clientData[2];
    int zopenData[2];
    socketPair();
    socketPair(const socketPair &_A);
    socketPair& operator=(const socketPair &_A);
}SPair;

typedef std::vector<SPair> VSP;
typedef std::vector<SPair>::iterator VSPI;
typedef std::vector<int> VSOCK;
typedef std::vector<int>::iterator VSOCKI;

int max(int a,int b);
int check_is_ip(const char*str);
int hostname_to_ip(const char * hostname , char* ip);
VSPI CloseFd(VSP &A,VSPI it,const char * file,int line=0);

int createConnect(const char *host,int port);
#define closeFd(a,b) CloseFd(a,b,__FILE__,__LINE__)

#endif


