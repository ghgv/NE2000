#include "ip.h"
#include "icmp.h"
#include "testne2000.h"
#include <stdio.h>
#include <string.h>


int decode_ip(unsigned char *packet)
{
  ipheader header;

  printf("##############INSIDE DECODE IP #######");
  memcpy(&header,&packet[14],20);

  printf("header.ip_src %x\n",htonl(header.ip_src));
  printf("header.ip_dst %x\n",htonl(header.ip_dst));
  printf("header.ip_p   %02x\n",header.ip_p);

  if(htonl(header.ip_dst)==0xc0a8020b)
  if(header.ip_p==IPPROTO_ICMP){
    printf("ICMP Packet.\n");
    decode_icmp(packet);
  
    }

  return 0;
}
