all:
	mkdir bin/
	gcc src/udp_router.c src/router_lib.c -pthread -O2 -o bin/router.sh
