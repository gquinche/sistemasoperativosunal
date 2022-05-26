all: server.c client.c
	gcc server.c -o server
	gcc client.c -o client
	gcc pipes.c -o pipes
	gcc fileShare.c -o fileShare
	gcc shmem.c -o shmem
	
100server: 
	gcc server.c -o server
	gcc client.c -o client
	./server 100

mbserver: 
	gcc server.c -o server
	gcc client.c -o client
	./server 1000000