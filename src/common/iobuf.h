

#ifndef IOBUF_H
#define IOBUF_H

typedef struct tagIobuf 
{
    unsigned char *buf;  // Pointer to stored data
    size_t size;         // Total size available
    size_t len;          // Current number of bytes
    size_t align;        // Alignment during allocation
}Iobuf_t;

#endif

