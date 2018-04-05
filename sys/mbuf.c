#include "mbuf.h"
#include "socket.h"
#include "../tcp.h"
#include "../ip.h"
#include "../testne2000.h"
#include <stdio.h>

mbuf_t *buffer,*head,*curr;
int y;

int init_buf(){

   curr=(mbuf_t *)malloc(128);
   curr->m_nextpacket=0;
   curr->m_next=0;
   head=curr;
   printf("Init mbuffer:  HEAD= %x\n",head);

  return 0;
}

int mbuf(raw_packet_t *pack){

  mbuf_t  *buffer;
  int length,i;
  unsigned char *s;
  ipheader *aheader;
  tcpheader_t *atcpheader;
  unsigned short source_port;
  unsigned short dest_port;
  unsigned int ip_src;        //4 Byte
  unsigned int ip_dst;        //4 Byte
  tcb_t *tcb;


  length =pack->len;

  buffer=(mbuf_t *)malloc(128);
  printf("\n### mbuf prev: 0x%X next: 0x%X \n",pack->prev,pack->next);
  fflush(stdout);

  buffer->m_data=28;// The size of the header of the mbuf
  if(length<100){
    buffer->m_nextpacket=0;
    buffer->m_len=pack->len;
    buffer->m_type=(int) 34;s=(unsigned char *) buffer;
    memcpy(s+48,pack->data,length);
    aheader=(pack->data+14);
    printf("\nIP ver: 0x%02X ",aheader->ip_v &0xf);
    printf("\nIP hl: 0x%02X ",aheader->ip_hl &0xf);
    printf("\nIP tos: 0x%X ",aheader->ip_tos);
    printf("\nIP length: 0x%X ",ntohs(aheader->ip_len));
    printf("\nIP protocol: 0x%02X ",aheader->ip_p);
    printf("\nIP Src: 0x%X ",ntohl(aheader->ip_src));
    printf("\nIP Dst: 0x%X ",ntohl(aheader->ip_dst));
    if(aheader->ip_p==0x6)
      {

        atcpheader=(pack->data+14+20);
        printf("\nTCP Source port: %i 0x%X ",ntohs(atcpheader->source_port),ntohs(atcpheader->source_port));
        printf("\nTCP Dst port: %i 0x%X \n",ntohs(atcpheader->dest_port),ntohs(atcpheader->dest_port));
        for(i=1;i<MAX_FILE_DESCRIPTORS;i++)
          {
            tcb=fd[i].sck;
            if(tcb!=NULL)
              {
                printf("Local IP: 0x%08X 0x%08X, Local port: 0x%X 0x%X\n",htonl(tcb->tcb_lip.s_addr),ntohl(aheader->ip_dst) ,htons(tcb->tcb_lport),ntohs(atcpheader->dest_port ));
                if(tcb->tcb_lip.s_addr==aheader->ip_dst && tcb->tcb_lport==atcpheader->dest_port )
                    if(tcb->tcb_flags=htons(SYN+ACK | 5<<12))
                      tcb->tcb_state=SYN_RECEIVED;
              }
          }

      }


  }
  else
  {
  }
  //for(i=0;i<length;i++)
  //  printf("0x%02X ",*(s+i));
  buffer->m_nextpacket=0;
  buffer->m_next=0;
  curr->m_next=buffer;
  curr=buffer;

  //free(pack->data);
  //free(pack);

  return 0;
}

int get_buff(int f, unsigned char *addr , int count){

  mbuf_t  *buffer,*prev;
  tcb_t *tcb;
  int len,i;
  unsigned char *offset;


  tcb=fd[f].sck;

  if(head==curr)
    return -1;

  for(buffer=head->m_next;buffer!=0;buffer=buffer->m_next)
    {

      if((buffer->data[0x24]*0x100+buffer->data[0x25])==ntohs(tcb->tcb_rport))
          {
            printf(" Source.[Remote] port: %i\n",buffer->data[34]*0x100+buffer->data[35]);
            printf(" Dest.[local] port: %i\n",buffer->data[36]*0x100+buffer->data[37]);
            printf(" Seq.[Remote]: %i\n",buffer->data[38]*0x10000+buffer->data[39]*0x1000+buffer->data[40]*0x100+buffer->data[41]);
            printf(" Ack.[Remote]: %i\n",buffer->data[42]*0x10000+buffer->data[43]*0x1000+buffer->data[44]*0x100+buffer->data[45]);
            printf(" Dataoffset: %x \n",buffer->data[46]>>4);
            printf(" Flags: 0x%02X\n",(buffer->data[46]*0x100+buffer->data[47])&0x1FF);
            printf(" Window Size: 0x%04X\n",(buffer->data[48]*0x100+buffer->data[49]));
            printf(" Checksum: 0x%04X\n",(buffer->data[50]*0x100+buffer->data[51]));
            printf(" Window Size: 0x%04X\n",(buffer->data[52]*0x100+buffer->data[53]));
            printf(" Urgent Pointer: 0x%04X\n",(buffer->data[54]*0x100+buffer->data[55]));
            printf("\n Buffer: 0x%X \n",buffer);
            printf(" Buffer->m_next = 0x%X\n",buffer->m_next);
            printf(" Buffer->m_len = %i \n",buffer->m_len);

            tcb->fin=(buffer->data[46]*0x100+buffer->data[47])&0x1;
            tcb->syn=((buffer->data[46]*0x100+buffer->data[47])&0x2);
            tcb->rst=((buffer->data[46]*0x100+buffer->data[47])&0x4)>>2;
            tcb->psh=((buffer->data[46]*0x100+buffer->data[47])&0x8)>>3;
            tcb->ack=((buffer->data[46]*0x100+buffer->data[47])&0x10)>>4;
            tcb->urg=((buffer->data[46]*0x100+buffer->data[47])&0x20)>>5;

            printf(" TCB FIN = %i \n",tcb->fin);
            printf(" TCB SYN = %i \n",tcb->syn);
            printf(" TCB RST = %i \n",tcb->rst);
            printf(" TCB PSH = %i \n",tcb->psh);
            printf(" TCB ACK = %i \n",tcb->ack);
            printf(" TCB URG = %i \n",tcb->urg);
            printf(" TCB state = %i \n",tcb->tcb_state);
            printf(" TCB state = %i \n",tcb->tcb_state);
            fflush(stdout);
            prev=head;
            prev->m_next=buffer->m_next;

            offset=&buffer->data[56];

            if(buffer->m_len<=count)
            memcpy(addr,offset,buffer->m_len-56);//fix
            len=buffer->m_len-56;
            free(buffer);

            return  len;

          }
          printf(".");
    }

return 0;
}

int send_buff(int f, unsigned char *addr , int count){

  mbuf_t  *buffer,*prev;
  tcb_t *tcb;
  tcpheader_t tcphead;
  ipheader iph;
  unsigned char *paquete,*pseudo;
  unsigned short tmp;
  int i;
  paquete=malloc(1560);
  pseudo=malloc(20+count);


  tcb=fd[f].sck;

  if(tcb==NULL)
    return -1;

            printf(" ### Sending buffer ###\n");
            printf(" Remote IP: 0x%X\n",ntohl(tcb->tcb_rip.s_addr));
            printf(" Remote IP Port: %i\n",ntohs(tcb->tcb_rport));
            printf(" Source IP: 0x%X\n",ntohl(tcb->tcb_lip.s_addr));
            printf(" Source IP Port: %i\n",ntohs(tcb->tcb_lport));
            printf(" SEQ: %i count %i\n",tcb->tcb_snext,count);
            printf(" SEQ2: %i \n",tcb->tcb_snext+count);
            tmp=tcb->tcb_snext+count;
            tcb->tcb_snext=tmp;
            tcphead.source_port = tcb->tcb_lport;
            tcphead.dest_port   = tcb->tcb_rport;
            tcphead.seq_number  = htonl(tcb->tcb_snext);
            tcphead.ack_number  = htonl(tcb->tcb_suna);
            tcphead.flags       = 0x0250;
            tcphead.windows     =100;
            tcphead.checksum    =0;
            tcphead.urgent      = 0;

            iph.ip_v =  0x04;
            iph.ip_hl=  0x05;
            iph.ip_tos= 0x00;
            iph.ip_len= htons(sizeof(ipheader)+sizeof(tcp_header_t)+count);
            iph.ip_id=  htons(0x6);
            iph.ip_off= htons(0x0);
            iph.ip_ttl= 64;
            iph.ip_p=   0x6; //TCP Protocol number
            iph.ip_sum= 0x0;
            iph.ip_src= tcb->tcb_lip.s_addr;//inet_addr("192.168.2.11");  //htonl(0x2);
          	iph.ip_dst= tcb->tcb_rip.s_addr;//inet_addr("192.168.2.111");  //htonl(0x6);
            iph.ip_sum= htons(ip_sum_calc(20,(unsigned short *) &iph));

            memcpy(pseudo,&tcphead,sizeof(tcpheader_t));
            memcpy(&pseudo[sizeof(tcp_header_t)],addr,count);
            tcphead.checksum=htons(tcp_sum(pseudo,sizeof(tcpheader_t)+count,tcb->tcb_rip.s_addr,tcb->tcb_lip.s_addr));

            free(pseudo);
            memcpy(paquete,&iph,sizeof(ipheader));
            memcpy(&paquete[sizeof(ipheader)],&tcphead,sizeof(tcpheader_t));
            memcpy(&paquete[sizeof(ipheader)+sizeof(tcphead)],addr,count);

          //  for(i=0;i<count;i++)
          //    printf("%c\n",*(addr+i) );

            send_raw_packet(paquete,sizeof(tcpheader_t)+sizeof(ipheader)+count,ETH_P_IP);

return 0;
}
