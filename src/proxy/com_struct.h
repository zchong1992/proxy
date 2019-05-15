
#ifndef __COMSTRUCT_H__
#define __COMSTRUCT_H__
#include "libbaseservice.h"

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
#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //for exit(0);
#include <sys/socket.h>
#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent
#include <arpa/inet.h>
#include <vector>

#define closeFd(a, b) CloseFd(a, b, __FILE__, __LINE__)

#define MYPORT 8887
#define QUEUE 20
#define BUFFER_SIZE 1024
typedef struct socketPair
{
    int clientFd;
    int zopenFd;
    int clientData[2];
    int zopenData[2];
    socketPair();
    socketPair(const socketPair &_A);
    socketPair &operator=(const socketPair &_A);
} SPair;
class DataTypeMsg
{
public:
    enum
    {
        HEARTBEAT = 100,
        STARTDATA
    };
};
typedef class BaseDataStruct
{
    char magic[4];
    unsigned short type;
    unsigned int length;

public:
    void readHeader(unsigned short &type, unsigned int &size);
    void writeHeader(unsigned short type, unsigned int size);
    void isVaild();
} BDS;

typedef std::list<SPair> VSP;
typedef std::list<SPair>::iterator VSPI;
typedef std::list<int> VSOCK;
typedef std::list<int>::iterator VSOCKI;

int max(int a, int b);
int check_is_ip(const char *str);
int hostname_to_ip(const char *hostname, char *ip);
VSPI CloseFd(VSP &A, VSPI it, const char *file, int line = 0);

int createConnect(const char *host, int port);
int closeExpPair(VSP &connectedList, fd_set &efdset);
int closeExpZSocket(VSOCK &zopenList, fd_set &efdset);

#endif
