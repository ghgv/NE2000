#include <stdio.h>
#include <pthread.h>
#include "nic.h"
#include "tcp.h"
#include "testne2000.h"
#include "sys/socket.h"

pthread_t thread1;
pthread_mutex_t lock;
extern sockaddr_in_t dest;

int  iret1;
char command[30][40];
unsigned char reg;

char *tok(char* str,char *limit ){
	static char *word;
	if(str!=NULL) word=str;
	if(word[0]== '\0') return NULL;

	char *ret = word,*b;
	const char *d;

	for(b=word;*b!='\n';b++){
		for(d=limit;*d!='\0';d++){
			if(*b==*d || *b=='\0'){
				*b='\0';
				word=b+1;
				if(b==ret){
				  ret++;
				  continue;
				}
			return ret;
			}
		}
	}
	return ret;
}



int run(){
printf("run\n");
  iret1 = pthread_create( &thread1, NULL, pool, NULL);
}



int decode_command(char *buff){
  char *token;
  void *func;
  int i=0;

//  printf("0 %x\n",command[0] );
//  printf("1 %x %x\n",command[1],&command[i] );
  token=tok(buff," ,-");
  	while(token!=NULL)
  	{
      memcpy((&command[i]),token,15);
      i++;
  		//printf("%i. %s \n",i,&command[i-1]);
  		token=tok(NULL, " ,-");
    //  printf("token: 0x%X\n",token );

  	}
    if(strcmp(&command[0],"show")==0)
      show(&command[1],&command[2]);
    if(strcmp(&command[0],"run")==0)
        run(&command[1]);
    if(strcmp(&command[0],"set")==0)
        set(&command[1],&command[2]);
    if(strcmp(&command[0],"reset")==0)
        reset_nic();
    if(strcmp(&command[0],"get")==0)
        get_bytes(&command[1]);
		if(strcmp(&command[0],"open")==0)
						open_socket(&command[1]);
		if(strcmp(&command[0],"close")==0)
						close_socket(&command[1]);
		if(strcmp(&command[0],"connect")==0)
						connect_socket(&command[1],&command[2],&command[3]);
}


int show(char *arg1, char *arg2){

  unsigned char value;
if(strcmp(arg1,"?")==0)
	  {
			printf("\
 regis				Registros\n\
 ISR				Interrupt Status Register\n\
 RSR				Receive Status Register\n\
 int0				Interface 0\n\
 socket				Sockets\n");
		}
if(strcmp(arg1,"regis")==0)
  {
  printf("Registers status:\n");
  printf("Base:     0x%04X\n",(BASE));
  outb(REG_PAGE2,COMMAND);
  printf("PSTART:   0x%02X PSTOP:  0x%02X\n",inb(NE_P2_PAGESTART),inb(NE_P2_PAGESTOP));
  outb(REG_PAGE0,COMMAND);
  printf("BOUNDARY: 0x%02X",inb(BOUNDARY));
  outb(REG_PAGE1,COMMAND);
  printf(" CURR: 0x%02X\n",inb(CURRENT));
  outb(REG_PAGE0,COMMAND);
  printf("ISR:      0x%02X RSR:  0x%02X\n",inb(INTERRUPTSTATUS),inb(RECEIVESTATUS));
	outb(REG_PAGE0,COMMAND);
	printf("IMR:      0x%02X RSR:  0x%02X\n",inb(INTERRUPTSTATUS),inb(RECEIVESTATUS));

  }
if(strcmp(arg1,"ISR")==0)
  {

		getreg();
    outb(REG_PAGE0,COMMAND);
    value=inb(INTERRUPTSTATUS);
    if(value&0x01)
        printf(" received ");
    if(value&0x02)
        printf(" transmited ");
    if(value&0x42)
            printf(" DMA finished ");
		putreg();

  }
if(strcmp(arg1,"RSR")==0)
    {

			getreg();
      outb(REG_PAGE0,COMMAND);
      value=inb(RECEIVESTATUS);
      if(value&0x01)
          printf(" Packet Intact ");
      if(value&0x20)
          printf(" MCAST ");
			putreg();


    }
if(strcmp(arg1,"int0")==0)
		    {
					getreg();
		      get_stats();
					putreg();
		    }
if(strcmp(arg1,"socket")==0)
						    {
									list_sockets();
						    }
printf("\b\b\n");

}

int set(char *reg, unsigned char *value){

  int num;

  if(value!=NULL)
    num = (int)strtol(value, NULL, 16);

  printf("Setting register %s to 0x%02x:\n",reg,num);

  if(strcmp(reg,"ISR")==0){
			pthread_mutex_lock(&lock);
      outb(REG_PAGE0,COMMAND);
      outb(num,INTERRUPTSTATUS);
			pthread_mutex_unlock(&lock);
      return 0;
  }
  if(strcmp(reg,"BNDY")==0){
			getreg();
      outb(REG_PAGE0,COMMAND);
      outb(num,BOUNDARY);
			putreg();
			return 0;
  }
printf("No register %s.\n",reg);
return -1;
}

int reset_nic(){

  printf("Reseting the NIC\n");
  unsigned char c;
  outb(REG_PAGE0,COMMAND);
  c = inb(COMMAND + NS_RESET);
  outb(c,COMMAND + NS_RESET);
  printf("Reset %02x\n",c);

}

void eraser()
{
  int i;
  for(i=0;i<4;i++)
  memcpy(&command[i],"    ",5);
}


int get_bytes(int *value){
  int num,i;

  if(value!=NULL)
    num = (int)strtol(value, NULL, 10);

  outb(REG_PAGE0, COMMAND); //to read the BOUNDARY
  outb(inb(NE_P0_BOUNDARY),REMOTESTARTADDRESS1);
  outb(0X00,REMOTESTARTADDRESS0);
  outb(num&0xff, REMOTEBYTECOUNT0);
	outb((num>>8)&0xff, REMOTEBYTECOUNT1);
  outb(CMD_REMOTE_READ+CMD_START, COMMAND);
  for(i=0;i<num;i++){
				printf("0x%02hhx ",inb(IOPORT));
	}
printf("\n");
}

void get_stats()
{
  printf("Interface 100M\n");
	printf("Received packets: %i\n",paquetes);
	printf("Sent packets: %i\n",enviados);
	printf("MAC address: ");
	outb( REG_PAGE1,COMMAND);
	printf("%02X:%02X:%02X:%02X:%02X:%02X\n",inb(COMMAND+NE_P1_PAR0),inb(COMMAND+NE_P1_PAR1),
	inb( COMMAND+NE_P1_PAR2),inb(COMMAND+NE_P1_PAR3),inb(COMMAND+NE_P1_PAR4),
	inb( COMMAND+NE_P1_PAR5));
}

void getreg(){
	pthread_mutex_lock(&lock);
	reg=inb(COMMAND) & 0xC0;
}

void putreg(){
	outb(reg,COMMAND);
	pthread_mutex_unlock(&lock);
}


int open_socket(){
	int sock;
	sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_TCP);
	if(sock!=-1)
		printf("Socket %i opened.\n",sock);
	else
	  printf("Socket could not be opened.\n");
}

int list_sockets(){
	int i;
	for(i=1;i<MAX_FILE_DESCRIPTORS;i++)
	 {
		 if(fd[i].sck!=0){
			printf("Socket[%i]:\n Type: %i\n",i,fd[i].type);
			printf(" Protocol: %i\n\n",fd[i].protocol);
		 }

	}
	pthread_mutex_unlock(&lock);
}

int close_socket(char * value){
	int num,i;

	if(value!=NULL){
		num = (int)strtol(value, NULL, 16);
		if(num>0)
			i=close_s(num);
	}
  if(i==0)
	printf("Socket %i closed.\n",num);
	else
		printf("Socket could not be closed.\n",num);
}



int connect_socket(char *sock,char *AF,char *Port,char *IP){
	int socket1,i;
	int AddFamily;
	int port;
	unsigned long IPadd;
	tcb_t *tcb;

printf("Connecting to:\n");


	if(sock!=NULL)
		socket1 = (int)strtol(sock, NULL, 16);
		else
		 {
			 printf("Wrong socket number.\n");
			 return -1;
		 }
	if(fd[socket1].sck==0){
		printf("Socket is closed.\n");
		return -1;
	}
	else
	{
		tcb=fd[socket1].sck;
	}

	if(strcmp(AF,"AF_INET")==0)
		AddFamily=2;

	if(Port!=NULL)
			port = (int)strtol(Port, NULL, 10);
	else
	{
		printf("Wrong port number.\n");
		return -1;
	}
	dest.sin_family = AddFamily;
	printf("Family: %i ",dest.sin_family);
	dest.sin_port = htons(port);//htons(4100);//dest_port
	printf("Port: %x ",dest.sin_port);

	dest.sin_addr.s_addr=inet_addr("192.168.2.109");
	printf("IP: 0x%X\n",dest.sin_family);

		connect_s(socket1,&dest,sizeof(dest));
	

	int msec = 0, trigger = 500;
	int iterations=0;/* 10ms */
	clock_t before = clock();
	do {
  /*
   * Do something to busy the CPU just here while you drink a coffee
   * Be sure this code will not take more than `trigger` ms
   */

  clock_t difference = clock() - before;
  msec = difference * 1000 / CLOCKS_PER_SEC;
  iterations++;
} while ( msec < trigger );

printf("Time taken %d seconds %d milliseconds (%d iterations)\n",
  msec/1000, msec%1000, iterations);
	if(tcb->tcb_state==SYN_SENT)
		printf("Connection timeout\n");


}
