
/*!
* \file zlibnet.h
* \brief 封装的网络通讯类
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/
#ifndef __ZLIBNET_H__
#define __ZLIBNET_H__

#include "global.h"
namespace baseservice
{
 
/**
* \brief 网络地址,为了解决windows/linux 网络地址不通用问题
*/
struct zlibNetAddr
{

    unsigned int ip;
    unsigned short port;
};

/**
* \brief tcp的客户端,析构自动关闭socket
*/
class tcpClient
{

  public:
    tcpClient();
    ~tcpClient();
    int init();
    int release();
    int connect(zlibNetAddr);
    int connect(const char *ip, const char *port);
    int connect(const char *ip, unsigned short port);
    int send(const void *buf, int size, int flag);
    int recv(void *buf, int bufSize, int flag);
    int fd;
};

/**
* \brief tcp的服务端,运行时绑定端口,析构自动关闭socket
*/
class tcpServer
{
  public:
    tcpServer();
    ~tcpServer();
    int init();
    int bind(zlibNetAddr);
    int bind(const char *ip, const char *port);
    int bind(const char *ip, unsigned short port);
    int release();
    int listen(int maxNum);
    int accept(zlibNetAddr *);
    int accept();
    int fd;
};

/**
* \brief udp通讯,可以绑定端口,也可以直接发送消息
*/
class udpSocket
{
  public:
    udpSocket();
    ~udpSocket();
    int init();
    int bind(zlibNetAddr);
    int bind(const char *ip,const unsigned short port);
    int bind(const char *ip,const char * port);

    int recv(void *, int, zlibNetAddr*);
    int recv(void *recvbuf, int buflen);

    int send(void *, int, zlibNetAddr);
    int send(void *sendbuf, int len, const char *ip, const unsigned short port);
    int send(void *sendbuf, int len, const char *ip, const char *port);
    int fd;
};

}; // namespace baseservice

#endif
