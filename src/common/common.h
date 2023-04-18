
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#ifndef __WIN32__
    #if defined(_WIN32) || defined(WIN32)
        #define __WIN32__
    #endif
#endif

#ifdef __WIN32__
    #ifdef __cplusplus
        extern "C"
        {
    #endif

    extern int access(const char *path, int mode);

    #ifdef __cplusplus
        }
    #endif
#else
    #include <unistd.h>
#endif

/* #define PRIVATE static */
#define PRIVATE

#ifdef TEST
    #define MAXRHS 5 /* Set low to exercise exception code */
#else
    #define MAXRHS 1000
#endif


#define ISSPACE(X) isspace((unsigned char)(X))
#define ISDIGIT(X) isdigit((unsigned char)(X))
#define ISALNUM(X) isalnum((unsigned char)(X))
#define ISALPHA(X) isalpha((unsigned char)(X))
#define ISUPPER(X) isupper((unsigned char)(X))
#define ISLOWER(X) islower((unsigned char)(X))

#define lemonStrlen(X) ((int)strlen(X))

typedef enum
{
    LEMON_FALSE = 0,
    LEMON_TRUE
} Boolean;

#include "utils.h"
#include "options.h"

#endif