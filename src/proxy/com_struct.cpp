
#include "com_struct.h"

using namespace baseservice;
using namespace std;

void BaseDataStruct::readHeader(unsigned short &type, unsigned int &size)
{
    memcpy(&type, &this->type, sizeof(unsigned short));
    memcpy(&size, &this->length, sizeof(unsigned int));
    type = ntohs(type);
    size = ntohl(size);
}
void BaseDataStruct::writeHeader(unsigned short type, unsigned int size)
{
    type = ntohs(type);
    size = ntohl(size);
    memcpy(&this->type, &type, sizeof(unsigned short));
    memcpy(&this->length, &size, sizeof(unsigned int));
}
void BaseDataStruct::isVaild()
{
}
socketPair::socketPair()
{
    memset(this, 0, sizeof(socketPair));
}
socketPair::socketPair(const socketPair &_A)
{
    //memcpy(this,&_A,sizeof(socketPair));
    this->clientFd = _A.clientFd;
    this->zopenFd = _A.zopenFd;
    this->clientData[0] = _A.clientData[0];
    this->clientData[1] = _A.clientData[1];
    this->zopenData[0] = _A.zopenData[0];
    this->zopenData[1] = _A.zopenData[1];
}
socketPair &socketPair::operator=(const socketPair &_A)
{
    memcpy(this, &_A, sizeof(socketPair));
    return *this;
}

int max(int a, int b)
{
    return a > b ? a : b;
}

int check_is_ip(const char *str)
{
    int len = strlen(str);
    int index = 0;
    for (index = 0; index < len; index++)
    {
        if (str[index] == '.')
            continue;
        if (str[index] >= '0' && str[index] <= '9')
            continue;
        return 0;
    }
    return 1;
}

VSPI CloseFd(VSP &A, VSPI it, const char *file, int line)
{
    close((*it).clientFd);
    close((*it).zopenFd);
    SYS_LOG(ZLOGINFO, "close fd pair,(%d,%d,%d,%d)at %s:%d\n", (*it).clientData[0], (*it).clientData[1], (*it).zopenData[0], (*it).zopenData[1], file, line);
    it = A.erase(it);
    return it;
}
int hostname_to_ip(const char *hostname, char *ip)
{
    if (check_is_ip(hostname))
    {
        strcpy(ip, hostname);
        return 1;
    }
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname(hostname)) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return 1;
    }

    return 0;
}

int createConnect(const char *host, int port)
{
    char ipaddr[1024];
    if (!hostname_to_ip(host, ipaddr))
    {
        SYS_LOG(ZLOGINFO, "resolved %s to %s fail\n", host, ipaddr);
        return 0;
    }

    struct sockaddr_in sockaddr;

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = inet_addr(ipaddr);
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    int ret = connect(sock, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));
    if (ret < 0)
    {
        SYS_LOG(ZLOGINFO, "connect fail ret %d\n", ret);
        close(sock);
        return 0;
    }
    return sock;
}

int closeExpPair(VSP &connectedList, fd_set &efdset)
{
    VSPI it;
    int closeNum = 0;
    for (it = connectedList.begin(); it != connectedList.end(); it++)
    {
        if (FD_ISSET((*it).zopenFd, &efdset))
        {
            VSPI itt = it++;
            closeFd(connectedList, itt);
            closeNum++;
        }
        if (FD_ISSET((*it).clientFd, &efdset))
        {
            VSPI itt = it++;
            closeFd(connectedList, itt);
            closeNum++;
        }
    }
    return closeNum;
}

int closeExpZSocket(VSOCK &zopenList, fd_set &efdset)
{
    VSPI it;
    int closeNum = 0;
    VSOCKI vit;
    for (vit = zopenList.begin(); vit != zopenList.end();)
    {
        if (FD_ISSET((*vit), &efdset))
        {
            VSOCKI vitt = vit++;
            close(*vitt);
            zopenList.erase(vitt);
            closeNum++;
        }
        else
        {
            vit++;
        }
    }
    return closeNum;
}