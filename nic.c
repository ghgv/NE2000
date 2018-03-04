#include <stdio.h>
#include "nic.h"
#include "testne2000.h"

const char *progress = "-\\|/";

int send_raw_packet(unsigned char *packet,int len,int proto)
{
  int count =100,i,s,conteo;

  printf("\n########### Inside Send Raw Packet ########### \n");

  count = len+sizeof(ethhdr_t);
  outb(REG_PAGE0, COMMAND);

  #ifdef DEBUG
  printf("\nWaiting for Tx to finish [-]\n");


  while(inb(COMMAND)==0x26)
  {
    if(i % 100 == 0)
       {
           printf("%c%c%c]",8,8,progress[(i/100) % 4]);
           fflush(stdout);
       }
       i++;
    }

  #endif


  printf("COUNT: %i  ISR: %x\n",count,inb(INTERRUPTSTATUS));
  outb(0xff,INTERRUPTSTATUS); // Clear all interrupts
  printf("TRANSMITBUFFER: %02x  ISR: %x\n",TRANSMITBUFFER,inb(INTERRUPTSTATUS));
  if(count<64){
    outb(64 & 0xFF,REMOTEBYTECOUNT0);
    outb((64 >>8)&0xFF , REMOTEBYTECOUNT1);
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
 while(conteo<64){
  outw(0x0000,IOPORT);
  printf("######################%i \n",conteo);
  conteo++;
 }




  for (i=0;i<(14+len);i++)
    printf("0x%02x ",*(ether_packet+i));

 printf("\nWaiting for Interrup [-]");
    s=inb(INTERRUPTSTATUS);
    i=0;
  	while(s!=0x40) //Remote DMA complete?
      {
      s=inb(INTERRUPTSTATUS);
      if(i % 100 == 0)
         {
             printf("%c%c%c]",8,8,progress[(i/100) % 4]);
             fflush(stdout);
         }
         i++;
      }


  	printf("\nInterrup status: 0x%x, count %i %x %x\n",s, count,count & 0xff,count >>8);

  	outb(TRANSMITBUFFER,TRANSMITPAGE);
    if(count<64){
      outb(64 & 0xff,TRANSMITBYTECOUNT0);
    	outb(64 >>8 ,TRANSMITBYTECOUNT1);
    }
    else
    {
      outb(count & 0xff,TRANSMITBYTECOUNT0);
      outb(count >>8 ,TRANSMITBYTECOUNT1);

    }
  	outb(0x26,COMMAND);//PAge 0, remote write,Transmite, start

    printf("\n########### End Send Raw Packet ########### \n");

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
