
#ifndef CORE_H
#define CORE_H

typedef struct tagContext Context_t;
typedef struct tagConnection Connection_t;
typedef void (*event_handler_t)(Connection_t *, int ev, void *ev_data, void *fn_data);

typedef struct tagSocketAddr
{
    uint16_t port;   // TCP or UDP port in network byte order
    uint32_t ip;     // IP address in network byte order
    uint8_t ip6[16]; // IPv6 address
    bool is_ip6;     // True when address is IPv6 address
} SocketAddr_t;

typedef struct tagDns 
{
    const char *url;    // DNS server URL
    Connection_t *conn;    // DNS server connection
}Dns_t;

struct tagContext
{
    Connection_t *conns;            // List of active connections
    Dns_t dns4;                  // DNS for IPv4
    Dns_t dns6;                  // DNS for IPv6
    int dnstimeout;              // DNS resolve timeout in milliseconds
    bool use_dns6;               // Use DNS6 server by default, see #1532
    unsigned long nextid;        // Next connection ID
    unsigned long timerid;       // Next timer ID
    void *userdata;              // Arbitrary user data pointer
    uint16_t mqtt_id;            // MQTT IDs for pub/sub
    void *active_dns_requests;   // DNS requests in progress
    Timer_t *timers;             // Active timers
    int epoll_fd;                // Used when MG_EPOLL_ENABLE=1
    void *priv;                  // Used by the MIP stack
    size_t extraconnsize;        // Used by the MIP stack
};

void contextInit(Context_t *mgr);

#include "connection.h"
#include "event.h"

#endif