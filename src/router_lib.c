#include "router_lib.h"
#include <unistd.h>

unsigned short * bellman_ford(unsigned size, unsigned vertex){
	int mat[MAX][MAX], v, u, d;
	FILE * F;
	unsigned dist[MAX];
	unsigned short *parent = malloc((size + 1)*sizeof(unsigned short));

	if(!(F = fopen("../config/enlaces.config", "r"))) return;

	for ( v = 1; v <= size; v++ ){
		for( u = 1; u <= size; u++ )
			mat[v][u] = 0;
		dist[v] = infinity;
		parent[v] = -1;
	}
	
	dist[vertex] = 0;
	parent[vertex] = vertex;
	
	while(fscanf(F, "%d %d %d", &v, &u, &d)!= EOF)
		mat[u][v] = mat[v][u] = d;


	for(d = 0; d < size - 1; d++ )
		for( v = 1; v <= size; v++ )
			for (u = 1; u <= size; u++ )
				if(mat[v][u])
					if(dist[v] > mat[v][u] + dist[u]){
						dist[v] = mat[v][u] + dist[u];
						parent[v] = u;
					}
	return parent;
}


unsigned short * next_hop(unsigned size, unsigned vertex, unsigned short * parent){
	unsigned short * hop = malloc((size + 1) * sizeof(unsigned short));
	unsigned i, j;

	for ( i = 1; i <= size; i++){
		j = i;
		while(parent[j] != vertex){
			j = parent[j];
		}
		hop[i] = j;
	}
	return hop;
}

struct list * insert_list(struct package pkg){
	struct list *new = malloc(sizeof(struct list));
	new->pkg = pkg;
	new->next = _list;	
	new->prev = NULL;
	if(_list) _list->prev = new;
	return new;
}

int ack(unsigned id){
	struct list * l = _list;
	while(l){
		if(l->pkg.id == id){
			if(l->prev) l->prev->next = l->next;
			if(l->next) l->next->prev = l->prev;
			if(!l->prev && !l->next) _list = NULL;
			return 1;
		}
		l = l->next;
	}
	return 0;
} 

void die(char *s){
    perror(s);
    exit(1);
}


void _send(struct sockaddr_in si_other, unsigned i, int s, struct package * pkg){
	si_other.sin_port = htons(ports[nextHop[i]]);     // refresh the port addres of the destiny
     
   	if (inet_aton(ips[nextHop[i]], &si_other.sin_addr) == 0){
    	fprintf(stderr, "inet_aton() failed\n");
    	exit(1);
	}

    //send the message
    if (sendto(s, pkg, sizeof(struct package) , 0 , (struct sockaddr *) &si_other, sizeof(si_other))==-1)
    	die("sendto()");   	
}


void * thread_resend( void * v ){
	struct sockaddr_in si_other;
	int s, i, slen=sizeof(si_other), minute;
	struct list * l;

	if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    	die("socket");

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
	
	while(1){
		sleep(5); 		//timeout
		pthread_mutex_lock(&list_mutex);
		l = _list;
		while(l){
			if(l->pkg.att++ <= 3){
				_send(si_other, l->pkg.destiny, s, &l->pkg);
				printf("> Resend package %d.\n", l->pkg.id);
			}else{
				ack(l->pkg.id);
				printf("> Package %d can't reach destiny.\n", l->pkg.id);
			}
			l = l->next;
		}
		pthread_mutex_unlock(&list_mutex);
	}
}


void * source(void * v){
    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
	struct package * pkg = malloc(sizeof(struct package));
 
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");
 
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;

    while(1){
        scanf("%d %s",&i, pkg->message);
		pkg->id = message_id++;
		pkg->source = id;
		pkg->destiny = i;	
		pkg->type = pkg->att = 1;

		pthread_mutex_lock(&list_mutex); //block call
		_list = insert_list(*pkg);
		pthread_mutex_unlock(&list_mutex);

		_send(si_other, i, s, pkg);
		
    }
	
	free(pkg);
    close(s);
    return;
}
 

void destiny( void ){
    struct sockaddr_in si_me, si_other;
	struct package * pkg = malloc(sizeof(struct package));
     
    int s, i, slen = sizeof(si_other) , recv_len;

	struct list *l = _list;

     
    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        die("socket");
     
    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));
     
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(ports[id]);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
        die("bind");
     
    //keep listening for data
    while(1){
		int new_d;

		fflush(stdout);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, pkg, sizeof(struct package), 0, (struct sockaddr *) &si_other, &slen)) == -1)
            die("recvfrom()");
        
		printf("\n> Received packet from %s: %d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

		//packing
		if(pkg->destiny == id){
			if(pkg->type == 0){
				printf("> Acknowledgment %d\n", pkg->id);
				//remove the package from the list
				pthread_mutex_lock(&list_mutex);
				ack(pkg->id);
				pthread_mutex_unlock(&list_mutex);
				continue;
			}

			printf("DATA: %s\n", pkg->message);
			new_d = pkg->source;
			pkg->type = 0;
			pkg->source = id;
			pkg->destiny = new_d;
			
		}else
			new_d = pkg->destiny;
		
		_send(si_other, new_d, s, pkg);

    }
 
    close(s);
	return;
}

void readf(){
	int i;
	FILE * F;

	if(!(F = fopen("../config/roteador.config", "r"))) return;
	
	//scan all the network information
	while(fscanf(F, "%d", &i)!=EOF){
		fscanf(F, "%hu %s", &ports[i], ips[i]);
		if(feof(F))break;
	}
}


void id_atoi (char * nptr){
	id = atoi(nptr);	
}

