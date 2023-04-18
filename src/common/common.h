
#ifndef COMMON_H
#define COMMON_H

#include <ctype.h>
#include <direct.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <ws2tcpip.h>
#include <process.h>
#include <winerror.h>
#include <winsock2.h>

#define EST_VERSION "1.0"

#define EST_ARCH_UNIX 1
#define EST_ARCH_WIN32 2

#if defined(__unix__)
#define EST_ARCH EST_ARCH_UNIX
#elif defined(_WIN32)
#define EST_ARCH EST_ARCH_WIN32
#endif

#define BUFF_SIZE_FILENAME FILENAME_MAX
#define BUFF_SIZE_DATA 32 /* struct mg_connection :: data size */
#define BUFF_SIZE_DATA 32 // struct mg_connection :: data size

#define EST_TIMER_ONCE 0    // Call function once
#define EST_TIMER_REPEAT 1  // Call function periodically
#define EST_TIMER_RUN_NOW 2 // Call immediately when timer is set

typedef struct tagTimer Timer_t;

struct tagTimer
{
    unsigned long id;      // Timer ID
    uint64_t period_ms;    // Timer period in milliseconds
    uint64_t expire;       // Expiration timestamp in milliseconds
    unsigned flags;        // Possible flags values below
    void (*fn)(void *);    // Function to call
    void *arg;             // Function argument
    Timer_t *next; // Linkage
};

#include "str.h"
#include "iobuf.h"
#include "log.h"

#endif