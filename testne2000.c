
#include "testne2000.h"
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#if defined(__linux__)  &&  __GNU_LIBRARY__ == 1
#include <asm/io.h>			
#else
#include <sys/io.h>
#endif
#if !defined(__OPTIMIZE__)
#error You must compile this driver with "-O"!
#endif


u16 ip_sum_calc(u16 len_ip_header, u16 *buff)
{
	u16 word16;
	u32 sum=0;
	u16 i;
	unsigned char *l;
	printf("\nip header \n");
	l=buff;
	for (i=0;i<21;i++)
		printf("0x%02x ",(unsigned short) *(l+i));
	printf("\n");
	    
	for (i=0;i<len_ip_header;i=i+2){
		word16 =((l[i]<<8)&0xFF00)+(l[i+1]&0xFF);
		printf("%04x\n",word16);
		sum = sum + (u32) word16;  
	}
	    
	while (sum>>16)
		sum = (sum & 0xFFFF)+(sum >> 16);
	    
	sum = ~sum;
	
	return ((u16) sum);
}

u16 tcp_sum( u16 *buff,u16 len_tcp,unsigned short *src_addr,unsigned int *dst_addr)
{
	u16 word16,a,b,c,d;
	u32 sum=0;
	u16 i;
	unsigned char *l;

	printf("\nTCP header-> \n");
	l=buff;
	a=htonl(&src_addr[0])>>16;
	b=htonl(&src_addr[0]);
	c=htonl(&dst_addr[0])>>16;
	d=htonl(&dst_addr[0]);
	printf("\n");
    
	for (i=0;i<len_tcp;i=i+2){
        	word16 =((l[i]<<8)&0xFF00)+(l[i+1]&0xFF);
	        sum = sum + (u32) word16;  
    	}
	
	sum = sum + (u32) a;
	sum = sum + (u32) b;
	sum = sum + (u32) c;  
	sum = sum + (u32) d;
	sum = sum + len_tcp; 
	sum = sum + 0x06; // The TCP protocol

	while (sum>>16)
      		sum = (sum & 0xFFFF)+(sum >> 16);
    
	sum = ~sum;

	return ((u16) sum);
}


unsigned short checksum (int count, unsigned short *addr) {

	unsigned long sum = 0;
	
	count=count>>8;
	printf("Count =%x \n",count);
	while (count > 1) {
        	printf("%x ",(*addr<<8 &0xff00  ));
		sum += *addr++;      
        	count -= 2;
    	}

    	// Add left-over byte, if any
    	if (count > 0)
        	sum += * (unsigned char *) addr;

	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);
	
	return (unsigned short)htons(~sum);
}


struct	ether_addr {
	unsigned short addr[ETHER_ADDR_LEN];
};

struct ne {
  //dev_t devno;                          // Device number
  struct ether_addr hwaddr;               // MAC address

  unsigned short iobase;                // Configured I/O base
  unsigned short irq;                   // Configured IRQ
  unsigned short membase;               // Configured memory base
  unsigned short memsize;               // Configured memory size

  unsigned short asic_addr;             // ASIC I/O bus address
  unsigned short nic_addr;              // NIC (DP8390) I/O bus address
  
  //struct interrupt intr;                // Interrupt object for driver
  //struct dpc dpc;                       // DPC for driver

  unsigned short rx_ring_start;         // Start address of receive ring
  unsigned short rx_ring_end;           // End address of receive ring

  unsigned char rx_page_start;          // Start of receive ring
  unsigned char rx_page_stop;           // End of receive ring
  unsigned char next_pkt;               // Next unread received packet

  //struct event rdc;                     // Remote DMA completed event
  //struct event ptx;                     // Packet transmitted event
  //struct mutex txlock;                  // Transmit lock
};

static int ne_probe(struct ne *ne) {
	
	unsigned char byte,i;
	int s=0;
  
  	byte=inb(COMMAND);
	outb(0x21,COMMAND);
	printf("Command reg %x\n",byte);
	outb(0x48,DATACONFIGURATION);//58 Auto DMA
   	outb(0x00,REMOTEBYTECOUNT0);
	outb(0x00,REMOTEBYTECOUNT1);
   	outb(0x00,RECEIVECONFIGURATION);//0x15
	outb(0x20,TRANSMITPAGE); 
	outb(0x02,TRANSMITCONFIGURATION);
	outb(PSTART,PAGESTART); //Receive Buffer Ring
	outb(PSTOP,PAGESTOP); //Receive Buffer Ring
	rx_page_start=PSTART;
	next_pkt = rx_page_start+1 ;
	printf("PSTART: %02x\n",PSTART); 
	outb(PSTART,BOUNDARY);
	printf("BOUNDARY SET AT: %02x\n",inb(BOUNDARY)); 
   	outb(PSTOP,PAGESTOP);  //Stop Receive Buffer Ring
	outb(REG_PAGE1+CMD_STOP,COMMAND);//Page 1, Stop

	

	/*for (i = 0; i < ETHER_ADDR_LEN; i++)
		{
		 printf("PAR %x %x\n",inb(COMMAND + NE_P1_PAR0 + i),ne->hwaddr.addr[i]);
		}*/

	outb(PSTART+1,CURRENT);//CURR Pointer in 0x26
	outb(REG_PAGE0+CMD_START,COMMAND);//PAGE 0, Start
	outb(0xff,INTERRUPTSTATUS); //Clear Interrripts
	outb(0x0B,INTERRUPTMASK);//Enable Receive and Tx
	outb(0x00,TRANSMITCONFIGURATION);//ERROR, TX and RX
	//send a packet
	int count =100;
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
	ethhdr->h_proto= htons(ETH_P_IP); 
	memcpy(&ether_packet,ethhdr,sizeof(ethhdr_t));

	ipheader *IPHeader;
	IPHeader=malloc(sizeof(20));
	IPHeader->ip_v=0x4;
	IPHeader->ip_hl=0x5;
	IPHeader->ip_tos=0x2;
	IPHeader->ip_len=htons(86);
	IPHeader->ip_id=htons(0x6);
	IPHeader->ip_off=htons(0x0);
	IPHeader->ip_ttl=0x14;
	IPHeader->ip_p=0x6;
	IPHeader->ip_sum=0x0;
	IPHeader->ip_src=inet_addr("192.168.2.11");  //htonl(0x2);
	IPHeader->ip_dst=inet_addr("192.168.2.105");  //htonl(0x6);
	memcpy(&ether_packet[14],IPHeader,sizeof(ipheader));
	unsigned char *l;
	l=IPHeader;
	IPHeader->ip_sum=ip_sum_calc(20,(unsigned short *) l);

	printf("\nIP Checksum = %x\n",IPHeader->ip_sum);

	ether_packet[14+11]=IPHeader->ip_sum;
	ether_packet[14+10]=IPHeader->ip_sum>>8;
	
	tcp_header_t *TCPHeader;
	TCPHeader=malloc(sizeof(tcp_header_t));
	
	TCPHeader->src_port=htons(4100);
	TCPHeader->dst_port=htons(0x100);
	TCPHeader->seq=htonl(0x2233);
	TCPHeader->ack=htons(0x00);
	TCPHeader->darefla=htons(0x5000+SYN);
	printf("TCP flags = %x\n",TCPHeader->darefla );
	TCPHeader->window_size=htons(0x100);
	TCPHeader->checksum=0;
	TCPHeader->urgent_p=0;
	memcpy(&ether_packet[14+20],TCPHeader,sizeof(tcp_header_t));
	

	unsigned char *TCPdata;
	int datalength =46;
	TCPdata = malloc((datalength));	
	printf("TCPData= %x len= %i\n",TCPdata,datalength);
	for(i=0;i<datalength;i++)
		{
			TCPdata[i]=i;
			
		}
	printf("TCP data: \n");
	for(i=0;i<datalength;i++)
		{
		printf("0x%X ",TCPdata[i]);
		}
		
	memcpy(&ether_packet[14+20+20],TCPdata,datalength);
        TCPHeader->checksum=tcp_sum(&ether_packet[14+20],20+datalength,IPHeader->ip_src,IPHeader->ip_dst);
	printf("\nTCP Checksum = %x\n",TCPHeader->checksum);
	ether_packet[14+20+17]=TCPHeader->checksum;
	ether_packet[14+20+16]=TCPHeader->checksum>>8;
	
	for (i=0;i<count;i++)
		outw(ether_packet[i],IOPORT);

	for (i=0;i<count;i++)
		 printf("%x ",*(ether_packet+i));

	printf("\nIP Header\n");
	
	for (i=0;i<21;i++)
		printf("%hx ",(unsigned short) *(l+i));


	s=inb(INTERRUPTSTATUS);

	while(s!=0x40) //Remote DMA complete?
		s=inb(INTERRUPTSTATUS);

	printf("\nInterrup status: %x\n",s);
	
	outb(TRANSMITBUFFER,TRANSMITPAGE);
	outb(count & 0xff,TRANSMITBYTECOUNT0);
	outb(count >>8 ,TRANSMITBYTECOUNT1);
	outb(0x26,COMMAND);//PAge 0, remote write,Transmite, start 

	return 1;
}

int main(char argc, char **argv)
{
	char e;
	unsigned char s,d;
	unsigned int data[100],i=0;

	if (iopl(3)) {perror("iopl error"); exit(1);}

	struct ne *NIC;

	NIC = (struct ne *) malloc(sizeof(struct ne));
	nic_reset();par();
	ne_probe(NIC);	


	
  	
	while(e!='s'){
		//scanf("%c",&e);
		irq_event(); 
	}
	
	printf("Exiting..\n");
	
	return 0;
}



static int irq_event()
{
	
	unsigned char irq_value;

	outb(CMD_NO_DMA | CMD_START,INTERRUPTSTATUS);

	while((irq_value = inb(COMMAND + P0_ISR))!=0)
		{
			
			printf("IRQ=	%02x %02x\n",irq_value,irq_value & ISR_VL_PTX);
			outb(irq_value,COMMAND + P0_ISR);
		
			if(irq_value & ISR_VL_OVW){
				printf("Overflow\n");
				
				}
			if(irq_value & ISR_VL_PRX){
				printf("######### Packet %i Received #########\n",k);
				received_packet();
				
				}

			if(irq_value & ISR_VL_PTX){		
				printf(" Packet Transmitted ##########\n");
				
				}
		
			if(irq_value & ISR_VL_RDC)
				printf("DMA complete\n");

			 outb( CMD_NO_DMA | CMD_START,INTERRUPTSTATUS);	
		}
	return 0;
}


int nic_reset()
{
	unsigned char c; 

	c = inb(COMMAND + NS_RESET);
	outb(c,COMMAND + NS_RESET);
	printf("Reset %02x\n",c);
}

int par(){
	
	int j;
	unsigned char c;

	outb( 0x40,COMMAND);//PAGE 1
	outb( 0x00,COMMAND+NE_P1_PAR0);
	outb( 0x24,COMMAND+NE_P1_PAR1);
	outb( 0x8c,COMMAND+NE_P1_PAR2);
	outb( 0x73,COMMAND+NE_P1_PAR3);
	outb( 0xef,COMMAND+NE_P1_PAR4);
	outb( 0x9c,COMMAND+NE_P1_PAR5);
}

int received_packet(){
	
	dp8390_pkt_hdr header;
	unsigned short packet_ptr;
	
	outb(CMD_NO_DMA | CMD_START | REG_PAGE0, COMMAND);
	printf("Inside Received Packet Function. Next Packet:	%02x ",next_pkt);
	outb(REG_PAGE1, COMMAND);
	printf("Current Page=%02x\n",inb(COMMAND+NE_P1_CURR));		
	while(next_pkt !=inb(COMMAND+NE_P1_CURR)){
		printf("Next Packet= %02x CURR=%02x\n",next_pkt,inb(COMMAND+NE_P1_CURR));
		packet_ptr= next_pkt*256;
		printf("Next Packet= %02x CURR=%02x, Packet Ptr=%02x\n",next_pkt,inb(COMMAND+NE_P1_CURR),packet_ptr);
		readmem(&header,packet_ptr,sizeof(dp8390_pkt_hdr));
		printf("Next Packet Pointer:	0x%02x\n",header.NextPacketPointer);
		printf("Receive Status:		0x%02x\n",header.ReceiveStatus);
		printf("Packet Length:		%i bytes\n\n",header.length);

		outb(REG_PAGE1, COMMAND);
		printf("Current Page=%02x\n",inb(COMMAND+NE_P1_CURR));
		}
			
	outb(REG_PAGE0, COMMAND);
	return 0;
}

int readmem(dp8390_pkt_hdr *dest,unsigned short src ,int n){
	
 	int i;
	char *tmp = (char*)dest,m;
	unsigned char pl0,pl1;
	unsigned short pal;
	unsigned char next_pack;
	
	n=4;
	

	printf("\nInside Readmem:\n\n");
	outb(REG_PAGE0, COMMAND);
	printf("BOUNDARY:		0x%02x\n",inb(BOUNDARY));
	outb(inb(BOUNDARY),REMOTESTARTADDRESS1);
	outb(0X00,REMOTESTARTADDRESS0);
	outb(0X04, REMOTEBYTECOUNT0); //Read 4 bytes	
	outb(0X00, REMOTEBYTECOUNT1);
	outb(0x0A, COMMAND);
	/*outb(REG_PAGE2, COMMAND);
	printf("PSTART %02x\n",inb(COMMAND+1));
	printf("PSTOP %02x\n",inb(COMMAND+2));

	
	
	
	/*outb(0x0,REMOTESTARTADDRESS0);
	usleep(2000);
	outb(0x26,REMOTESTARTADDRESS1);
	usleep(2000);	*/	
	
	/*printf("RBC0 %02x\n",n & 0xff);
	printf("RBC1 %02x\n",n >> 8);
	printf("RSTA0 %02x\n",src&0xff);
	printf("RSTA1 %02x\n",src>>8);*/
	printf("Receive Status:		0x%02x \n", dest->ReceiveStatus=inb(IOPORT));
	next_pkt=inb(IOPORT);
	dest->NextPacketPointer=next_pkt;
	printf("Next Packet Pointer:	0x%02x \n", next_pkt);
	pl0=inb(IOPORT);
	dest->NextPacketPointer=next_pkt;
	printf("Receive Byte Count 0:	0x%02x \n",pl0 );
	pl1=inb(IOPORT);
	printf("Receive Byte Count 1:	0x%02x \n", pl1);
	pal=((pl1<<8)& 0xff00)+pl0-sizeof(dp8390_pkt_hdr);
	dest->length=pal;
	printf("Packet Length:		%i bytes\n\n",pal);	
	

	outb(inb(BOUNDARY),REMOTESTARTADDRESS1);
	outb(0X04,REMOTESTARTADDRESS0);
	outb(pal&0xff, REMOTEBYTECOUNT0);	
	outb((pal>>8)&0xff, REMOTEBYTECOUNT1);
	outb( 0x0A, COMMAND);
	for(i=0;i<pal;i++){
		m=inb(IOPORT);
		printf("0x%02hhx ",m);
	}
	
		
	while(!(inb(INTERRUPTSTATUS)) & 0x40);
	
	outb(next_pkt,BOUNDARY);

	if((next_pkt-1) < PSTART)
		outb(PSTOP-1,BOUNDARY);
		
	outb(0x40,INTERRUPTSTATUS);//Clear the interrupt
	outb(0x02,INTERRUPTSTATUS);//Clear the interrupt
	outb(REG_PAGE2, COMMAND);
	printf("\n\nCurrent Pointer:	%02x \n\n",inb(CURRENT));
	
	outb(REG_PAGE0, COMMAND);
	printf("\n\nCurrent ISR status:	%02x \n\n",inb(INTERRUPTSTATUS));
	if(k>5){
		while(1){printf(".");};
		}
	else
	 k++;
	
	return n;

}

