
#include "com_struct.h"

using namespace baseservice;
using namespace std;
struct global_config_st
{
    int server_port;
    char server_ip[1024];
    int zopen_port;
    char zopen_ip[1024];
} g_config;

inline int max(int a, int b)
{
    return a > b ? a : b;
}

inline int min(int a, int b)
{
    return a < b ? a : b;
}

int createZopenConnect(VSOCK &list, int needSockNum, const char *host, int port)
{
    char remote_ipaddr[1024];
    int num = 0;
    while (num < needSockNum)
    {
        int sock = createConnect(host, port);
        if (sock == 0)
        {
            return num;
        }
        list.push_back(sock);
        num++;
    }
    return num;
}
void fillZopenSock(VSOCK &zopenList, int num, const char *ip, int port)
{
    int res = zopenList.size();
    int ret = res;
    if (ret == num)
    {
        return;
    }
    if (ret > num)
    {
        while (ret > num)
        {
            ret = zopenList.size();
            zopenList.erase(zopenList.begin());
            SYS_LOG(ZLOGINFO, "zopenList current %d \n", ret);
        }
    }
    else
    {
        int needSock = num - ret;
        ret = createZopenConnect(zopenList, needSock, ip, port);
        if (needSock == ret)
        {
            SYS_LOG(ZLOGINFO, "createRConnect success need %d created:%d\n", needSock, ret);
        }
        else
        {
            SYS_LOG(ZLOGINFO, "createRConnect fail need %d created:%d\n", needSock, ret);
        }
    }
    SYS_LOG(ZLOGINFO, "zopenList current %d need %d created %d\n", res, num - res, ret);
}

int makePairFromZSocket(VSOCK &zopenList, fd_set &rfdset, VSP &connectedList)
{
    VSOCKI vit;
    VSPI it;
    int changeNum = 0;
    // ZSocket recv data ,make pair
    for (vit = zopenList.begin(); vit != zopenList.end(); vit++)
    {
        if (FD_ISSET((*vit), &rfdset))
        {
            BDS bds;
            int msgRet = recv(*vit, &bds, sizeof(BDS), 0);
            if (msgRet == sizeof(BDS))
            {
                unsigned short type;
                unsigned int length;
                bds.readHeader(type, length);
                if (type == DataTypeMsg::HEARTBEAT)
                {
                    SYS_LOG(ZLOGINFO, "socket %d recv heartBeat\n", *vit);
                    continue;
                }
                else if (type == DataTypeMsg::STARTDATA)
                {
                    int clientSock = *vit;
                    int localSock = createConnect(g_config.server_ip, g_config.server_port);
                    SYS_LOG(ZLOGINFO, "connect client localSock %d connectedList size %d\tzopenList size %d\n", localSock, connectedList.size(), zopenList.size());
                    if (localSock != 0)
                    {
                        SPair unit;
                        unit.clientFd = clientSock;
                        unit.zopenFd = localSock;
                        VSOCKI vitt = vit++;
                        zopenList.erase(vitt);
                        connectedList.push_back(unit);
                        int zsockNum = max(5, connectedList.size() * 2 + 5);
                        zsockNum = min(zsockNum, 100);
                        SYS_LOG(ZLOGINFO, "connectedList sock %d <---> %d\n", clientSock, localSock);
                        changeNum++;
                        break;
                    }
                    else //connect local server fail
                    {
                        goto END;
                    }
                }
                else
                {
                    goto END;
                }
            }
            else
            {
            END:
                SYS_LOG(ZLOGINFO, "socket %d recv err msg close\n", *vit);
                VSOCKI vitt = vit++;
                close(*vitt);
                zopenList.erase(vitt);
                vit = zopenList.begin();
            }
        }
    }
    fillZopenSock(zopenList, 5, g_config.zopen_ip, g_config.zopen_port);
    return changeNum;
}
int exchangeData(VSP &connectedList, fd_set &rfdset)
{
    VSPI it;
    int ret = 0;
    char writebuf[10240];
    int readlen = 0;
    for (it = connectedList.begin(); it != connectedList.end();)
    {
        if (connectedList.size() <= 0)
            return 0;
        if (FD_ISSET((*it).clientFd, &rfdset))
        {
            readlen = recv((*it).clientFd, writebuf, 10240, 0);
            (*it).clientData[0] += readlen;
            if (readlen <= 0)
            {
                VSPI itt = it;
                it++;
                closeFd(connectedList, itt);
                break;
            }
            int ret = send((*it).zopenFd, writebuf, readlen, 0);
            (*it).zopenData[1] += ret;
            if (ret <= 0)
            {
                VSPI itt = it;
                it++;
                it = closeFd(connectedList, itt);
                break;
            }
        }
        if (FD_ISSET((*it).zopenFd, &rfdset))
        {
            // SYS_LOG(ZLOGINFO,"zopenFd read data connectedList size %d\tzopenList size %d\n",connectedList.size()
            // ,zopenList.size());

            readlen = recv((*it).zopenFd, writebuf, 10240, 0);
            (*it).zopenData[0] += readlen;
            if (readlen <= 0)
            {
                VSPI itt = it;
                it++;
                it = closeFd(connectedList, itt);
                break;
            }
            int ret = send((*it).clientFd, writebuf, readlen, 0);
            (*it).clientData[1] += ret;
            if (ret <= 0)
            {
                VSPI itt = it;
                it++;
                it = closeFd(connectedList, itt);
                break;
            }
        }
        ++it;
    }
    return 0;
}
int getMaxIndex(VSP &connectedList, VSOCK &zopenList, fd_set &rfdset, fd_set &efdset)
{
    int maxIndex = 0;
    VSPI it;
    VSOCKI vit;

    for (it = connectedList.begin(); it != connectedList.end(); ++it)
    {
        FD_SET((*it).clientFd, &efdset);
        FD_SET((*it).clientFd, &rfdset);
        FD_SET((*it).zopenFd, &efdset);
        FD_SET((*it).zopenFd, &rfdset);
        maxIndex = max(maxIndex, (*it).clientFd);
        maxIndex = max(maxIndex, (*it).zopenFd);
    }
    for (vit = zopenList.begin(); vit != zopenList.end(); ++vit)
    {
        FD_SET((*vit), &efdset);
        FD_SET((*vit), &rfdset);
        maxIndex = max(maxIndex, (*vit));
    }
    maxIndex += 1;
    return maxIndex;
}
int off_main(int argc, char *argv[])
{
    VSOCK zopenList;
    VSP connectedList;
    int zsockNum = 5;
    int MaxConnectNum = 100;

    struct sockaddr_in server_sockaddr;
    struct sockaddr_in zopen_sockaddr;
    connectedList.clear();
    zopenList.clear();
    if (argc < 5)
    {
        SYS_LOG(ZLOGINFO, "need server ip and port zopen ip and port\n", argc);
        return 0;
    }
    strcpy(g_config.server_ip, argv[1]);
    g_config.server_port = atoi(argv[2]);
    strcpy(g_config.zopen_ip, argv[3]);
    g_config.zopen_port = atoi(argv[4]);
    SYS_LOG(ZLOGINFO, "remote port [%s:%d] connect to [%s:%d]\n", g_config.server_ip, g_config.server_port, g_config.zopen_ip, g_config.zopen_port);

    char remote_ipaddr[1024];
    char local_ipaddr[1024];

    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(g_config.server_port);
    server_sockaddr.sin_addr.s_addr = inet_addr(remote_ipaddr);

    zopen_sockaddr.sin_family = AF_INET;
    zopen_sockaddr.sin_port = htons(g_config.zopen_port);
    zopen_sockaddr.sin_addr.s_addr = inet_addr(local_ipaddr);

    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    struct timeval tv;
    fd_set rfdset;
    fd_set efdset;
    int ret = 0;
    char writebuf[10240];
    int readlen = 0;
    while (1)
    {
        FD_ZERO(&rfdset);
        FD_ZERO(&efdset);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int maxIndex = 0;
        VSPI it;
        VSOCKI vit;
        fillZopenSock(zopenList, zsockNum, g_config.zopen_ip, g_config.zopen_port);

        maxIndex = getMaxIndex(connectedList, zopenList, rfdset, efdset);
        int nready = 0;
        if (zsockNum == zopenList.size())
        {
            nready = select(maxIndex, &rfdset, NULL, &efdset, 0);
        }
        else
        {
            nready = select(maxIndex, &rfdset, NULL, &efdset, &tv);
        }

        if (nready == 0)
        {

            usleep(10);
            continue;
        }
        exchangeData(connectedList, rfdset);
        // ZSocket recv data ,make pair
        makePairFromZSocket(zopenList, rfdset, connectedList);
        //catch exception
        if (closeExpPair(connectedList, efdset))
        {
            SYS_LOG(ZLOGINFO, "disconnect client current connectedList size %d\tzopenList size %d\n", connectedList.size(), zopenList.size());
        }
        if (closeExpZSocket(zopenList, efdset))
        {
            SYS_LOG(ZLOGINFO, "connect client current connectedList size %d\tzopenList size %d\n", connectedList.size(), zopenList.size());
        }
    }
END:
    //close(server_sockfd);
    return 0;
}
