#ifndef NIC_H
#define NIC_H

#include <stdio.h>

#if defined(__linux__)  &&  __GNU_LIBRARY__ == 1
#include <asm/io.h>
#else
#include <sys/io.h>
#endif
#if !defined(__OPTIMIZE__)
#error You must compile this driver with "-O"!
#endif

typedef struct raw_packet_t
{
unsigned char pac;
int len;
};

int send_raw_packet(unsigned char *packet,int len,int proto);





#endif
