#ifndef COMMANDS_H
#define COMMANDS_H



int run();
int set();
int show(char *arg1, char *arg2);
char *tok(char* str,char *limit );
void eraser();
void get_stats();
void getreg();
void putreg();
int connect_socket(char *sock,char *AF,char *Port,char *IP);
int interface(unsigned char *nic,unsigned char *state );
int debug(unsigned char *proto, unsigned char *state);
char sysname[]="dali.com.co";



#endif
