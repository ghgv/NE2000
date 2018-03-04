#include "nic.h"
#include "testne2000.h"

const char *progress = "-\\|/";

int send_raw_packet(unsigned char *packet,int len,int proto)
{
  int count =100,i,s,conteo;

  printf("\n########### Inside Send Raw Packet ########### \n");


  outb(REG_PAGE0, COMMAND);
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

  printf("COUNT: %i  ISR: %x\n",count,inb(INTERRUPTSTATUS));
  outb(0xff,INTERRUPTSTATUS);
  printf("TRANSMITBUFFER: %02x  ISR: %x\n",TRANSMITBUFFER,inb(INTERRUPTSTATUS));
  outb(count & 0xff,REMOTEBYTECOUNT0);
  outb(count >>8 , REMOTEBYTECOUNT1);
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



  for (i=0;i<(14+len);i++){
    outw(ether_packet[i],IOPORT);
    conteo++;
  }
 while(conteo<100){
  outw(0x0000,IOPORT);
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


  	printf("\nInterrup status: 0x%x\n",s);

  	outb(TRANSMITBUFFER,TRANSMITPAGE);
  	outb(count & 0xff,TRANSMITBYTECOUNT0);
  	outb(count >>8 ,TRANSMITBYTECOUNT1);
  	outb(0x26,COMMAND);//PAge 0, remote write,Transmite, start

    printf("\n########### End Send Raw Packet ########### \n");

    return 0;
}