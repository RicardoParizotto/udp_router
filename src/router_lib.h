#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<stdio_ext.h>
 
#define BUFLEN 512  				//Max length of buffer
#define N_ROT 6					//network size
#define IP_SIZE 16				//category
#define MAX 1000				//max capacity of network
#define infinity 200000
#define TIMEOUT 5 				//timeout (sec)

struct package{
	unsigned id, att;								
	char type;				//ack(0) or data(1)
	unsigned source, destiny;		//router's id
	char message[BUFLEN];			//message
};

struct list{
	struct package pkg;
	struct list *next, *prev;
};

static pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;         //thread for share the _list to all the threads

int message_id;								//id of message ( auto increment when send )

int id;									//router's id

unsigned short ports[N_ROT + 1];			//network information

unsigned short *nextHop;				//store all the next hops

char ips[N_ROT + 1][16];				//network information

struct list *_list;					//waiting for ack list




unsigned short * bellman_ford(unsigned, unsigned);
/*
*	first argument is the size of the network topology
*	the other argument is the origin vertex. 
*
*	Returns a pointer to the parents' array
*/


unsigned short * next_hop(unsigned, unsigned, unsigned short *);
/*
* 	the first argument (unsigned) is the size of the network topology
*
*	second (unsigned) argument is the origin vertex
* 	
*	last argument is a pointer that has the information of all the parents
* 		- parent[I] = A
* 			A is the parent of I when we want to reach A starting in the origin 
*			vertex
*/

void reaf();
/*
* 	reads roteador.config file and store the network information in 
*	ports and ips arrays
*
*/

void id_atoi(char *);
/*
* 	convert the only argument (char) into a integer and store it in a global
* 	variable "id". 
*
*/

void destiny( void );
/*
*	this function keep listening for messages in the network and decides what
*	to do whit the received package: resend to the right node or send a acknowledgement to the source
*/

void * source ( void *);
/*
*	this function waits for users messages and send them to the destiny
*/

void die (char *);
/*
*	Just print a error message (char *) and exit the program
*/

int ack(unsigned);
/*
*	delete, if exists, the first argument (unsigned) of the global list (_list)
*
*	returns a non-zero if the operation is done
*	otherwise, returns zero
*
*/

struct list * insert_list(struct package);
/*
*	Insert package into a global list called _list
*	the package is inserted in the beginning of the list
*/

void * thread_resend( void * );
/*
*	This function sleeps for TIMEOUT seconds and then resend all the packages that are in a global list called _list
*	
*/
