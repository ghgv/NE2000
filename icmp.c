#include "icmp.h"
#include "nic.h"
#include "ip.h"
#include "testne2000.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

extern pthread_mutex_t lock;
extern int debugicmp;

icmp_reply_t icmp;

int decode_icmp(unsigned char *packet)
{


  ipheader ipheader1;
  if(debugicmp)
  printf("\n########## INSIDE ICMP ############\n");
  memcpy(&ipheader1,&packet[14],sizeof(ipheader));
  memcpy(&icmp,&packet[14+20],sizeof(icmp_reply_t));
  if(debugicmp)
  {
    printf("Source IP:          0x%X %i.%i.%i.%i\n", ntohl(ipheader1.ip_src),0xFF&ntohl(ipheader1.ip_src)>>24,0xFF&ntohl(ipheader1.ip_src)>>16,0xFF&ntohl(ipheader1.ip_src)>>8,0xFF&ntohl(ipheader1.ip_src));
    printf("Type:               %x\n", icmp.type);
    printf("Code:               %x\n", icmp.code);
    printf("Checksum:           %x\n", icmp.checksum);
    printf("Identifier:         %x\n", icmp.identifier);
    printf("Sequence_number:    %x\n", icmp.Sequence_number);
    printf("Identifier:         %x\n", icmp.identifier);
  }



  if(icmp.type==ICMP_ECHO_REQUEST){
    if(debugicmp)
    printf("ICMP REQUEST.\n");
    pthread_mutex_unlock(&lock);// This might not be OK
    icmp_reply(packet,ipheader1.ip_src);
    }

  return 0;
}

int icmp_reply(unsigned char *packet,unsigned long src_ip)
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
	iph.ip_dst=inet_addr(&src_ip);  //htonl(0x6);

  iph.ip_sum=htons(ip_sum_calc(20,(unsigned short *) l));
  //printf("Checksum 0x%X\n",iph.ip_sum);
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



  return 0;
}
