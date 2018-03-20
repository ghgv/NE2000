#include "socket.h"
#include "../tcp.h"
#include <stdio.h>
#include <stdlib.h>

int socket_number=0; //socket_number

int socket(int domain, int type, int protocol){
  int i;

  if(domain!= AF_INET )
    return 1;

  if(type!=SOCK_DGRAM)
      return 1;

   if(socket_number>MAX_FILE_DESCRIPTORS)
     return 1;

   for(i=1;i<MAX_FILE_DESCRIPTORS;i++)
    {
      if(fd[i].sck==0)
        {
        if(protocol==IPPROTO_TCP)
          fd[i].sck=(tcb_t *)malloc(sizeof(tcb_t));
        if(fd[i].sck==0)
            return 1;

        socket_number=i;
        return i;
        }
    }

   return 1;
}

int close_s(int f){

  if( fd[f].sck== 0)
    return -1;//Socket did not exist

    free(fd[f].sck);
    return 0;
}

int connect_s(int sockfd,  sockaddr_in_t *addr, int addrlen)
{
  tcb_t *tcb;

  tcb=fd[sockfd].sck;



  if(tcb!=0)
  {
    tcb->tcb_type=0;//addr->sin_family;
    tcb->tcb_rip.s_addr=addr->sin_addr.s_addr;
    tcb->tcb_rport=addr->sin_port;
    tcb->tcb_state=SYN_SENT;
    tcb->tcb_flags= htons(SYN | 5<<12) ;
    tcb->tcb_lip.s_addr=inet_addr("192.168.2.11");
    tcb->tcb_lport=htons(4100);
    printf("connect lport %i\n",ntohs(tcb->tcb_lport));


    return 0;
  }

  return -1; //Error
}

int read_s(int sockfd,  unsigned char *addr, int count)
{
  tcb_t *tcb;



  tcb=fd[sockfd].sck;
  if(tcb!=0)
  {
    if(get_buff(sockfd,addr,count)==-1)
      return -1;
    return 0;
  }

  return -1;
}
