#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <strings.h>

class TcpSocket {
public:
    TcpSocket();
    bool server_init(const long lInPort, const int iInLisNum);
    bool client_init(const char* sInSerIP, const int iInSerPort);

private:
    int close_fd(int fd);
    bool get_host_info(const char* sNameOrIp, char* pIpAddr, size_t nIpAddrLen);

private:
    int m_iListeningFd;
    int m_iEpollFd;

    struct sockaddr_in m_stServerAddr;
    struct sockaddr_in m_stClientAddr;
};

#endif //TCPSOCKET_H
