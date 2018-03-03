#ifndef ARP_H
#define ARP_H

#define ETH_P_ARP 0x0806
#define ARP_HTYPE 1
#define ARP_PTYPE_IPV4 0x0800

#define htons(A) ((((unsigned int)(A) & 0xff00) >> 8) | \
(((uint16)(A) & 0x00ff) << 8))

#define htonl(A) ((((unsigned long)(A) & 0xff000000) >> 24) | \
(((unsigned long)(A) & 0x00ff0000) >> 8) | \
(((unsigned long)(A) & 0x0000ff00) << 8) | \
(((unsigned long)(A) & 0x000000ff) << 24))

#define ntohs(A) ((((unsigned int)(A) & 0xff00) >> 8) | \
(((unsigned int)(A) & 0x00ff) << 8))

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
