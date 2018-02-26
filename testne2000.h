#ifndef testne2000_H
#define testne2000_H


/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Global definitions for the Ethernet IEEE 802.3 interface.
 *
 * Version:	@(#)if_ether.h	1.0.1a	02/08/94
 *
 * Author:	Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *		Donald Becker, <becker@super.org>
 *		Alan Cox, <alan@redhat.com>
 *		Steve Whitehouse, <gw7rrm@eeshack3.swan.ac.uk>
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
 


/*
 *	IEEE 802.3 Ethernet magic constants.  The frame sizes omit the preamble
 *	and FCS/CRC (frame check sequence). 
 */

#define ETH_ALEN	6		/* Octets in one ethernet addr	 */
#define ETH_HLEN	14		/* Total octets in header.	 */
#define ETH_ZLEN	60		/* Min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */
#define ETH_FCS_LEN	4		/* Octets in the FCS		 */

/*
 *	These are the defined Ethernet Protocol ID's.
 */

#define ETH_P_LOOP		0x0060		/* Ethernet Loopback packet	*/
#define ETH_P_PUP	   	0x0200		/* Xerox PUP packet		*/
#define ETH_P_PUPAT		0x0201		/* Xerox PUP Addr Trans packet	*/
#define ETH_P_IP	   	0x0800		/* Internet Protocol packet	*/
#define ETH_P_X25	   	0x0805		/* CCITT X.25			*/
#define ETH_P_ARP	   	0x0806		/* Address Resolution packet	*/
#define	ETH_P_BPQ		0x08FF		/* G8BPQ AX.25 Ethernet Packet	[ NOT AN OFFICIALLY REGISTERED ID ] */
#define ETH_P_IEEEPUP	0x0a00		/* Xerox IEEE802.3 PUP packet */
#define ETH_P_IEEEPUPAT	0x0a01		/* Xerox IEEE802.3 PUP Addr Trans packet */
#define ETH_P_DEC       0x6000          /* DEC Assigned proto           */
#define ETH_P_DNA_DL    0x6001          /* DEC DNA Dump/Load            */
#define ETH_P_DNA_RC    0x6002          /* DEC DNA Remote Console       */
#define ETH_P_DNA_RT    0x6003          /* DEC DNA Routing              */
#define ETH_P_LAT       0x6004          /* DEC LAT                      */
#define ETH_P_DIAG      0x6005          /* DEC Diagnostics              */
#define ETH_P_CUST      0x6006          /* DEC Customer use             */
#define ETH_P_SCA       0x6007          /* DEC Systems Comms Arch       */
#define ETH_P_RARP      0x8035		/* Reverse Addr Res packet	*/
#define ETH_P_ATALK		0x809B		/* Appletalk DDP		*/
#define ETH_P_AARP		0x80F3		/* Appletalk AARP		*/
#define ETH_P_8021Q		0x8100          /* 802.1Q VLAN Extended Header  */
#define ETH_P_IPX			0x8137		/* IPX over DIX			*/
#define ETH_P_IPV6		0x86DD		/* IPv6 over bluebook		*/
#define ETH_P_SLOW		0x8809		/* Slow Protocol. See 802.3ad 43B */
#define ETH_P_WCCP		0x883E		/* Web-cache coordination protocol
					 * defined in draft-wilson-wrec-wccp-v2-00.txt */
#define ETH_P_PPP_DISC	0x8863		/* PPPoE discovery messages     */
#define ETH_P_PPP_SES	0x8864		/* PPPoE session messages	*/
#define ETH_P_MPLS_UC	0x8847		/* MPLS Unicast traffic		*/
#define ETH_P_MPLS_MC	0x8848		/* MPLS Multicast traffic	*/
#define ETH_P_ATMMPOA	0x884c		/* MultiProtocol Over ATM	*/
#define ETH_P_ATMFATE	0x8884		/* Frame-based ATM Transport
					 * over Ethernet
					 */
#define ETH_P_AOE			0x88A2		/* ATA over Ethernet		*/
#define ETH_P_TIPC		0x88CA		/* TIPC 			*/
#define ETH_P_FCOE		0x8906		/* Fibre Channel over Ethernet	*/
#define ETH_P_FIP			0x8914		/* FCoE Initialization Protocol */

/*
 *	Non DIX types. Won't clash for 1500 types.
 */
 
#define ETH_P_802_3		0x0001		/* Dummy type for 802.3 frames  */
#define ETH_P_AX25		0x0002		/* Dummy protocol id for AX.25  */
#define ETH_P_ALL			0x0003		/* Every packet (be careful!!!) */
#define ETH_P_802_2		0x0004		/* 802.2 frames 		*/
#define ETH_P_SNAP		0x0005		/* Internal only		*/
#define ETH_P_DDCMP     0x0006          /* DEC DDCMP: Internal only     */
#define ETH_P_WAN_PPP   0x0007          /* Dummy type for WAN PPP frames*/
#define ETH_P_PPP_MP    0x0008          /* Dummy type for PPP MP frames */
#define ETH_P_LOCALTALK 0x0009		/* Localtalk pseudo type 	*/
#define ETH_P_PPPTALK	0x0010		/* Dummy type for Atalk over PPP*/
#define ETH_P_TR_802_2	0x0011		/* 802.2 frames 		*/
#define ETH_P_MOBITEX	0x0015		/* Mobitex (kaz@cafe.net)	*/
#define ETH_P_CONTROL	0x0016		/* Card specific control frames */
#define ETH_P_IRDA		0x0017		/* Linux-IrDA			*/
#define ETH_P_ECONET		0x0018		/* Acorn Econet			*/
#define ETH_P_HDLC		0x0019		/* HDLC frames			*/
#define ETH_P_ARCNET		0x001A		/* 1A for ArcNet :-)            */

#define IPPROTO_IP              0               /* dummy for IP */
#define IPPROTO_UDP             17              /* user datagram protocol */
#define IPPROTO_TCP             6               /* tcp */
#define FIN 			0x01
#define SYN 			0x02
#define RST 			0x04
#define PSH			0x08
#define ACK			0x10
#define URG			0x20	

//DP8390 Registers
#define         REG_PAGE0               0x00
#define         REG_PAGE1               0x40
#define         REG_PAGE2               0x80
#define         REG_PAGE3               0xC0

#define         P0_RCR                  0x0c
#define         P0_TCR                  0x0d
#define         P0_ISR                  0x07
#define         P0_RBCR0                0x0a    /* Remote Byte Count Lo */
#define         P0_RBCR1                0x0b    /* Remote Byte Count Hi */
#define         P0_TBCR0                0x05
#define         P0_TBCR1                0x06

#define         CMD_REMOTE_READ         0x08
#define         CMD_REMOTE_WRITE        0x10
#define         CMD_SEND_PACKET         0x20
#define         CMD_TXP_BIT             0x04
#define         CMD_NO_DMA              0x20
#define         CMD_STOP                0x01
#define         CMD_START               0x02

#define         P0_RCR                  0x0c
#define         P0_TCR                  0x0d
#define         P0_ISR                  0x07
#define         P0_RBCR0                0x0a    /* Remote Byte Count Lo */
#define         P0_RBCR1                0x0b    /* Remote Byte Count Hi */
#define         P0_TBCR0                0x05
#define         P0_TBCR1                0x06

#define         P1_CURR                 0x07

#define         ISR_VL_PRX              0x01    /* Packet Received */
#define         ISR_VL_PTX              0x02    /* Packet Transmitted */
#define         ISR_VL_RXE              0x04    /* Receive Error */
#define         ISR_VL_TXE              0x08    /* Transmission Error */
#define         ISR_VL_OVW              0x10    /* Overwrite */
#define         ISR_VL_CNT              0x20    /* Counter Overflow */
#define         ISR_VL_RDC              0x40    /* Remote Data Complete */
#define         ISR_VL_RST              0x80    /* Reset status */

#define         NS_DATA_PORT            0x10
#define         NS_RESET                0x1f

#define         NE_PAGE_SIZE            256


#define BASE 0xEC00
#define COMMAND BASE+0
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

static unsigned char next_pkt =0;

static unsigned int  rx_page_start = 64;


typedef struct t_dp8390_pkt_hdr
{
 unsigned char NextPacketPointer;
 unsigned char ReceiveStatus;
 unsigned short length;

} __attribute__ ((packed)) dp8390_pkt_hdr;

//END OF DP8390


typedef struct header
{
    unsigned char ip_hl:4, ip_v:4;/* this means that each member is 4 bits */
    unsigned char ip_tos;       //1 Byte
    unsigned short int ip_len;  //2 Byte
    unsigned short int ip_id;   //2 Byte
    unsigned short int ip_off;  //2 Byte
    unsigned char ip_ttl;       //1 Byte
    unsigned char ip_p;         //1 Byte
    unsigned short int ip_sum;  //2 Byte
    unsigned int ip_src;        //4 Byte
    unsigned int ip_dst;        //4 Byte


} __attribute__((packed)) ipheader;

/*
 *	This is an Ethernet frame header.
 */



typedef struct t_ethhdr {
	unsigned char	h_dest[6];	/* destination eth addr	*/
	unsigned char	h_source[6];	/* source ether addr	*/
	unsigned int	h_proto;		/* packet type ID field	*/
} __attribute__((packed)) ethhdr_t;

unsigned char ether_packet[1560];


typedef unsigned short 	uint16;
typedef unsigned long 	uint32;



typedef struct {
  unsigned short int  src_port;
  unsigned short int  dst_port;
  unsigned int seq;
  unsigned int ack;
  unsigned short int darefla;
//  unsigned char reserved:3;
 // unsigned short int flags:9;
//  unsigned char  data_offset;  // 4 bits
 // unsigned char  flags;
  unsigned short int window_size;
  unsigned short int checksum;
  unsigned short int urgent_p;
} __attribute__((packed)) tcp_header_t;

static int irq_event();
int nic_reset();
int par();

int received_packet();
int readmem(void *dest,unsigned short src ,int n);

#define htons(A) ((((uint16)(A) & 0xff00) >> 8) | \
(((uint16)(A) & 0x00ff) << 8))

#define htonl(A) ((((uint32)(A) & 0xff000000) >> 24) | \
(((uint32)(A) & 0x00ff0000) >> 8) | \
(((uint32)(A) & 0x0000ff00) << 8) | \
(((uint32)(A) & 0x000000ff) << 24))


#endif

