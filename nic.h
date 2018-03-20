#ifndef NIC_H
#define NIC_H



#if defined(__linux__)  &&  __GNU_LIBRARY__ == 1
#include <asm/io.h>
#else
#include <sys/io.h>
#endif
#if !defined(__OPTIMIZE__)
#error You must compile this driver with "-O"!
#endif



typedef struct in_addr
{
    unsigned long s_addr;  // load with inet_aton()
} in_addr;


struct netif {
  struct netif *next;
  in_addr ip_addr;
   in_addr netmask;
   in_addr gw;
  //err_t (* input)(struct pbuf *p, struct netif *inp);
  //err_t (* output)(struct netif *netif, struct pbuf *p,struct ip_addr *ipaddr);
  //err_t (* linkoutput)(struct netif *netif, struct pbuf *p);
  void *state;
  #if LWIP_DHCP
  struct dhcp *dhcp;
  #endif
  unsigned char hwaddr_len;
  unsigned char hwaddr[6];//NETIF_MAX_HWADDR_LEN
  unsigned short mtu;
  char name[2];
  unsigned char num;
  unsigned char flags;
};


typedef struct raw_packet_t
{
unsigned char pac;
int len;
};

int send_raw_packet(unsigned char *packet,int len,int proto);
int inet_aton(const char *cp, in_addr *ap);
int get_raw_packet(unsigned char *packet,int len,int proto);



#endif
