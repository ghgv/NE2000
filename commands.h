#ifndef COMMANDS_H
#define COMMANDS_H

int run();
int set();
int show(int );
char *tok(char* str,char *limit );
void eraser();
void get_stats();
void getreg();
void putreg();
int connect_socket(char *,char *,char *,char *);
char sysname[]="dali.com.co";

#endif
