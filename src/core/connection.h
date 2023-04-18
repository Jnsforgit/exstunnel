
#ifndef CONNECTION_H
#define CONNECTION_H

struct tagConnection
{
    Connection_t *next;
    Context_t *context;
    SocketAddr_t loc;
    SocketAddr_t rem;
    void *fd;                    // Connected socket, or LWIP data
    unsigned long id;            // Auto-incrementing unique connection ID
    Iobuf_t recv;                // Incoming data
    Iobuf_t send;                // Outgoing data
    event_handler_t fn;           // User-specified event handler function
    void *fn_data;               // User-specified function parameter
    event_handler_t pfn;         // Protocol-specific handler function
    void *pfn_data;              // Protocol-specific function parameter
    char data[BUFF_SIZE_DATA];   // Arbitrary connection data
    void *tls;                   // TLS specific data
    unsigned is_listening : 1;   // Listening connection
    unsigned is_client : 1;      // Outbound (client) connection
    unsigned is_accepted : 1;    // Accepted (server) connection
    unsigned is_resolving : 1;   // Non-blocking DNS resolution is in progress
    unsigned is_arplooking : 1;  // Non-blocking ARP resolution is in progress
    unsigned is_connecting : 1;  // Non-blocking connect is in progress
    unsigned is_tls : 1;         // TLS-enabled connection
    unsigned is_tls_hs : 1;      // TLS handshake is in progress
    unsigned is_udp : 1;         // UDP connection
    unsigned is_websocket : 1;   // WebSocket connection
    unsigned is_mqtt5 : 1;       // For MQTT connection, v5 indicator
    unsigned is_hexdumping : 1;  // Hexdump in/out traffic
    unsigned is_draining : 1;    // Send remaining data, then close and free
    unsigned is_closing : 1;     // Close and free the connection immediately
    unsigned is_full : 1;        // Stop reads, until cleared
    unsigned is_resp : 1;        // Response is still being generated
    unsigned is_readable : 1;    // Connection is ready to read
    unsigned is_writable : 1;    // Connection is ready to write
};

#endif