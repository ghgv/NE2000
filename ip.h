#ifndef IP_H
#define IP_H


#define IPPROTO_IP              0               /* dummy for IP */
#define IPPROTO_UDP             17              /* user datagram protocol */
#define IPPROTO_TCP             6               /* tcp */
#define IPPROTO_ICMP            1               /*ICMP*/    

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

int decode_ip(unsigned char *packet);


#endif
