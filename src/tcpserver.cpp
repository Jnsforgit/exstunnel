#include "tcpserver.h"

namespace exstunnel
{
    TcpServer::TcpServer(EventBase *evbase)
        : m_evbase(evbase)
    {
    }

    TcpServer::~TcpServer()
    {
    }

    int TcpServer::bind(const std::string &host, unsigned short port, bool reuse_port)
    {
        m_addr = Ip4Addr(host, port);
        int fd = socket(AF_INET, SOCK_STREAM, 0);

        int r = net::setReuseAddr(fd);
        fatalif(r, "set socket reuse option failed");

        r = net::setReusePort(fd, reuse_port);
        fatalif(r, "set socket reuse port option failed");

    #ifdef FD_CLOEXEC
        r = util::addFdFlag(fd, FD_CLOEXEC);
        fatalif(r, "addFdFlag FD_CLOEXEC failed");
    #endif
    
        r = ::bind(fd, (struct sockaddr *)&m_addr.getAddr(), sizeof(struct sockaddr));
        if (r)
        {
            closesocket(fd);
            loge("bind to %s failed %d %s", m_addr.toString().c_str(), errno, strerror(errno));
            return errno;
        }

        r = listen(fd, 20);
        fatalif(r, "listen failed %d %s", errno, strerror(errno));

        logi("fd %d listening at %s", fd, m_addr.toString().c_str());

        // listen_channel_ = new Channel(base_, fd, kReadEvent);
        // listen_channel_->onRead([this] { 
        //     handleAccept(); 
        // });

        return 0;
    }
}