
#include "testne2000.h"
#include "arp.h"
#include "ip.h"
#include "sys/socket.h"
#include "sys/mbuf.h"
#include "commands.h"
#include "timer.h"

#include <unistd.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>



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

#include <pthread.h>

extern pthread_mutex_t lock;
sockaddr_in_t dest;

int debugtcp =0;
int debugip=0 ;
int debugsocket=0;
int debugmbuf=0;
int debugnic=0;
int debugicmp=0;
int debugarp=0;



const char *progresso = "-\\|/";


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


int kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

u16 ip_sum_calc(u16 len_ip_header, u16 *buff)
{
	u16 word16;
	u32 sum=0;
	u16 i;
	unsigned char *l;

	l=buff;


	#ifdef DEBUG
	for (i=0;i<21;i++)
		printf("0x%02x ",(unsigned short) *(l+i));
	printf("\n");
	#endif

	for (i=0;i<len_ip_header;i=i+2){
		word16 =((l[i]<<8)&0xFF00)+(l[i+1]&0xFF);
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

	//*********TCP checksum
	l=buff;
	a=htonl(&src_addr[0])>>16;
	b=htonl(&src_addr[0]);
	c=htonl(&dst_addr[0])>>16;
	d=htonl(&dst_addr[0]);


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
	outb(CMD_STOP + CMD_NO_DMA,COMMAND);//Stop amd Abort
	printf("Command Register:		%x\n",byte);
	outb(0x48,DATACONFIGURATION);//58 Auto DMA
  outb(0x00,REMOTEBYTECOUNT0);
	outb(0x00,REMOTEBYTECOUNT1);
  outb(0x15,RECEIVECONFIGURATION);//0x15 o 0x00 o 17
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
	IPHeader->ip_dst=inet_addr("192.168.2.102");  //htonl(0x6);
	memcpy(&ether_packet[14],IPHeader,sizeof(ipheader));
	unsigned char *l;
	l=IPHeader;
	IPHeader->ip_sum=ip_sum_calc(20,(unsigned short *) l);

	//printf("\nIP Checksum = %x\n",IPHeader->ip_sum);

	ether_packet[14+11]=IPHeader->ip_sum;
	ether_packet[14+10]=IPHeader->ip_sum>>8;

	tcp_header_t *TCPHeader;
	TCPHeader=malloc(sizeof(tcp_header_t));

	TCPHeader->src_port=htons(4100);//It is 0x1004
	TCPHeader->dst_port=htons(256);//it is 0x100
	TCPHeader->seq=htonl(0x2233);
	TCPHeader->ack=htons(0x00);
	TCPHeader->darefla=htons(0x5000+SYN);
	TCPHeader->window_size=htons(0x100);
	TCPHeader->checksum=0;
	TCPHeader->urgent_p=0;
	memcpy(&ether_packet[14+20],TCPHeader,sizeof(tcp_header_t));


	unsigned char *TCPdata;
	int datalength =46;
	TCPdata = malloc((datalength));

	for(i=0;i<datalength;i++)
		{
			TCPdata[i]=i;

		}

	memcpy(&ether_packet[14+20+20],TCPdata,datalength);
        TCPHeader->checksum=tcp_sum(&ether_packet[14+20],20+datalength,IPHeader->ip_src,IPHeader->ip_dst);
	//printf("\nTCP Checksum = %x\n",TCPHeader->checksum);
	ether_packet[14+20+17]=TCPHeader->checksum;
	ether_packet[14+20+16]=TCPHeader->checksum>>8;

	for (i=0;i<count;i++)
		outw(ether_packet[i],IOPORT);

	for (i=0;i<count;i++)
		 printf("0x%02X ",*(ether_packet+i));

	//printf("\nIP Header\n");

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
	int sock,j;
	char sel[30];
	unsigned char s,d;
	unsigned char data[100],i=0;

  signal(SIGALRM, jiffies);

  if (pthread_mutex_init(&lock, NULL) != 0)
  {
      printf("\n mutex init failed\n");
      return 1;
  }

  char * pch;
  char arguv[15][15];



	root=(raw_packet_t *)malloc(sizeof(raw_packet_t));
	root->next=NULL;
	root->prev=NULL;
	root->len=0;
	root->data="HULL";
	pack=root;
	printf("packet buffer root =%x\n",root);

	if (iopl(3)) {perror("iopl error"); exit(1);}

	struct ne *NIC;


	dest.sin_family = AF_INET;
	dest.sin_port = htons(231);//htons(4100);//dest_port
	dest.sin_addr.s_addr=inet_addr("192.168.2.102");

	NIC = (struct ne *) malloc(sizeof(struct ne));
	nic_reset();
	par();
	ne_probe(NIC);
  init_buf();

  //sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_TCP);
	//printf("Socket Number %i\n",sock);
  //printf("\nResult of connect: 	%i\n",connect_s(sock,&dest,sizeof(dest)));
  fflush(stdout);

	while(e!='s'){
    printf("%s#",sysname);

		//irq_event();
		//*g=read_s(sock,data,100);
		//printf("G = %i\n",g);
		/*if(g>0){
			printf("\nIncoming bytes %i\n",g);
			for(i=0;i<g;i++)
			{
		 	printf("%i.:0x%02x ",i,*(data+i));
			}
			printf("\n");
		}*/
    //e=scanf("%s\n",&sel );

    gets (sel);
    decode_command(&sel);

/*if(strcmp(sel,"show")!=0)
 		 g=write_s(sock,&sel,strlen(sel));*/

    j=0;

	}
    printf("sizeof(pch): ",strlen(pch));
close_s(sock);
printf("Exiting..\n");

	return 0;
}
