#ifndef ICMP_H
#define ICMP_H

#define ICMP_ECHO_REPLY   0
#define ICMP_ECHO_REQUEST 8

typedef struct icmp_t
{
 unsigned char type;
 unsigned char code;
 unsigned short checksum;
 unsigned long ROH; //Rest Of Header

}__attribute__ ((packed)) icmp_hdr;

int decode_icmp(unsigned char *packet);
int icmp_reply(unsigned char *packet);


#endif
