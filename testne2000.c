
#include "testne2000.h"
#include <unistd.h>
#include <string.h>

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

#define COMMAND 0xec00
#define DATACONFIGURATION COMMAND+0x0E


#define PAGESTART  COMMAND+01
#define PAGESTOP  COMMAND+02
#define BOUNDARY  COMMAND+03
#define TRANSMITSTATUS  COMMAND+04
#define TRANSMITPAGE  COMMAND+04
#define TRANSMITBYTECOUNT0  COMMAND+0x05
#define NCR  COMMAND+05
#define TRANSMITBYTECOUNT1  COMMAND+0x06
#define INTERRUPTSTATUS  COMMAND+0x07
#define CURRENT  COMMAND+07
#define REMOTESTARTADDRESS0 COMMAND+0x08
#define CRDMA0  COMMAND+0x08
#define REMOTESTARTADDRESS1 COMMAND+0x09
#define CRDMA1  COMMAND+0x09
#define REMOTEBYTECOUNT0  COMMAND+0x0A
#define REMOTEBYTECOUNT1  COMMAND+0x0B
#define RECEIVESTATUS  COMMAND+0x0C
#define RECEIVECONFIGURATION  COMMAND+0x0C
#define TRANSMITCONFIGURATION  COMMAND+0x0D
#define FAE TALLY  COMMAND+0x0D
#define DATACONFIGURATION  COMMAND+0x0E
#define CRC TALLY  COMMAND+0x0E
#define INTERRUPTMASK  COMMAND+0x0F
#define MISS PKT TALLY  COMMAND+0x0F
#define IOPORT COMMAND+0x10

#define PSTART  0x46
#define PSTOP 0x80
#define TRANSMITBUFFER 0x40


#define NE_P1_PAR0      0x01           // Physical Address Register 0
#define NE_P1_PAR1      0x02           // Physical Address Register 1
#define NE_P1_PAR2      0x03           // Physical Address Register 2
#define NE_P1_PAR3      0x04           // Physical Address Register 3
#define NE_P1_PAR4      0x05           // Physical Address Register 4
#define NE_P1_PAR5      0x06           // Physical Address Register 5
#define NE_P1_CURR      0x07           // Current RX ring-buffer page
#define NE_P1_MAR0      0x08           // Multicast Address Register 0
#define NE_P1_MAR1      0x09           // Multicast Address Register 1
#define NE_P1_MAR2      0x0A           // Multicast Address Register 2
#define NE_P1_MAR3      0x0B           // Multicast Address Register 3
#define NE_P1_MAR4      0x0C           // Multicast Address Register 4
#define NE_P1_MAR5      0x0D           // Multicast Address Register 5
#define NE_P1_MAR6      0x0E           // Multicast Address Register 6
#define NE_P1_MAR7      0x0F           // Multicast Address Register 7
#define	ETHER_ADDR_LEN		6
#define MTU 1560




typedef unsigned short u16;
typedef unsigned long u32;
u16 ip_sum_calc(u16 len_ip_header, u16 *buff)
{
u16 word16;
u32 sum=0;
u16 i;
printf("\nip header \n");
unsigned char *l;
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
printf("\nTCP header-> \n");
unsigned char *l;
l=buff;
/*for (i=0;i<len_tcp;i++)
	printf("0x%02x ",(unsigned short) *(l+i));
printf("\n");
printf("\nIP addr-> \n");*/
a=htonl(&src_addr[0])>>16;
//printf("0x%04x ", a);
b=htonl(&src_addr[0]);
//printf("0x%04x \n", b );
c=htonl(&dst_addr[0])>>16;
//printf("0x%04x ", c);
d=htonl(&dst_addr[0]);
//printf("0x%04x \n", d );
printf("\n");
    
    for (i=0;i<len_tcp;i=i+2){
        word16 =((l[i]<<8)&0xFF00)+(l[i+1]&0xFF);
	//printf("%04x\n",word16);
        sum = sum + (u32) word16;  
    }
	sum = sum + (u32) a;
	sum = sum + (u32) b;
	sum = sum + (u32) c;  
	sum = sum + (u32) d;
//printf("len 0x%04x %i \n", len_tcp,len_tcp );
	sum = sum + len_tcp; 
	sum = sum + 0x06; 
//printf("Sum 0x%04x  \n", sum );
	

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
 
  // Reset
  //msleep(5000);
   byte=inb(COMMAND);
	outb(0x21,COMMAND);
 // sleep(1);
	printf("Command reg %x\n",byte);
	outb(0x58,DATACONFIGURATION);
   outb(0x00,REMOTEBYTECOUNT0);
	outb(0x00,REMOTEBYTECOUNT1);
   outb(0x00,RECEIVECONFIGURATION);
	outb(0x20,TRANSMITPAGE); 
   outb(0x02,TRANSMITCONFIGURATION);
	outb(0x26,PAGESTART);
	outb(0x26,BOUNDARY);
   outb(0x40,PAGESTOP); 
	outb(0x61,COMMAND);

	

	for (i = 0; i < ETHER_ADDR_LEN; i++)
		{
		 printf("PAR %x %x\n",inb(COMMAND + NE_P1_PAR0 + i),ne->hwaddr.addr[i]);
		}
   // Actually the ethernet address is overwritten later on wit 0xA5A5. You can fix it. I will put a useful payload later
	outb(0x26,CURRENT);
	outb(0x22,COMMAND);
	outb(0xff,INTERRUPTSTATUS); 
	outb(0x0B,INTERRUPTMASK);
	outb(0x00,TRANSMITCONFIGURATION);

//send a packet
	int count =100;
	outb(count & 0xff,REMOTEBYTECOUNT0);
	outb(count >>8 , REMOTEBYTECOUNT1);
	outb(0x00, REMOTESTARTADDRESS0);
	outb(TRANSMITBUFFER, REMOTESTARTADDRESS1);	
	outb(0x12,COMMAND);
	ethhdr_t *ethhdr;
	ethhdr=malloc(sizeof(ethhdr_t));
	ethhdr->h_dest[0]=0x00;
	ethhdr->h_dest[1]=0x24;
	ethhdr->h_dest[2]=0x8c;
	ethhdr->h_dest[3]=0x73;
	ethhdr->h_dest[4]=0xef;
	ethhdr->h_dest[5]=0x9b;
	ethhdr->h_source[0]=0x6;
	ethhdr->h_source[1]=0x7;
	ethhdr->h_source[2]=0x8;
	ethhdr->h_source[3]=0x9;
	ethhdr->h_source[4]=0xA;
	ethhdr->h_source[5]=0xB;
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
	IPHeader->ip_src=inet_addr("192.168.1.11");  //htonl(0x2);
	IPHeader->ip_dst=inet_addr("192.168.2.105");  //htonl(0x6);
	memcpy(&ether_packet[14],IPHeader,sizeof(ipheader));
	unsigned char *l;
	l=IPHeader;
	IPHeader->ip_sum=ip_sum_calc(20,(unsigned short *) l);
	//Test IPHeader->ip_sum=0x21a6;
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

	int s=0;
	s=inb(INTERRUPTSTATUS);
	while(s!=0x40)
		s=inb(INTERRUPTSTATUS);
	printf("\nInterrup status %x\n",s);
	outb(0x40,INTERRUPTSTATUS);
	s=inb(INTERRUPTSTATUS);
	printf("Interrup status %x\n",s);
	outb(TRANSMITBUFFER,TRANSMITPAGE);
	outb(count & 0xff,TRANSMITBYTECOUNT0);
	outb(count >>8 ,TRANSMITBYTECOUNT1);
	outb(0x26,COMMAND);

return 1;
}

int main(char argc, char **argv)
{
char e;
if (iopl(3)) {perror("iopl error"); exit(1);}
	struct ne *NIC;
	NIC = (struct ne *) malloc(sizeof(struct ne));

  ne_probe(NIC);
  while(e!='s')
	{
	printf("Enter command:\n");
	scanf("%c",&e);
	}
 printf("Exiting..\n");
  return 0;
}
