#include "nic.h"
#include "ip.h"
#include "sys/mbuf.h"
#include "tcp.h"
#include "testne2000.h"

#include <stdio.h>

int decode_tcp(unsigned char *packet){

	printf("#########INSIDE TCP ################\n");
	fflush(stdout);

	ipheader header;
	tcpheader_t tcpheader;


	memcpy(&header,&packet[14],20);
	memcpy(&tcpheader,&packet[14+20],20);

	printf("header.ip_src %x\n",htonl(header.ip_src));
	printf("header.ip_dst %x\n",htonl(header.ip_dst));
	printf("header.ip_port   %02x\n",header.ip_p);
	printf("tcpheader.sorce port 			%i\n",ntohs(tcpheader.source_port));
	printf("tcpheader.dest port  			%i\n",ntohs(tcpheader.dest_port));
	printf("tcpheader.seq   		 	%02x\n",ntohl(tcpheader.seq_number));
	printf("tcpheader.ack   		 	%02x\n",ntohl(tcpheader.ack_number));
	printf("tcpheader.windows s			%02x\n",ntohs(tcpheader.windows));
	printf("tcpheader.check				%02x\n",ntohs(tcpheader.checksum));

	if(ntohs(tcpheader.dest_port)==4100)
	{
		//printf("######### MATCH TCP ################\n");

			if(((ntohs(tcpheader.flags) & 0x1ff) & SYN)==SYN)
					printf("SYN\n");
			if(((ntohs(tcpheader.flags) & 0x1ff) & RST)==RST)
							printf("Reset\n");
  		if(((ntohs(tcpheader.flags) & 0x1ff) & ACK)==ACK)
							printf("ACK\n");
	}



while(1);

}
