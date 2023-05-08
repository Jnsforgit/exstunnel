#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "event.h"

namespace exstunnel {
class TcpServer : private Noncopyable
{
public:
    TcpServer(EventBase *evbase);
    ~TcpServer();

    int bind(const std::string &host, unsigned short port, bool reuse_port = false);

private:

    Ip4Addr m_addr;
    EventBase *m_evbase;
};
}
#endif
