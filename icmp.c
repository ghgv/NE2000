#include "icmp.h"
#include "nic.h"
#include "testne2000.h"
#include <stdio.h>
#include <string.h>

icmp_hdr icmp;

int decode_icmp(unsigned char *packet)
{


  printf("########## INSIDE ICMP ############\n");
  memcpy(&icmp,&packet[14+20],sizeof(icmp_hdr));
  printf("Type:               %x\n", icmp.type);
  printf("Code:               %x\n", icmp.code);
  printf("Rest of the Header: %x\n", icmp.ROH);



  if(icmp.type==ICMP_ECHO_REQUEST){
    printf("ICMP REQUEST.\n");
    icmp_reply(packet);
    }

  return 0;
}

int icmp_reply(unsigned char *packet)
{
  icmp_hdr *pac;
  unsigned char *l;
  int len,i;
  unsigned char *paquete;


  ipheader iph;
  l=&iph;
  paquete=malloc(1560);

  iph.ip_v =  0x04;
  iph.ip_hl=  0x05;
  iph.ip_tos= 0x00;
  iph.ip_len= htons(sizeof(ipheader)+sizeof(icmp_hdr));
  iph.ip_id=  htons(0x6);
  iph.ip_off=htons(0x0);
  iph.ip_ttl=64;
  iph.ip_p=0x1; //ICMP Protocol number
  iph.ip_sum=0x0;
  iph.ip_src=inet_addr("192.168.2.11");  //htonl(0x2);
	iph.ip_dst=inet_addr("192.168.2.111");  //htonl(0x6);

  iph.ip_sum=htons(ip_sum_calc(20,(unsigned short *) l));
  printf("Checksum %x",iph.ip_sum);
  len=sizeof(icmp_hdr);
  pac=malloc(len);
  pac->type=0x0;
  pac->code=0x00;
  pac->ROH=0xd713;

  memcpy(paquete,&iph,sizeof(ipheader));
  memcpy(&paquete[sizeof(ipheader)],pac,sizeof(icmp_hdr));



  send_raw_packet(paquete,(len+20),ETH_P_IP);

  printf("####### end of icmp reply #########\n");

  return 0;
}
