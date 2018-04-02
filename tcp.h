#ifndef TCP_H
#define TCP_H

#include "nic.h"


#define SYN 		0x02
#define RST 		0x04
#define PSH			0x08
#define ACK			0x10
#define URG			0x20



#define SYN_SENT 0x01
#define TCP_CLOSED 0x00

static const char *STATES[] = {
    "CLOSED", "SYN_SENT", "SYN_RECEIVED", "ESTABLISHED",
};
/*typedef struct
{
    unsigned long s_addr;  // load with inet_aton()
} __attribute__ ((packed)) in_addr;
*/
typedef int tcpseq;

//http://www.cs.unc.edu/~dewan/242/s00/xinu-pentium/h/tcb.h
typedef struct tcb {
	short	tcb_state;	/* TCP state				*/
	short	tcb_ostate;	/* output state				*/
	short	tcb_type;	/* TCP type (SERVER, CLIENT)		*/
	int	tcb_mutex;	/* tcb mutual exclusion			*/
	short	tcb_code;	/* TCP code for next packet		*/
	short	tcb_flags;	/* various TCB state flags		*/
	short	tcb_error;	/* return error for user side		*/

	in_addr	tcb_rip;	/* remote IP address			*/
	short	tcb_rport;	/* remote TCP port			*/
	in_addr tcb_lip;	/* local IP address			*/
	short	tcb_lport;	/* local TCP port			*/
	struct	netif	*tcb_pni; /* pointer to our interface		*/

  tcpseq	tcb_suna;	/* send unacked				*/
	tcpseq	tcb_snext;	/* send next				*/
	tcpseq	tcb_slast;	/* sequence of FIN, if TCBF_SNDFIN	*/
	long	tcb_swindow;	/* send window size (octets)		*/
	tcpseq	tcb_lwseq;	/* sequence of last window update	*/
	tcpseq	tcb_lwack;	/* ack seq of last window update	*/
	int	tcb_cwnd;	/* congestion window size (octets)	*/
	int	tcb_ssthresh;	/* slow start threshold (octets)	*/
	int	tcb_smss;	/* send max segment size (octets)	*/
	tcpseq	tcb_iss;	/* initial send sequence		*/

	int	tcb_srt;	/* smoothed Round Trip Time		*/
	int	tcb_rtde;	/* Round Trip deviation estimator	*/
	int	tcb_persist;	/* persist timeout value		*/
	int	tcb_keep;	/* keepalive timeout value		*/
	int	tcb_rexmt;	/* retransmit timeout value		*/
	int	tcb_rexmtcount;	/* number of rexmts sent		*/

	tcpseq	tcb_rnext;	/* receive next				*/
	tcpseq	tcb_rupseq;	/* receive urgent pointer		*/
	tcpseq	tcb_supseq;	/* send urgent pointer			*/

	int	tcb_lqsize;	/* listen queue size (SERVERs)		*/
	int	tcb_listenq;	/* listen queue port (SERVERs)		*/
	struct tcb *tcb_pptcb;	/* pointer to parent TCB (for ACCEPT)	*/
	int	tcb_ocsem;	/* open/close semaphore 		*/
	int	tcb_dvnum;	/* TCP slave pseudo device number	*/

	int	tcb_ssema;	/* send semaphore			*/
	unsigned char	*tcb_sndbuf;	/* send buffer				*/
	int	tcb_sbstart;	/* start of valid data			*/
	int	tcb_sbcount;	/* data character count			*/
	int	tcb_sbsize;	/* send buffer size (bytes)		*/

	int	tcb_rsema;	/* receive semaphore			*/
	unsigned char	*tcb_rcvbuf;	/* receive buffer (circular)		*/
	int	tcb_rbstart;	/* start of valid data			*/
	int	tcb_rbcount;	/* data character count			*/
	int	tcb_rbsize;	/* receive buffer size (bytes)		*/
	int	tcb_rmss;	/* receive max segment size		*/
	tcpseq	tcb_cwin;	/* seq of currently advertised window	*/
	int	tcb_rsegq;	/* segment fragment queue		*/
	tcpseq	tcb_finseq;	/* FIN sequence number, or 0		*/
	tcpseq	tcb_pushseq;	/* PUSH sequence number, or 0		*/

	unsigned short fin;
	unsigned short syn;
	unsigned short rst;
	unsigned short psh;
	unsigned short ack;
	unsigned short urg;

}tcb_t;

typedef struct tcpheader
{
    unsigned short source_port;
    unsigned short dest_port;
    unsigned int seq_number;
    unsigned int ack_number;
    unsigned short flags;
    unsigned short windows;
    unsigned short checksum;
    unsigned short urgent;
} __attribute__((packed)) tcpheader_t;



int decode_tcp(unsigned char *packet);

#endif
