#include "TcpSocket.h"

TcpSocket::TcpSocket()
{
    this->m_iListeningFd = -1;
    this->m_iEpollFd = -1;
}

bool TcpSocket::server_init(const long lInPort, const int iInLisNum)
{
    m_iListeningFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == m_iListeningFd)
    {
        return false;
    }

    int optval = -1;
    int ret = setsockopt (m_iListeningFd, SOL_SOCKET, SO_REUSEADDR, (const char*) &optval, sizeof(optval));
    if (-1 == ret)
    {
        return false;
    }

    bzero(&m_stServerAddr, sizeof(m_stServerAddr));
    m_stServerAddr.sin_family = AF_INET;
    m_stServerAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    m_stServerAddr.sin_port = htons(lInPort);

    ret = bind(m_iListeningFd, (struct sockaddr *)(&m_stServerAddr), sizeof(struct sockaddr_in));
    if(-1 == ret)
    {
        close(m_iListeningFd);
        return false;
    }

    ret = listen(m_iListeningFd, iInListenNum);
    if (ret == -1)
    {
        close(m_iListeningFd);
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

    iListener = ::socket(AF_INET, SOCK_STREAM, 0);

    if ( !is_valid() )
    {
        LOG_ERROR("status == -1   errno == %s in ::socket" ,strerror(errno));
        return false;
    }

    int iOn = 1;

    if ( setsockopt ( iListener, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &iOn, sizeof ( iOn ) ) == -1 )
    {
        LOG_ERROR("status == -1   errno == %s in ::setsockopt" ,strerror(errno));
        return false;
    }

    bzero(&stExtSerAddr, sizeof(stExtSerAddr));
    stExtSerAddr.sin_family = AF_INET;
    stExtSerAddr.sin_addr.s_addr = inet_addr(sIpAddr);
    stExtSerAddr.sin_port = htons(iInSerPort);

    if(connect(iListener , (struct sockaddr *)&stExtSerAddr, sizeof(stExtSerAddr)) == -1)
    {
        LOG_ERROR("status == -1   errno == %s in ::connect" ,strerror(errno));
        return false;
    }

    return true;
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

int TcpSocket::close_fd(int fd)
{
    return close(fd);
}
