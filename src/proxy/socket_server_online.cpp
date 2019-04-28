#include "com_struct.h"

using namespace baseservice;
using namespace std;

inline int max(int a, int b)
{
    return a > b ? a : b;
}

int exchangeData(VSP &connectedList, VSOCK &zopenList, VSOCK &clientList, fd_set &rfdset)
{
    VSPI it;
    int ret = 0;
    char writebuf[10240];
    int readlen = 0;
    for (it = connectedList.begin(); it != connectedList.end();)
    {

        if (FD_ISSET((*it).clientFd, &rfdset))
        {
            readlen = recv((*it).clientFd, writebuf, 10240, 0);
            (*it).clientData[0] += readlen;
            if (readlen <= 0)
            {
                it = closeFd(connectedList, it);
                SYS_LOG(ZLOGINFO, "client disconnected and put in connectedList size %d clientList size %d mList size %d\n", connectedList.size(), clientList.size(), zopenList.size());
                continue;
            }
            int ret = send((*it).zopenFd, writebuf, readlen, 0);
            (*it).zopenData[1] += ret;
            if (ret <= 0)
            {
                it = closeFd(connectedList, it);
                SYS_LOG(ZLOGINFO, "client disconnected and put in connectedList size %d clientList size %d mList size %d\n", connectedList.size(), clientList.size(), zopenList.size());
                continue;
            }
        }
        if (FD_ISSET((*it).zopenFd, &rfdset))
        {
            readlen = recv((*it).zopenFd, writebuf, 10240, 0);
            (*it).zopenData[0] += readlen;
            if (readlen <= 0)
            {
                it = closeFd(connectedList, it);
                SYS_LOG(ZLOGINFO, "client disconnected and put in connectedList size %d clientList size %d mList size %d\n", connectedList.size(), clientList.size(), zopenList.size());
                continue;
            }
            int ret = send((*it).clientFd, writebuf, readlen, 0);
            (*it).clientData[1] += ret;
            if (ret <= 0)
            {

                it = closeFd(connectedList, it);
                SYS_LOG(ZLOGINFO, "client disconnected and put in connectedList size %d clientList size %d mList size %d\n", connectedList.size(), clientList.size(), zopenList.size());
                continue;
            }
        }
        it++;
    }
    return 0;
}
int on_main(int argc, char *argv[])
{
    VSP connectedList;
    VSOCK clientList;
    VSOCK zopenList;
    int listen_port;
    int zopen_port;
    connectedList.clear();
    clientList.clear();
    zopenList.clear();

    int maxClientNum = 100;

    if (argc < 3)
    {
        SYS_LOG(ZLOGINFO, "need listen port zopen port\n", argc);
        return 0;
    }
    listen_port = atoi(argv[1]);
    zopen_port = atoi(argv[2]);

    int zopen_fd = socket(AF_INET, SOCK_STREAM, 0);
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(zopen_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));

    struct sockaddr_in zopen_ipaddr;
    struct sockaddr_in listen_ipaddr;

    listen_ipaddr.sin_family = AF_INET;
    listen_ipaddr.sin_port = htons(listen_port);
    listen_ipaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    zopen_ipaddr.sin_family = AF_INET;
    zopen_ipaddr.sin_port = htons(zopen_port);
    zopen_ipaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(zopen_fd, (struct sockaddr *)&zopen_ipaddr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("client_listen_sockfd");
        exit(1);
    }

    if (bind(listen_fd, (struct sockaddr *)&listen_ipaddr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("server_sockfd ");
        exit(1);
    }

    if (listen(listen_fd, 1024) == -1)
    {
        perror("client_listen_sockfd");
        exit(1);
    }
    if (listen(zopen_fd, 1024) == -1)
    {
        perror("server_sockfd");
        exit(1);
    }
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);

    struct timeval tv;
    fd_set rfdset;
    fd_set efdset;
    int tmpSocket = 0;
    char writebuf[10240];
    int readlen = 0;
    while (1)
    {
        FD_ZERO(&rfdset);
        FD_ZERO(&efdset);
        FD_SET(listen_fd, &rfdset);
        FD_SET(listen_fd, &efdset);
        FD_SET(zopen_fd, &rfdset);
        FD_SET(zopen_fd, &efdset);
        int maxIndex = max(zopen_fd, listen_fd);
        VSPI it;
        VSOCKI vit;
        for (it = connectedList.begin(); it != connectedList.end(); it++)
        {
            FD_SET((*it).clientFd, &efdset);
            FD_SET((*it).clientFd, &rfdset);
            FD_SET((*it).zopenFd, &efdset);
            FD_SET((*it).zopenFd, &rfdset);
            maxIndex = max(maxIndex, (*it).clientFd);
            maxIndex = max(maxIndex, (*it).zopenFd);
        }

        for (vit = zopenList.begin(); vit != zopenList.end(); vit++)
        {
            FD_SET((*vit), &efdset);
            FD_SET((*vit), &rfdset);
            maxIndex = max(maxIndex, (*vit));
        }

        for (vit = clientList.begin(); vit != clientList.end(); vit++)
        {
            FD_SET((*vit), &efdset);
            FD_SET((*vit), &rfdset);
            maxIndex = max(maxIndex, (*vit));
        }
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        maxIndex += 1;
        //  int nready = select(maxIndex, &rfdset, NULL, &efdset, &tv);
        int nready = select(maxIndex, &rfdset, NULL, &efdset, 0);
        if (nready == 0)
        {
            //SYS_LOG(ZLOGINFO,"select timeout %d\n",nready);
            usleep(10);
            continue;
        }
        //client_listen_sockfd  等待客户端连接
        //
        if (FD_ISSET(listen_fd, &rfdset))
        {
            int client = accept(listen_fd, (struct sockaddr *)&client_addr, &length);

            if (zopenList.size() > 0)
            {
                SPair unit;
                unit.clientFd = client;
                VSOCKI it = zopenList.begin();
                unit.zopenFd = *it;
                it = zopenList.erase(it);
                connectedList.push_back(unit);
                SYS_LOG(ZLOGINFO, "new client connected %d and put in connectedList size %d clientList size %d mList size %d\n", nready, connectedList.size(), clientList.size(), zopenList.size());
            }
            else
            {
                if (clientList.size() + connectedList.size() > 100)
                {
                    close(client);
                    SYS_LOG(ZLOGINFO, "new client connected %d ,but client num is up and connectedList size %d put in clientList size %d mList size %d\n", nready, connectedList.size(), clientList.size(), zopenList.size());
                }
                else
                {
                    clientList.push_back(client);
                    SYS_LOG(ZLOGINFO, "new client connected %d and connectedList size %d put in clientList size %d mList size %d\n", nready, connectedList.size(), clientList.size(), zopenList.size());
                }
            }
        }
        //client_listen_sockfd  等待内网程序连接
        if (FD_ISSET(zopen_fd, &rfdset))
        {
            int remoteSock = accept(zopen_fd, (struct sockaddr *)&client_addr, &length);
            if (clientList.size() > 0)
            {
                SPair unit;
                unit.zopenFd = remoteSock;
                VSOCKI it = clientList.begin();
                unit.clientFd = *it;
                it = clientList.erase(it);
                SYS_LOG(ZLOGINFO, "new client connected %d and put in connectedList size %d clientList size %d mList size %d\n", nready, connectedList.size(), clientList.size(), zopenList.size());
            }
            else
            {
                zopenList.push_back(remoteSock);
                SYS_LOG(ZLOGINFO, "new client connected %d and connectedList size %d clientList size %d put in mList size %d\n", nready, connectedList.size(), clientList.size(), zopenList.size());
            }
        }

        ///read and write data
        exchangeData(connectedList, zopenList, clientList, rfdset);

        for (vit = zopenList.begin(); vit != zopenList.end(); vit++)
        {
            if (FD_ISSET(*vit, &rfdset))
            {
                readlen = recv((*vit), writebuf, 10240, 0);
                if (readlen <= 0)
                {
                    VSOCKI vitt = vit++;
                    zopenList.erase(vitt);
                    SYS_LOG(ZLOGINFO, "zopen disconnected %d and put in connectedList size %d clientList size %d mList size %d\n", nready, connectedList.size(), clientList.size(), zopenList.size());
                    continue;
                }
            }
        }
        for (vit = clientList.begin(); vit != clientList.end(); vit++)
        {
            readlen = recv((*vit), writebuf, 10240, 0);
            (*vit) += readlen;
            if (readlen <= 0)
            {
                VSOCKI vitt = vit++;
                zopenList.erase(vitt);
                SYS_LOG(ZLOGINFO, "client disconnected %d and put in socklist size %d clientList size %d mList size %d\n", nready, connectedList.size(), clientList.size(), zopenList.size());
                continue;
            }
        }
        // catch exception
        if (closeExpPair(connectedList, efdset))
        {
            SYS_LOG(ZLOGINFO, "disconnect client current connectedList size %d\tzopenList size %d\n", connectedList.size(), zopenList.size());
        }
    }
END:
    close(listen_fd);
    close(zopen_fd);
    return 0;
}
