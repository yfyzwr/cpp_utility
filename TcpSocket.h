#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <sys/resource.h>

class TcpSocket {
public:
    TcpSocket();
    int get_listen_socket();

    bool server_init(const long lInPort, const int iInLisNum);
    bool client_init(const char* sInSerIP, const int iInSerPort);
    bool setnonblocking(const int iInSockFd, const bool bInFlag);

    bool accept (int& iOutRetFd);
    int receive_data(int iInSocFd, char* &pOutRecvBuf, int &iOutRecvLength);

    bool epoll_init()(int iInMaxEpollNum = 1000);
    bool epoll_add_sock(int iInNewFd);
    bool epoll_del_sock(int iInSocFd);
    int epoll_wait_get(int iInTimeout);

private:
    int close_fd(int iInFd);
    bool get_host_info(const char* sNameOrIp, char* pIpAddr, size_t nIpAddrLen);

private:
    int m_iListenSockFd;
    int m_iMaxListenNum;
    int m_iClientSockFd;

    int m_iEpollFd;
    int m_iMaxEpollNum;
    struct epoll_event m_stEpollEvt;
    struct epoll_event m_stEvents[MAXEPOLLSIZE];

    struct sockaddr_in m_stServerAddr;
    struct sockaddr_in m_stClientAddr;
    struct sockaddr_in m_stRemoteServerAddr;
};

#endif //TCPSOCKET_H
