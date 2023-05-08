#include "common.h"
#include "net.h"
#include "port_posix.h"

namespace exstunnel
{
    Ip4Addr::Ip4Addr(const std::string &host, unsigned short port)
    {
    }

    Ip4Addr::Ip4Addr(unsigned short port) : Ip4Addr("", port)
    {
    }

    Ip4Addr::Ip4Addr(const sockaddr_in &addr) : m_addr(addr)
    {
    }

    std::string Ip4Addr::toString() const
    {
        uint32_t uip = m_addr.sin_addr.s_addr;
        return utils::format("%d.%d.%d.%d:%d", (uip >> 0) & 0xff, (uip >> 8) & 0xff, (uip >> 16) & 0xff, (uip >> 24) & 0xff, ntohs(m_addr.sin_port));
    }

    std::string Ip4Addr::ip() const
    {
        uint32_t uip = m_addr.sin_addr.s_addr;
        return utils::format("%d.%d.%d.%d", (uip >> 0) & 0xff, (uip >> 8) & 0xff, (uip >> 16) & 0xff, (uip >> 24) & 0xff);
    }

    unsigned short Ip4Addr::port() const
    {
        return (unsigned short)ntohs(m_addr.sin_port);
    }

    unsigned int Ip4Addr::ipInt() const
    {
        return ntohl(m_addr.sin_addr.s_addr);
    }

    bool Ip4Addr::isIpValid() const
    {
        return m_addr.sin_addr.s_addr != INADDR_NONE;
    }

    sockaddr_in &Ip4Addr::getAddr()
    {
        return m_addr;
    }

    std::string Ip4Addr::hostToIp(const std::string &host)
    {
        Ip4Addr addr(host, 0);
        return addr.ip();
    }

    template <class T>
    T net::hton(T v)
    {
        return port::htobe(v);
    }

    template <class T>
    T net::ntoh(T v)
    {
        return port::htobe(v);
    }

    int net::setNonBlock(int fd, bool value)
    {
#if defined F_GETFL && defined F_SETFL && defined O_NONBLOCK && !defined __INNOTEK_LIBC__
        int err;
        int flags;

        do
        {
            flags = fcntl(fd, F_GETFL, 0);
        } while (flags < 0 && get_last_socket_error() == S_EINTR);

        if (flags < 0)
        {
            loge("fcntl GETFL"); /* non-critical */
            return;
        }

        if (value)
        {
            flags |= O_NONBLOCK;
        }
        else
        {
            flags &= ~O_NONBLOCK;
        }

        do
        {
            err = fcntl(fd, F_SETFL, flags);
        } while (err < 0 && get_last_socket_error() == S_EINTR);

        if (err < 0)
        {
            sockerror("fcntl SETFL"); /* non-critical */
        }

#else /* WIN32 or similar */
        if (ioctlsocket(fd, (long)FIONBIO, (unsigned long *)&value) < 0)
        {
            loge("ioctlsocket error!"); /* non-critical */
        }
#endif
    }

    int net::setReuseAddr(int fd, bool value)
    {
        char flag = value;
        int len = sizeof(flag);

        return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, len);
    }

    int net::setReusePort(int fd, bool value)
    {
#ifndef SO_REUSEPORT
        fatalif(value, "SO_REUSEPORT not supported");
        return 0;
#else
        int flag = value;
        int len = sizeof flag;
        return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, len);
#endif
    }

    int net::setNoDelay(int fd, bool value)
    {
        char flag = value;
        int len = sizeof flag;

        return setsockopt(fd, SOL_SOCKET, TCP_NODELAY, &flag, len);
    }
}