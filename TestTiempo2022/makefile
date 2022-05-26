all: server.c client.c
	gcc server.c -o server
	gcc client.c -o client
	gcc pipes.c -o pipes
	gcc fileShare.c -o fileShare
	gcc shmem.c -o shmem
	chmod +rwx bashScript.sh 
	chmod +rwx bashAll.sh 
	bash bashScript.sh
	bash bashAll.sh
	
100server: 
	gcc server.c -o server
	gcc client.c -o client
	./server 100

mbserver: 
	gcc server.c -o server
	gcc client.c -o client
	./server 1000000