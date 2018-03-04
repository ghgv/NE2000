#include "icmp.h"
#include "nic.h"
#include "testne2000.h"
#include <stdio.h>
#include <string.h>

icmp_reply_t icmp;

int decode_icmp(unsigned char *packet)
{


  printf("########## INSIDE ICMP ############\n");
  memcpy(&icmp,&packet[14+20],sizeof(icmp_reply_t));
  printf("Type:               %x\n", icmp.type);
  printf("Code:               %x\n", icmp.code);
  printf("Checksum:           %x\n", icmp.checksum);
  printf("Identifier:         %x\n", icmp.identifier);
  printf("Sequence_number:    %x\n", icmp.Sequence_number);
  printf("Identifier:         %x\n", icmp.identifier);



  if(icmp.type==ICMP_ECHO_REQUEST){
    printf("ICMP REQUEST.\n");
    icmp_reply(packet);
    }

  return 0;
}

int icmp_reply(unsigned char *packet)
{
  icmp_reply_t *pac;
  unsigned char *l;
  int len,i;
  unsigned char *paquete;


  ipheader iph;
  l=&iph;
  paquete=malloc(1560);

  iph.ip_v =  0x04;
  iph.ip_hl=  0x05;
  iph.ip_tos= 0x00;
  iph.ip_len= htons(sizeof(ipheader)+sizeof(icmp_reply_t));
  iph.ip_id=  htons(0x6);
  iph.ip_off=htons(0x0);
  iph.ip_ttl=64;
  iph.ip_p=0x1; //ICMP Protocol number
  iph.ip_sum=0x0;
  iph.ip_src=inet_addr("192.168.2.11");  //htonl(0x2);
	iph.ip_dst=inet_addr("192.168.2.111");  //htonl(0x6);

  iph.ip_sum=htons(ip_sum_calc(20,(unsigned short *) l));
  printf("Checksum %x",iph.ip_sum);
  len=sizeof(icmp_reply_t);
  pac=malloc(len);
  pac->type=0x0;
  pac->code=0x00;
  pac->checksum=0x00;
  pac->identifier=icmp.identifier;
  pac->Sequence_number=icmp.Sequence_number;

/* TODO Time stamp*/

  memcpy(paquete,&iph,sizeof(ipheader));
  memcpy(&paquete[sizeof(ipheader)],pac,sizeof(icmp_reply_t));



  send_raw_packet(paquete,(len+20),ETH_P_IP);

  printf("####### end of icmp reply #########\n");

  return 0;
}
