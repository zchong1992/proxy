

#include "zlibnet.h"
#include "log.h"
#include <string>
using namespace std;
namespace baseservice
{

string GetIpFromZlibNetAddr(zlibNetAddr addr)
{
	string ip = inet_ntoa(*(struct in_addr *)&addr.ip);
	return ip;
}
zlibNetAddr GetZlibNetAddrFromIpAndPort(const char *ip, const unsigned short port)
{
	zlibNetAddr addr;
	addr.ip = inet_addr(ip);
	addr.port = ntohs(port);
	return addr;
}
zlibNetAddr GetZlibNetAddrFromIpAndPort(const char *ip, const char *port)
{
	unsigned short mport = atoi(port);
	return GetZlibNetAddrFromIpAndPort(ip, mport);
}
SOCKADDR_IN GetSOCKADD_INFromZlibNetAddr(zlibNetAddr addr)
{

#ifdef ZLINUX
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.s_addr = addr.ip;
	addrSrv.sin_port = addr.port;
	addrSrv.sin_family = AF_INET;
#endif
#ifdef ZWINDOWS
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = addr.port;
	addrSrv.sin_addr.S_un.S_addr = addr.ip;
#endif
	return addrSrv;
}
tcpClient::tcpClient() { fd = 0; }
tcpClient::~tcpClient() { release(); }
int tcpClient::init()
{

	fd = socket(AF_INET, SOCK_STREAM, 0);
	return fd;
}
int tcpClient::release()
{

	if (fd != 0)
#ifdef ZWINDOWS
		::closesocket(fd);
#else
		::close(fd);
#endif
	fd = 0;
	return 0;
}
int tcpClient::connect(zlibNetAddr addr)
{
	struct sockaddr_in serAddr=GetSOCKADD_INFromZlibNetAddr(addr);
	return ::connect(fd, (sockaddr *)&serAddr, sizeof(serAddr));
}
int tcpClient::connect(const char *ip, const char *port)
{
	zlibNetAddr addr;
	addr.ip = inet_addr(ip);
	addr.port = ntohs(atoi(port));
	return connect(addr);
}
int tcpClient::connect(const char *ip, unsigned short port)
{
	zlibNetAddr addr;
	addr.ip = inet_addr(ip);
	addr.port = ntohs(port);
	return connect(addr);
}
int tcpClient::send(const void *buf, int size, int flag)
{

	return ::send(fd, (char *)buf, size, flag);
}
int tcpClient::recv(void *buf, int bufSize, int flag)
{

	return ::recv(fd, (char *)buf, bufSize, flag);
}
tcpServer::tcpServer() { fd = 0; }
tcpServer::~tcpServer() { release(); }
int tcpServer::init()
{

	fd = socket(AF_INET, SOCK_STREAM, 0);
	int flag = 1, len = sizeof(int);
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, len) ==
		-1)
	{
		return -1;
	}
	return fd;
}
int tcpServer::bind(zlibNetAddr addr)
{
	SOCKADDR_IN addrSrv=GetSOCKADD_INFromZlibNetAddr(addr);
	return ::bind(fd, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
}
int tcpServer::bind(const char *ip, const char *port)
{
	zlibNetAddr addr;
	addr.ip = inet_addr(ip);
	addr.port = ntohs(atoi(port));
	return bind(addr);
}
int tcpServer::bind(const char *ip, unsigned short port)
{
	zlibNetAddr addr;
	addr.ip = inet_addr(ip);
	addr.port = ntohs(port);
	return bind(addr);
}
int tcpServer::release()
{
	if (fd != 0)
#ifdef ZWINDOWS
		::closesocket(fd);
#else
		::close(fd);
#endif
	fd = 0;
	return 0;
}
int tcpServer::listen(int maxNum) { return ::listen(fd, maxNum); }
int tcpServer::accept(zlibNetAddr *clientAddr)
{
#ifdef ZLINUX
	SOCKADDR_IN addrSrv;
	int size = sizeof(SOCKADDR);
	int ret = ::accept(fd, (SOCKADDR *)&addrSrv, (socklen_t *)&size);
	clientAddr->ip = addrSrv.sin_addr.s_addr;
	clientAddr->port = addrSrv.sin_port;
	return ret;
#endif
#ifdef ZWINDOWS
	SOCKADDR_IN addrSrv;
	int size = sizeof(SOCKADDR);
	int ret = ::accept(fd, (SOCKADDR *)&addrSrv, (socklen_t *)&size);
	clientAddr->ip = addrSrv.sin_addr.S_un.S_addr;
	clientAddr->port = addrSrv.sin_port;
	return ret;
#endif
}
int tcpServer::accept()
{
	zlibNetAddr clientAddr;
	return accept(&clientAddr);
}

udpSocket::udpSocket()
{
}
udpSocket::~udpSocket()
{
}
int udpSocket::init()
{
	if (fd != 0)
#ifdef ZWINDOWS
		::closesocket(fd);
#else
		::close(fd);
#endif
	fd = 0;
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd <= 0)
	{
		return fd;
	}
	return fd;
}
int udpSocket::bind(zlibNetAddr addr)
{
	SOCKADDR_IN addrSrv=GetSOCKADD_INFromZlibNetAddr(addr);
	return ::bind(fd, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
}

int udpSocket::bind(const char *ip, const unsigned short port)
{
	zlibNetAddr addr = GetZlibNetAddrFromIpAndPort(ip, port);
	return bind(addr);
}
int udpSocket::bind(const char *ip, const char *port)
{
	zlibNetAddr addr = GetZlibNetAddrFromIpAndPort(ip, port);
	return bind(addr);
}
int udpSocket::recv(void *recvbuf, int buflen, zlibNetAddr*addr)
{
	SOCKADDR_IN peeraddr=GetSOCKADD_INFromZlibNetAddr(*addr);
	int peerlen = sizeof(SOCKADDR_IN);
	int recvNum = recvfrom(fd, (char *)recvbuf, buflen, 0, 
			(struct sockaddr *)&peeraddr, (socklen_t *)&peerlen);
	return recvNum;
}

int udpSocket::recv(void *recvbuf, int buflen)
{
	zlibNetAddr aa;
	return recv(recvbuf,buflen,&aa);
}
int udpSocket::send(void *sendbuf, int len, zlibNetAddr addr)
{
	SOCKADDR_IN peeraddr=GetSOCKADD_INFromZlibNetAddr(addr);
	int peerlen = sizeof(SOCKADDR_IN);
	int recvNum = sendto(fd,(const char*) sendbuf, len, 0, (struct sockaddr *)&peeraddr, sizeof(peeraddr));
	return recvNum;
}
int udpSocket::send(void *sendbuf, int len, const char *ip, const unsigned short port)
{
	zlibNetAddr zaddr=GetZlibNetAddrFromIpAndPort(ip, port);
	return send(sendbuf,len,zaddr);;
}
int udpSocket::send(void *sendbuf, int len, const char *ip, const char *port)
{
	zlibNetAddr zaddr=GetZlibNetAddrFromIpAndPort(ip, port);
	return send(sendbuf,len,zaddr);;
}

}; // namespace baseservice
