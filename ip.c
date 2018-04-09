#include "ip.h"
#include "icmp.h"
#include "tcp.h"
#include "nic.h"
//#include "testne2000.h"
#include <stdio.h>
#include <string.h>

extern int debugip;

int decode_ip(unsigned char *packet)
{
  ipheader header;

  if(debugip=1)
  printf("##############INSIDE DECODE IP #######");
  memcpy(&header,&packet[14],20);

  if(debugip=1)
  {
    printf("header.ip_src %x\n",htonl(header.ip_src));
    printf("header.ip_dst %x\n",htonl(header.ip_dst));
    printf("header.ip_p   %02x\n",header.ip_p);
  }

  if(htonl(header.ip_dst)==0xc0a8020b)
  {
    switch (header.ip_p) {
      case IPPROTO_ICMP:
            if(debugip=1)
            printf("ICMP Packet.\n");
            decode_icmp(packet);
            break;
      case IPPROTO_TCP:
          if(debugip=1)
            printf("TCP Packet.\n");
            decode_tcp(packet);
            break;


    }


  }

  return 0;
}
