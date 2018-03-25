#ifndef MBUF_H
#define MBUF_H

typedef struct mbuf{

  void *m_next;
  void *m_nextpacket;
  int m_len;
  void *m_data;
  int m_type;
  int m_flags;
  int m_packethdrlen;
  void *m_packet_recv_inf;
  unsigned char data[80];


}__attribute__((packed)) mbuf_t;

typedef struct mbuf2{

  void *m_next;
  void *m_nextpacket;
  int m_len;
  void *m_data;
  int m_type;
  int m_flags;
  int m_packethdrlen;
  void *m_packet_recv_inf;



}__attribute__((packed))  mbuf2_t;

typedef struct circularq{

  int front;
  int rear;
  int MAX;
  void *datapointer;

} circularq_t;




#endif
