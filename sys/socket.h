#ifndef SOCKET_H
#define SOCKET_H

#define MAX_FILE_DESCRIPTORS 10
#define AF_INET		2	/* Internet IP Protocol 	*/

#define SOCK_STREAM	1		/* stream (connection) socket	*/
#define SOCK_DGRAM	2		/* datagram (conn.less) socket	*/


#define	IPPROTO_IP		0		/* dummy for IP */
#define	IPPROTO_ICMP		1		/* control message protocol */
#define	IPPROTO_TCP		6		/* tcp */
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#define	IPPROTO_PUP		12		/* pup */
#define	IPPROTO_UDP		17		/* user datagram protocol */
#define	IPPROTO_IDP		22		/* xns idp */
#define	IPPROTO_TP		29 		/* tp-4 w/ class negotiation */
#define	IPPROTO_EON		80		/* ISO cnlp */

#define	IPPROTO_RAW		255		/* raw IP packet */
#define	IPPROTO_MAX		256

#include "../nic.h"

/* Internet address. */
/*struct in_addr {
    unsigned long        s_addr;     /* address in network byte order */
//};

typedef struct sockaddr_in {
  unsigned char         sin_len;
  unsigned short  			sin_family;
  unsigned short        sin_port;
  struct in_addr  			sin_addr;
  char            			sin_zero[8];
} sockaddr_in_t;

typedef struct socket_descriptor{
  int type;
  int domain;
  int protocol;
  void *sck;
} socket_descriptor_t;


socket_descriptor_t fd[MAX_FILE_DESCRIPTORS];

int socket(int domain, int type, int protocol);

#endif

/*
#define MAKE_INSTR(nm, ...) static const u8 OPCODE_##nm[] = { __VA_ARGS__ }
MAKE_INSTR(INVD,   2, 0x0f, 0x08);
MAKE_INSTR(WBINVD, 2, 0x0f, 0x09);
MAKE_INSTR(CPUID,  2, 0x0f, 0xa2);
MAKE_INSTR(RDMSR,  2, 0x0f, 0x32);
MAKE_INSTR(WRMSR,  2, 0x0f, 0x30);
MAKE_INSTR(VMCALL, 3, 0x0f, 0x01, 0xd9);
MAKE_INSTR(HLT,    1, 0xf4);
MAKE_INSTR(INT3,   1, 0xcc);
MAKE_INSTR(RDTSC,  2, 0x0f, 0x31);

static const u8 *opc_bytes[INSTR_MAX_COUNT] =
{
    [INSTR_INVD]   = OPCODE_INVD,
    [INSTR_WBINVD] = OPCODE_WBINVD,
    [INSTR_CPUID]  = OPCODE_CPUID,
    [INSTR_RDMSR]  = OPCODE_RDMSR,
    [INSTR_WRMSR]  = OPCODE_WRMSR,
    [INSTR_VMCALL] = OPCODE_VMCALL,
    [INSTR_HLT]    = OPCODE_HLT,
    [INSTR_INT3]   = OPCODE_INT3,
    [INSTR_RDTSC]  = OPCODE_RDTSC
}; */
