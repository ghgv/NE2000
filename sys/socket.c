#include "socket.h"
#include "../tcp.h"
#include "../ip.h"
#include "mbuf.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>


int socket_number=0; //socket_number
static int trigger=0;
extern int debugsocket;

int socket(int domain, int type, int protocol){
  int i;
  tcb_t *tcb;

  if(trigger==0)
  {
    srand(time(NULL));
    trigger++;
  }

  if(domain!= AF_INET )
    return -1;

  if(type!=SOCK_DGRAM)
      return -1;

   if(socket_number>MAX_FILE_DESCRIPTORS)
     return -1;

   for(i=1;i<MAX_FILE_DESCRIPTORS;i++)
    {

      if(fd[i].sck==0)
        {
        if(protocol==IPPROTO_TCP){
          fd[i].sck=(tcb_t *)malloc(sizeof(tcb_t));
          fd[i].type=type;
          fd[i].protocol=protocol;
          tcb=fd[i].sck;
          tcb->tcb_state=TCP_CLOSED;
          tcb->tcb_lport=0xffff & htons(rand());
          if(debugsocket==1)
          printf("Local port: 0x%X %i\n",0xffff & ntohs(tcb->tcb_lport), (unsigned short) ntohs(tcb->tcb_lport));
        }

        if(fd[i].sck==0)
            return -1;
        return i;
        }
    }

   return -1;
}

int close_s(int f){

  if( fd[f].sck== 0)
    return -1;//Socket did not exist

    free(fd[f].sck);
    fd[f].sck=0;
    return 0;
}

int connect_s(int sockfd,  sockaddr_in_t *addr, int addrlen)
{
  tcb_t *tcb;
  tcpheader_t tcphead;
  unsigned char *paquete,*pseudo;
  ipheader iph;
  paquete=malloc(1560);
  pseudo=malloc(20);

  tcb=fd[sockfd].sck;

  if(tcb!=0)
  {
    tcb->tcb_type=0;//addr->sin_family;
    tcb->tcb_rip.s_addr=addr->sin_addr.s_addr;
    tcb->tcb_rport=addr->sin_port;
    tcb->tcb_state=SYN_SENT;
    tcb->tcb_flags= htons(SYN | 5<<12) ;
    tcb->tcb_lip.s_addr=inet_addr("192.168.2.11");
    //tcb->tcb_lport=htons(100);
    tcb->tcb_swindow=htonl(0x123);
    tcb->tcb_snext=12;
    if(debugsocket==1){
    printf("Local port: %i 0x%X\n",(unsigned int ) ntohs(0xffff & tcb->tcb_lport),0xffff & ntohs(tcb->tcb_lport));
    printf("Remote port: %i 0x%X\n",(unsigned int) ntohs(0xffff & tcb->tcb_rport),0xffff & ntohs(tcb->tcb_rport));
    }


    iph.ip_v =  0x04;
    iph.ip_hl=  0x05;
    iph.ip_tos= 0x00;
    iph.ip_len= htons(sizeof(ipheader)+sizeof(tcpheader_t));
    iph.ip_id=  htons(0x6);
    iph.ip_off= htons(0x0);
    iph.ip_ttl= 64;
    iph.ip_p=   0x6; //TCP Protocol number
    iph.ip_sum= 0x0;
    iph.ip_src= tcb->tcb_lip.s_addr;//inet_addr("192.168.2.11");  //htonl(0x2);
    iph.ip_dst= tcb->tcb_rip.s_addr;//inet_addr("192.168.2.111");  //htonl(0x6);
    iph.ip_sum= htons(ip_sum_calc(20,(unsigned short *) &iph));

    tcphead.source_port = tcb->tcb_lport;
    tcphead.dest_port   = tcb->tcb_rport;
    tcphead.seq_number  = htonl(tcb->tcb_snext);
    tcphead.ack_number  = htonl(tcb->tcb_suna);
    tcphead.flags       = 0x0250;//tcb->tcb_flags=htons(SYN+ACK | 5<<12))
    tcphead.windows     =100;
    tcphead.checksum    =0;
    tcphead.urgent      = 0;

    memcpy(pseudo,&tcphead,sizeof(tcpheader_t));
    tcphead.checksum=htons(tcp_sum(pseudo,sizeof(tcpheader_t),tcb->tcb_rip.s_addr,tcb->tcb_lip.s_addr));



    memcpy(paquete,&iph,sizeof(ipheader));
    memcpy(&paquete[sizeof(ipheader)],&tcphead,sizeof(tcpheader_t));
    //memcpy(&paquete[sizeof(ipheader)+sizeof(tcphead)],addr,count);

    send_raw_packet(paquete,sizeof(tcpheader_t)+sizeof(ipheader),	0x0800);

    int msec = 0, trigger = 500;
  	int iterations=0;/* 10ms */
  	clock_t before = clock();
  	do {
    /*
     * wait while the answer from the peer happens
     */

    clock_t difference = clock() - before;
    msec = difference * 1000 / CLOCKS_PER_SEC;
    iterations++;
  } while ( msec < trigger );
  //printf("Time taken %d seconds %d milliseconds (%d iterations)\n", msec/1000, msec%1000, iterations);
  	if(tcb->tcb_state==SYN_SENT){
      printf("Connection timeout\n");
      return -1;
    }
  	if(tcb->tcb_state==SYN_RECEIVED)
    {
      	printf("Stablishing..\n");
        tcphead.flags=htons(ACK | 5<<12);
        tcphead.seq_number  = htonl(tcb->tcb_suna);
        tcphead.ack_number  = htonl(++tcb->tcb_snext);
        tcphead.checksum    =0;
        memcpy(pseudo,&tcphead,sizeof(tcpheader_t));
        tcphead.checksum=htons(tcp_sum(pseudo,sizeof(tcpheader_t),tcb->tcb_rip.s_addr,tcb->tcb_lip.s_addr));

        memcpy(paquete,&iph,sizeof(ipheader));
        memcpy(&paquete[sizeof(ipheader)],&tcphead,sizeof(tcpheader_t));
        //memcpy(&paquete[sizeof(ipheader)+sizeof(tcphead)],addr,count);
        if(send_raw_packet(paquete,sizeof(tcpheader_t)+sizeof(ipheader),	0x0800)==0){
          tcb->tcb_state=ESTABLISHED;
          printf("Stablished\n");
          free(pseudo);
          return 0;
        }
        else{
          tcb->tcb_state=TCP_CLOSED;
          printf("Not connected. Closing.\n");
          free(pseudo);
          return -11;
        }


    }

    free(pseudo);


    return -1;

  }
  return -1; //Error
}


int read_s(int sockfd,  unsigned char *addr, int count)
{
  tcb_t *tcb;
  int count1;

  tcb=fd[sockfd].sck;
  if(tcb!=0)
  {
    count1=get_buff(sockfd,addr,count);
    if(count1==-1)
      return -1;
    return count1;
  }
  return -1;
}

int write_s(int sockfd,  unsigned char *addr, int count)
{
  tcb_t *tcb;
  int count1;

  tcb=fd[sockfd].sck;
  if(tcb!=0)
  {
    count1=send_buff(sockfd,addr,count);
    if(count1==-1)
      return -1;
    return count1;
  }
  return -1;
}
