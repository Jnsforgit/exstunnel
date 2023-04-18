#include "core.h"

void contextInit(Context_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));

#if (EST_ARCH == EST_ARCH_UNIX)
    if ((ctx->epoll_fd = epoll_create1(0)) < 0)
    {
        LOGE(("epoll: %d", errno));
    }
#else
    ctx->epoll_fd = -1;
#endif

#if EST_ARCH == EST_ARCH_WIN32
    {
        WSADATA data;
        WSAStartup(MAKEWORD(2, 2), &data);
    }
#else
    signal(SIGPIPE, SIG_IGN);
#endif

    ctx->dnstimeout = 3000;
    ctx->dns4.url = "udp://8.8.8.8:53";
    ctx->dns6.url = "udp://[2001:4860:4860::8888]:53";
}
