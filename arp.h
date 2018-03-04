#ifndef ARP_H
#define ARP_H

#define ETH_P_ARP 0x0806
#define ARP_HTYPE 1
#define ARP_PTYPE_IPV4 0x0800



typedef struct arp_t
{
unsigned short hardware_type;
unsigned short protocol;
unsigned char hardware_addr_length;
unsigned char protocol_addr_length;
unsigned short Opcode;
unsigned char Sender_H_A[6];
unsigned char Sender_P_A[4];
unsigned char Target_H_A[6];
unsigned char Target_P_A[4];


} __attribute__((packed)) arp_frame;


int decode_arp(void *packet);
int reply_arp();


#endif
