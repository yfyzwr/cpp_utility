#include "TcpSocket.h"

TcpSocket::TcpSocket()
{
    this->m_iListenSockFd = -1;
    this->m_iEpollFd = -1;
}

bool TcpSocket::server_init(const long lInPort, const int iInLisNum)
{
    m_iMaxListenNum = iInLisNum;

    m_iListenSockFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == m_iListenSockFd)
    {
        return false;
    }

    int optval = 1;
    int ret = setsockopt (m_iListenSockFd, SOL_SOCKET, SO_REUSEADDR, (const char*) &optval, sizeof(optval));
    if (-1 == ret)
    {
        return false;
    }

    bzero(&m_stServerAddr, sizeof(m_stServerAddr));
    m_stServerAddr.sin_family = AF_INET;
    m_stServerAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    m_stServerAddr.sin_port = htons(lInPort);

    ret = bind(m_iListenSockFd, (struct sockaddr *)(&m_stServerAddr), sizeof(struct sockaddr_in));
    if(-1 == ret)
    {
        close(m_iListenSockFd);
        return false;
    }

    ret = listen(m_iListenSockFd, m_iMaxListenNum);
    if (ret == -1)
    {
        close(m_iListenSockFd);
        return false;
    }

    return true;
}

bool TcpSocket::client_init(const char *sInSerIP, const int iInSerPort)
{
    char sIpAddr[32] = {0};

    if (!get_host_info(sInSerIP, sIpAddr, sizeof(sIpAddr)))
    {
        return false;
    }

    m_iClientSockFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == m_iClientSockFd)
    {
        return false;
    }

    int optval = 1;
    int ret = setsockopt (m_iClientSockFd, SOL_SOCKET, SO_REUSEADDR, (const char*) &optval, sizeof(optval));
    if (-1 == ret)
    {
        return false;
    }

    bzero(&m_stRemoteServerAddr, sizeof(m_stRemoteServerAddr));
    m_stRemoteServerAddr.sin_family = AF_INET;
    m_stRemoteServerAddr.sin_addr.s_addr = inet_addr(sIpAddr);
    m_stRemoteServerAddr.sin_port = htons(iInSerPort);

    if(connect(m_iClientSockFd , (struct sockaddr *)&m_stRemoteServerAddr, sizeof(m_stRemoteServerAddr)) == -1)
    {
        return false;
    }

    return true;
}

bool TcpSocket::setnonblocking(const int iInSockFd, const bool bInFlag)
{
    int iOpts;
    iOpts = fcntl(iInSockFd, F_GETFL);
    if (iOpts < 0)
    {
        return false;
    }

    if (bInFlag)
    {
        iOpts = (iOpts | O_NONBLOCK);
    }
    else
    {
        iOpts = (iOpts & ~O_NONBLOCK);
    }
    fcntl(iInSockFd, F_SETFL, iOpts);

    return true;
}

bool TcpSocket::epoll_add_sock(int iInNewFd)
{
    memset(&m_stEpollEvt, 0, sizeof(m_stEpollEvt));
    m_stEpollEvt.events = EPOLLIN | EPOLLET;
    m_stEpollEvt.data.ptr = NULL;
    m_stEpollEvt.data.fd = iInNewFd;

    if (epoll_ctl(m_iEpollFd, EPOLL_CTL_ADD, iInNewFd, &m_stEpollEvt) < 0)
    {
        return false;
    }

    return true;
}

bool TcpSocket::epoll_del_sock(int iInSocFd)
{
    memset(&m_stEpollEvt, 0, sizeof(m_stEpollEvt));
    m_stEpollEvt.data.fd = iInSocFd;
    m_stEpollEvt.data.ptr = NULL;

    if(epoll_ctl(m_iEpollFd, EPOLL_CTL_DEL, iInSocFd, &m_stEpollEvt) < 0)
    {
        return false;
    }

    return true;
}

bool TcpSocket::accept (int& iOutRetFd)
{
    socklen_t stLen = 0;
    iOutRetFd = ::accept(m_iListenSockFd, (struct sockaddr *) &m_iClientSockFd, &stLen);
    if (iOutRetFd < 0)
    {
        if (errno == EINTR ||errno == EAGAIN)
        {
            return false;
        }
        else
        {
            return false;
        }
    }

    return true;
}

int TcpSocket::receive_data(int iInSocFd, char* &pOutRecvBuf, int &iOutRecvLength)
{
    int iRet = 0;
    int iDateLength = 0;
    char sDateLength[10] = {0};

    iRet = recv(iInSocFd, sDateLength, sizeof(sDateLength), 0);
    if(iRet == -1)
    {
        return -1;
    }

    return atoi(sDateLength);
}

bool TcpSocket::epoll_init(int iInMaxEpollNum/* = 1000 */)
{
    struct rlimit stRt;

    stRt.rlim_max = stRt.rlim_cur = m_iMaxEpollNum = iInMaxEpollNum;

    if (-1 == setrlimit(RLIMIT_NOFILE, &stRt)) //指定比进程可打开的最大文件描述词大一的值，此值限制epoll最大连接能力
    {
        return false;
    }

    if (-1 == (m_iEpollFd = epoll_create(iInMaxEpollNum)))
    {
        return false;
    }

    if (-1 == fcntl(m_iEpollFd, F_SETFD, FD_CLOEXEC))
    {
        close(m_iEpollFd);
        return false;
    }

    return true;
}

int TcpSocket::epoll_wait_get(int iInTimeout)
{
    return epoll_wait(m_iEpollFd, m_stEvents, m_iMaxEpollNum, iInTimeout);
}

int TcpSocket::get_listen_socket()
{
    return this->m_iListenSockFd;
}

// privated method

bool TcpSocket::get_host_info(const char* sNameOrIp, char* pIpAddr, size_t nIpAddrLen)
{
    struct hostent* hptr = NULL;

    if ((hptr = gethostbyname(sNameOrIp)) == NULL)
    {
        struct in_addr address;
        if(inet_aton(sNameOrIp, &address) != 0)
        {
            hptr = gethostbyaddr((const char*)&address.s_addr, sizeof(address.s_addr), AF_INET);
        }
    }

    if (NULL == hptr)
    {
        return false;
    }
    else
    {
        inet_ntop(hptr->h_addrtype, hptr->h_addr_list[0], pIpAddr, nIpAddrLen);
        return true;
    }
}

int TcpSocket::close_fd(int iInFd)
{
    return close(iInFd);
}
