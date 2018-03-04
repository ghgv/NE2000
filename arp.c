#include "arp.h"
#include "nic.h"
#include "testne2000.h"
#include <string.h>
#include <stdio.h>

arp_frame *arpa;

int decode_arp(void *packet){

	unsigned char PACKET[1560];



	arpa=packet;
	memcpy(&PACKET,packet,1560);
	printf("########### Packet Decode #########\n");
	memcpy(arpa,&PACKET[14],sizeof(arp_frame));
	printf("Hardware Type:		0x%02x\n",ntohs(arpa->hardware_type));
	printf("Protocol Type:		0x%02x\n",ntohs(arpa->protocol));
	printf("HAL:			0x%02x\n",(arpa->hardware_addr_length));
	printf("PAL:			0x%02x\n",(arpa->protocol_addr_length));
	printf("Opcode:			0x%02x\n",ntohs(arpa->Opcode));
	printf("SHA:			%02X:%02X:%02X:%02X:%02X:%02X\n",arpa->Sender_H_A[0],arpa->Sender_H_A[1],arpa->Sender_H_A[2],arpa->Sender_H_A[3],arpa->Sender_H_A[4],arpa->Sender_H_A[5]);
	printf("PHA:			%i.%i.%i.%i\n",arpa->Sender_P_A[0],arpa->Sender_P_A[1],arpa->Sender_P_A[2],arpa->Sender_P_A[3]);
	printf("THA:			%02x:%02x:%02x:%02x:%02x:%02x\n",arpa->Target_H_A[0],arpa->Target_H_A[1],arpa->Target_H_A[2],arpa->Target_H_A[3],arpa->Target_H_A[4],arpa->Target_H_A[5]);
	printf("THA:			%i.%i.%i.%i\n",arpa->Target_P_A[0],arpa->Target_P_A[1],arpa->Target_P_A[2],arpa->Target_P_A[3]);

	if(arpa->Target_P_A[0]=192 && arpa->Target_P_A[1]== 168 && arpa->Target_P_A[2] == 2 && arpa->Target_P_A[3] == 108)
		reply_arp();

}

int reply_arp()
{
	int i;
	printf("########### Reply ARP ########### \n");
	arp_frame *pac;
	int len;
	int proto;

	len=sizeof(arp_frame);
	pac=malloc(len);
	pac->hardware_type=arpa->hardware_type;
	pac->protocol=arpa->protocol;
	pac->hardware_addr_length=arpa->hardware_addr_length;
	pac->protocol_addr_length=arpa->protocol_addr_length;
	pac->Opcode=0x0200;//Reply Opcode
	pac->Sender_H_A[0]=arpa->Sender_H_A[0];
	pac->Sender_H_A[1]=arpa->Sender_H_A[1];
	pac->Sender_H_A[2]=arpa->Sender_H_A[2];
	pac->Sender_H_A[3]=arpa->Sender_H_A[3];
	pac->Sender_H_A[4]=arpa->Sender_H_A[4];
	pac->Sender_H_A[5]=arpa->Sender_H_A[5];
	pac->Sender_P_A[0]=arpa->Sender_P_A[0];
	pac->Sender_P_A[1]=arpa->Sender_P_A[1];
	pac->Sender_P_A[2]=arpa->Sender_P_A[2];
	pac->Sender_P_A[3]=arpa->Sender_P_A[3];

	send_raw_packet(pac,len,ETH_P_ARP);


}