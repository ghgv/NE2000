#include "mbuf.h"
#include "socket.h"
#include "../tcp.h"
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

  }
  else
  {
  }

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
            printf(" Dest. TCP port: %x\n",buffer->data[0x24]*0x100+buffer->data[0x25]);
            printf(" Buffer: 0x%X \n",buffer);
            printf(" Buffer->m_next = 0x%X\n",buffer->m_next);
            printf(" Buffer->m_len = %i \n",buffer->m_len);
            fflush(stdout);
            prev=head;
            prev->m_next=buffer->m_next;


            offset=&buffer->data[14];


            if(buffer->m_len<=count)
            memcpy(addr,offset,buffer->m_len-14);//fix
            //for(i=0;i<buffer->m_len;i++)
              //printf("-0x%02X ",*(addr+i));
            len=buffer->m_len-14;
            free(buffer);

            return  len;

          }
          printf(".");
    }

return 0;
}
