#ifndef NET_H
#define NET_H

namespace exstunnel
{
    struct Ip4Addr
    {
        Ip4Addr(const std::string &host, unsigned short port);
        Ip4Addr(unsigned short port = 0);
        Ip4Addr(const struct sockaddr_in &addr);
        std::string toString() const;
        std::string ip() const;
        unsigned short port() const;
        unsigned int ipInt() const;
        // if you pass a hostname to constructor, then use this to check error
        bool isIpValid() const;
        struct sockaddr_in &getAddr();
        static std::string hostToIp(const std::string &host);

    private:
        struct sockaddr_in m_addr;
    };

    class net
    {
    public:
        template <class T>
        static T hton(T v);
        template <class T>
        static T ntoh(T v);
        static int setNonBlock(int fd, bool value = true);
        static int setReuseAddr(int fd, bool value = true);
        static int setReusePort(int fd, bool value = true);
        static int setNoDelay(int fd, bool value = true);
    };
}

#endif
