#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include <stdio_ext.h>
 
#define BUFLEN 512  					//Max length of buffer
#define N_ROT 6							
#define IP_SIZE 16						//category
#define MAX 1000						//max capacity of network
#define infinity 200000
#define TIMEOUT 5 						//timeout (sec)

struct package{
	unsigned id;						//id of package
	char type;							//ack(0) or data(1)
	unsigned source, destiny;			//router's id
	char message[BUFLEN];				//message
};

struct list{
	struct package pkg;
	struct list *next, *prev;
};

static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

int message_id;
int id;									//id router
unsigned short ports[N_ROT + 1];		//network information
unsigned short *nextHop;						//store all the next hops
char ips[N_ROT + 1][16];				//network information
struct list *_list;					//waiting for ack list



unsigned short * bellman_ford(unsigned, unsigned);

unsigned short * next_hop(unsigned, unsigned, unsigned short *);

void reaf();

void id_atoi(char *);

void destiny( void );

void * source ( void *);

void die (char *);

int ack(unsigned);

struct list * insert_list(struct list *, struct package);

void * thread_resend( void * );
