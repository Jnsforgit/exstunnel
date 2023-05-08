#include "port_posix.h"

namespace exstunnel
{
    namespace port
    {
        in_addr getHostByName(const std::string &host)
        {
            struct in_addr addr;
            char buf[1024];
            struct hostent hent;
            struct hostent *he = NULL;
            int herrno = 0;

            memset(&hent, 0, sizeof hent);

        #ifdef USE_WIN32
            he = gethostbyname(host.c_str());
            if (NULL == he)
            {
                DWORD dwError = WSAGetLastError();
                if (dwError != 0)
                {
                    if (dwError == WSAHOST_NOT_FOUND)
                    {
                        loge("Host not found:[%s]", host.c_str());
                        addr.s_addr = INADDR_NONE;
                    }
                    else if (dwError == WSANO_DATA)
                    {
                        loge("No data record found:[%s]", host.c_str());
                        addr.s_addr = INADDR_NONE;
                    }
                    else
                    {
                        loge("Function failed with error: :[%ld:%s]", dwError, host.c_str());
                        addr.s_addr = INADDR_NONE;
                    }
                }
            }
            else
            {
                addr = *reinterpret_cast<struct in_addr *>(he->h_addr);
            }
        #else
            int r = gethostbyname_r(host.c_str(), &hent, buf, sizeof buf, &he, &herrno);
            if (r == 0 && he && he->h_addrtype == AF_INET)
            {
                addr = *reinterpret_cast<struct in_addr *>(he->h_addr);
            }
            else
            {
                addr.s_addr = INADDR_NONE;
            }
        #endif

            return addr;
        }

        uint64_t gettid()
        {
        #ifdef USE_WIN32
            return GetCurrentThreadId();
        #else
            return syscall(SYS_gettid);
        #endif
        }
    }
}
