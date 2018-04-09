#include "sys/mbuf.h"
#include <stdio.h>
#include <pthread.h>
#include "nic.h"
#include "arp.h"
#include "testne2000.h"

const char *progress = "-\\|/";
extern pthread_mutex_t lock;
extern debugnic;


int send_raw_packet(unsigned char *packet,int len,int proto)
{


  pthread_mutex_lock(&lock);

  int count =100,i,s,conteo;
  if(debugnic==1)
  printf("\n########### Inside Send Raw Packet ########### \n");

//  printf("\n########### Inside Send Raw Packet ########### \n");
//  printf(" Packet length %i %i\n",len,sizeof(ethhdr_t));

  count = len+sizeof(ethhdr_t);
  outb(REG_PAGE0, COMMAND);

  outb(0x00,TRANSMITCONFIGURATION);//ERROR, TX and RX

  if(debugnic==1)
  printf("\nWaiting for Tx to finish [-]\n");
  while(inb(COMMAND)==0x26)
  {
    if(i % 100 == 0)
       {
         if(debugnic==1){
           printf("%c%c%c]",8,8,progress[(i/100) % 4]);
           fflush(stdout);
         }
       }
       i++;
    }


  if(debugnic==1)
  printf("\nCount: %i  ISR: %x\n",count,inb(INTERRUPTSTATUS));
  outb(0xff,INTERRUPTSTATUS); // Clear all interrupts
  if(debugnic==1)
  printf("TRANSMITBUFFER: %02x  ISR: %x\n",TRANSMITBUFFER,inb(INTERRUPTSTATUS));
  if(count<60){
    outb(count & 0xFF,REMOTEBYTECOUNT0);
    outb((count >>8)&0xFF , REMOTEBYTECOUNT1);
    }
  else
    {
    outb(count & 0xFF,REMOTEBYTECOUNT0);
    outb((count >>8)&0xFF , REMOTEBYTECOUNT1);
    }
  outb(0x00, REMOTESTARTADDRESS0);
  outb(TRANSMITBUFFER, REMOTESTARTADDRESS1);
  outb(0x12,COMMAND);//START y Remote Write



  ethhdr_t *ethhdr;
	ethhdr=malloc(sizeof(ethhdr_t));
	ethhdr->h_dest[0]=0x00;
	ethhdr->h_dest[1]=0x24;
	ethhdr->h_dest[2]=0x8c;
	ethhdr->h_dest[3]=0x73;
	ethhdr->h_dest[4]=0xef;
	ethhdr->h_dest[5]=0x9b;
	ethhdr->h_source[0]=0x00;
	ethhdr->h_source[1]=0x24;
	ethhdr->h_source[2]=0x8c;
	ethhdr->h_source[3]=0x73;
	ethhdr->h_source[4]=0xef;
	ethhdr->h_source[5]=0x9c;
	ethhdr->h_proto= htons(proto);
	memcpy(&ether_packet,ethhdr,sizeof(ethhdr_t));
	memcpy(&ether_packet[14],packet,len);



  for (i=0;i<(count);i++){
    outw(ether_packet[i],IOPORT);
    conteo++;
  }
  int h=0;
 while(conteo<64){
  outw(0x0000,IOPORT);
  conteo++;
  }
  //for (i=0;i<(14+len);i++)
  //  printf("%i.0x%02X "S,i,*(ether_packet+i));
    if(debugnic==1)
    printf("\nWaiting for Interrupt in send_raw_packet [-]");
    s=inb(INTERRUPTSTATUS);
    i=0;
  	while((s&0x40)!=0x40) //Remote DMA complete?
      {
      s=inb(INTERRUPTSTATUS);
      if(i % 100 == 0)
         {
             printf("%c%c%c]",8,8,progress[(i/100) % 4]);
             printf("%x ",s);
             fflush(stdout);
         }
         i++;
      }
      if(debugnic==1)
      printf("\nWaiting for Interrupt in send_raw_packet after\n");
    //printf("\n");
//  	printf("\nInterrup status: 0x%x, count %i %x %x\n",s, count,count & 0xff,count >>8);
  	outb(TRANSMITBUFFER,TRANSMITPAGE);
    if(count<60){
      outb(60 & 0xff,TRANSMITBYTECOUNT0);
    	outb(60 >>8 ,TRANSMITBYTECOUNT1);
    }
    else
    {
      outb(count & 0xff,TRANSMITBYTECOUNT0);
      outb(count >>8 ,TRANSMITBYTECOUNT1);

    }
  	outb(0x26,COMMAND);//PAge 0, remote write,Transmite, start

  //  printf("\n########### End Send Raw Packet ########### \n");
    enviados++;
    pthread_mutex_unlock(&lock);
    return 0;
}


int inet_aton(const char *cp,  in_addr *ap)
{
    int dots = 0;
    register unsigned long acc = 0, addr = 0;

    do {
	register char cc = *cp;

	switch (cc) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    acc = acc * 10 + (cc - '0');
	    break;

	case '.':
	    if (++dots > 3) {
		return 0;
	    }
	    /* Fall through */

	case '\0':
	    if (acc > 255) {
		return 0;
	    }
	    addr = addr << 8 | acc;
	    acc = 0;
	    break;

	default:
	    return 0;
	}
    } while (*cp++) ;

    /* Normalize the address */
    if (dots < 3) {
	addr <<= 8 * (3 - dots) ;
    }

    /* Store it if requested */
    if (ap) {
	ap->s_addr = htonl(addr);
    }

    return 1;
}

int par(){

	int j;
	unsigned char c;

 /*Assign the MAC address*/

	outb( 0x40,COMMAND);//PAGE 1
	outb( 0x00,COMMAND+NE_P1_PAR0);
	outb( 0x24,COMMAND+NE_P1_PAR1);
	outb( 0x8c,COMMAND+NE_P1_PAR2);
	outb( 0x73,COMMAND+NE_P1_PAR3);
	outb( 0xef,COMMAND+NE_P1_PAR4);
	outb( 0x9c,COMMAND+NE_P1_PAR5);
}

int nic_reset()
{
	unsigned char c;

  outb(REG_PAGE0,COMMAND);
	c = inb(COMMAND + NS_RESET);
	outb(c,COMMAND + NS_RESET);
  outb(0xff,INTERRUPTSTATUS);
  //sleep(1);
  printf("Reset 0x%02x\n",c);


}

void overflow()
{

  {
  			/*overflow */
  			printf("OVER");
        outb(CMD_STOP,COMMAND);
  			outb(0x0,REMOTEBYTECOUNT0);
        outb(0x0,REMOTEBYTECOUNT0);
        outb(2,TRANSMITCONFIGURATION);
        while(1);
}
}

void readmem(dp8390_pkt_hdr *dest,unsigned short src ,int n){

 	int i;
	char *tmp = (char*)dest,m;
	unsigned char pl0,pl1,*pos;
	unsigned short pal;
	unsigned char next_pack;
  unsigned char BNDY;
	raw_packet_t *newpack;
  unsigned char pac;
	int len;



	n=4;

	outb(REG_PAGE0, COMMAND); //to read the BOUNDARY
  BNDY=inb(NE_P0_BOUNDARY);
  if(debugnic==1)
  printf("BOUNDARY: 0x%X\n ",BNDY);
	outb(BNDY,REMOTESTARTADDRESS1);
	outb(0X00,REMOTESTARTADDRESS0);
	outb(0X04, REMOTEBYTECOUNT0); //Read 4 bytes
	outb(0X00, REMOTEBYTECOUNT1);
	outb(CMD_REMOTE_READ+CMD_START, COMMAND);
	dest->ReceiveStatus=inb(IOPORT);
  if(debugnic==1)
  printf("Status: 0x%X\n ",dest->ReceiveStatus);
	next_pkt=inb(IOPORT);
  if(debugnic==1)
  printf("Next Packet Pointer page: 0x%X \n ",next_pkt);
	dest->NextPacketPointer=next_pkt;
	pl0=inb(IOPORT);
	pl1=inb(IOPORT);
	pal=((pl1<<8)& 0xff00)+pl0-sizeof(dp8390_pkt_hdr);
	dest->length=pal;


	outb(inb(NE_P0_BOUNDARY),REMOTESTARTADDRESS1);
	outb(0X04,REMOTESTARTADDRESS0);//Four bytes ahead
	outb(pal&0xff, REMOTEBYTECOUNT0);
	outb((pal>>8)&0xff, REMOTEBYTECOUNT1);
	outb( CMD_REMOTE_READ+CMD_START, COMMAND);

  if(debugnic==1)
  printf("LEN: 0x%X %i\n ",pal,pal);
  if(pal>1200)
    return;
	for(i=0;i<pal;i++){
		ether_packet[i]=inb(IOPORT);
    if(debugnic==1)
		printf("0x%02hhx ",ether_packet[i]);
			}

	newpack=(raw_packet_t	*)malloc(sizeof(raw_packet_t));

	newpack->len=pal;
	newpack->next=NULL;
	newpack->prev=pack;
	newpack->data=(unsigned char *)malloc(pal);

	pos=newpack->data;

	memcpy(pos,&ether_packet[0],pal);
	pack->next=newpack;

	paquetes++;

	while(!(inb(INTERRUPTSTATUS)) & 0x40);

  outb(0x40,INTERRUPTSTATUS);//Clear the interrupt

  outb(next_pkt,BOUNDARY);

	if((next_pkt-1) < PSTART)
		outb(PSTOP-1,BOUNDARY);

  outb(REG_PAGE1, COMMAND); //to read the CURR
  if(debugnic==1)
  printf("\nNext PP 0x%02X ,  CURR= %02x \n",next_pkt,inb(COMMAND+NE_P1_CURR));

  outb(REG_PAGE0, COMMAND); //to read the CURR
	outb(0x02,INTERRUPTSTATUS);//Clear the interrupt
	outb(0x01,INTERRUPTSTATUS);//Clear the interrupt
	//outb(REG_PAGE2, COMMAND);//fix  Next packet 46?
	outb(REG_PAGE0, COMMAND);
	//printf("\n\nCurrent ISR status:	%02x \n\n",inb(INTERRUPTSTATUS));
}

int received_packet(){

	dp8390_pkt_hdr header;

	unsigned short packet_ptr;


	outb(CMD_NO_DMA | CMD_START | REG_PAGE0, COMMAND);
  outb(REG_PAGE1, COMMAND);

  while(next_pkt !=inb(COMMAND+NE_P1_CURR)){
    if(debugnic==1)
  	printf("Next Packet= %02x CURR=%02x\n",next_pkt,inb(COMMAND+NE_P1_CURR));
  	packet_ptr= next_pkt*256;
		readmem(&header,packet_ptr,sizeof(dp8390_pkt_hdr));
    ++pkt;
    if(debugnic==1)
    printf("pkt=0x%02X\n",pkt);
    outb(REG_PAGE1, COMMAND);
    if(pkt==15)
        while(1);
		}

	return paquetes;
}

int pool(){

while(1)
{
irq_event();
}


}


static int irq_event()
{
pthread_mutex_lock(&lock);
  unsigned char irq_value;
  //while(1)
  {
  //  printf("\n######### IRQ= %02x #########\n",inb(COMMAND + P0_ISR));
  outb(CMD_NO_DMA | CMD_START,INTERRUPTSTATUS);

	while((irq_value = inb(COMMAND + P0_ISR))!=0)
		{
      if(debugnic==1)
      printf("IRQ event: %02x %02x\n",irq_value,irq_value & ISR_VL_PTX);
			outb(irq_value,COMMAND + P0_ISR);

			if(irq_value & ISR_VL_OVW){
        if(debugnic==1)
				printf("Overflow\n");
        //overflow();
        while(1);
				}
			if(irq_value & ISR_VL_PRX){
        if(debugnic==1)
				printf("######### Packet Received #########\n",k);
				received_packet();
				packet_selector();
        outb(REG_PAGE0, COMMAND);
				}

			if(irq_value & ISR_VL_PTX){
        if(debugnic==1)
				printf(" Packet Transmitted ##########\n");
        outb(REG_PAGE0, COMMAND);
				}

			if(irq_value & ISR_VL_RDC){
        if(debugnic==1)
        printf("DMA completed\n");
        outb(0x40,COMMAND + P0_ISR);
        outb(REG_PAGE0, COMMAND);
      }


			 outb( CMD_NO_DMA | CMD_START,INTERRUPTSTATUS);
		}

  }
  pthread_mutex_unlock(&lock);
	return 0;
}


int packet_selector()
{
	int i,j=0;
	arp_frame *arpa;
	unsigned short EthernetP;
	raw_packet_t *pack,*prev,*post;
	int s=0;

  if(debugnic==1)
	printf("\n######## Packet selector[%i]####_# 0x%X 0x%X\n",paquetes,root,root->next);
	for(pack=root;pack!=NULL;pack=pack->next)
		{
      ++j;
      if(debugnic==1)
			printf("%i. Curr: 0x%02X Prev: 0x%02X Next: 0x%02X\n",j,pack,pack->prev,pack->next);
		}
	for(pack=root;pack!=NULL;pack=pack->next)
		{
      ++s;
      if(debugnic==1)
			printf("\nPacket %i length: %i\n",s,pack->len);
      if(debugnic==1)
			printf("Curr: 0x%02X Next: 0x%02X\n",pack,pack->next);
      if(debugnic==1)
			printf("Data:\n ");
			prev=pack->prev;
      if (pack->len==0)
       continue;
			 for(i=0;i<pack->len;i++)
        {
          if(debugnic==1)
          printf("0x%02X ",(unsigned char )(pack->data[i]));
        }


			memcpy(&EthernetP,&pack->data[12],sizeof(unsigned short));


	switch (ntohs(EthernetP)) {
		case ETH_P_IP:
            if(debugnic==1)
						printf("\nIPv4 Packet:\n");
						mbuf(pack);
            if(debugnic==1)
						printf("prev : 0x%X 0x%X\n",prev,pack->next);fflush(stdout);
						prev->next=pack->next;
						if(pack->next!=NULL)
							{
							post=pack->next;
							post->prev=pack->prev;
							}
						free(pack->data);
						free(pack);
						//decode_ip(&ether_packet);
						break;
		case ETH_P_ARP:
            if(debugnic==1)
						printf("\n\n ARP Packet:\n");
						decode_arp(pack);
            if(debugnic==1)
						printf("prev : 0x%X 0x%X\n",prev,pack->next);fflush(stdout);
						prev->next=pack->next;
						if(pack->next!=NULL)
							{
							post=pack->next;
							post->prev=pack->prev;
							}
						free(pack->data);
						free(pack);
						break;
	}
}
if(debugnic==1)
printf("\n######## end of packet selector####_# %x %x %x\n",prev,prev->prev,prev->next);

root->next=NULL;
root->prev=NULL;
root->len=0;
pack=root;
paquetes=0;

}
