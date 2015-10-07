#include "router_lib.h"


int main(int argc, char ** argv){
	pthread_t sender, timeout;

	id_atoi(argv[1]);
	
	readf();

	//starting list
	_list = NULL;

	nextHop = bellman_ford(N_ROT, id);
	
	nextHop = next_hop(N_ROT, id, nextHop);

	//Message identifier starts in 0
	message_id = 0;
	
	//thread waiting for message
	pthread_create(&sender, NULL, source, NULL);

	pthread_create(&timeout, NULL, thread_resend, NULL);
	
	//listening -- main process
	destiny();


    return 0;
}
