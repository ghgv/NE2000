#include "mbuf.h"
#include "socket.h"
#include "../tcp.h"
#include <stdio.h>

mbuf_t *buffer,*head,*curr;

int init_buf(){

   curr=(mbuf_t *)malloc(128);
   curr->m_nextpacket=0;
   curr->m_next=0;
   head=curr;
   printf("HEAD %x\n",head);

  return 0;
}

int mbuf(unsigned char *data){

  mbuf_t  *buffer;
  int length,i;
  unsigned char *s;

  length =*(data+17);
  for (i=0;i<20;i++)
  printf("0x%02x ",*(data+i));

  buffer=(mbuf_t *)malloc(128);
  printf("\n### mbuf length### %i %x \n",length,buffer);
  fflush(stdout);

  buffer->m_data=(28);
  if(length<100){
    buffer->m_nextpacket=0;
    buffer->m_type=(int) 34;s=(unsigned char *) buffer;
    memcpy(s+48,data,length);

  }
  else
  {
  }

  buffer->m_nextpacket=0;
  buffer->m_next=0;

  curr->m_next=buffer;
  curr=buffer;


  return 0;
}

int get_buff(int f, unsigned char *addr , int count){

  mbuf_t  *buffer;
  tcb_t *tcb;

  tcb=fd[f].sck;

  if(head==curr)
    return -1;

  for(buffer=head->m_next;buffer!=0;buffer=buffer->m_next)
    {
      if((buffer->data[0x24]*0x100+buffer->data[0x25])==ntohs(tcb->tcb_rport))
          {
            //printf("#### get buff ####");
            memcpy(addr,&buffer->data[0x24],1);//fix
            return  1;

          }
    }

}
