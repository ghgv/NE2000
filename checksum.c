#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//unsigned short int le_value = xyz[0] + (xyz[1] << 8);   // little-endian
//unsigned short int be_value = xyz[1] + (xyz[0] << 8);   //    big-endian

unsigned int ip_checksum(void* vdata,int length) {
    // Cast the data pointer to one that can be indexed.
    char* data=(char*)vdata;
    int i=0;

    // Initialise the accumulator.
    unsigned long acc=0xffff;
    
    // Handle complete 16-bit blocks.
    for (i=0;i+1<length;i+=2) {
        unsigned int word;
        memcpy(&word,data+i,2);printf("%X %x\n",*(data+i),(char*)(vdata+i));
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Handle any partial block at the end of the data.
    if (length&1) {
        unsigned int word=0;
        memcpy(&word,data+length-1,1);
        acc+=ntohs(word);
        if (acc>0xffff) {
            acc-=0xffff;
        }
    }

    // Return the checksum in network byte order.
    return htons(~acc);
}

unsigned short checksum (int count, unsigned int * addr) {
    unsigned long sum = 0;

    //count=count>>8;
    printf("Count =%x \n",count);
    while (count > 1) {
        printf("%x ",(*addr ));
        // sum+=ntohs(*addr++);
        sum += *addr++;  
			printf("Partial %x \n",sum);
			    
        count -= 2;
    }

    // Add left-over byte, if any
    if (count > 0)
        sum += * (unsigned char *) addr;

    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return (unsigned short)~sum;
}

int main(int argc, char *argv[])
{
unsigned int addr[]={0x4500, 0x003c, 0x1c46, 0x4000, 0x4006, 0x0000, 0xac10, 0x0a63, 0xac10, 0x0a0c};
printf("QQ %x",addr[0]);
printf("Result: %x\n",checksum(20,addr));
printf("Result: %x\n",ip_checksum(&addr[0],10));
return 0;
}
