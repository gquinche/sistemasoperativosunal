#include <stdio.h>
#include <string.h> //for bzero
#include <sys/types.h>
#include <netinet/in.h> //for socket structs
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
// #include randomgenerator

#define SUBPROTOCOL 1
#define LENADDRESS  8
#define BACKLOG  8 //max users waiting for responde
#define PORT 3535 
#define FLAGS 0

// setsockopt es una forma de permitir reutilizar un puerto y una ip
//otra opcion es customizar como cierra el programa al recibir un ctrl+c




void myError(char * message){
    perror(message);
    exit(1);
}

void sendData(int bytesToSend,struct sockaddr_in client,int fdcl){
    //for example 1024*1024 for 1mb
    // or 1024

    while(bytesToSend>0){
    char * msg="hola mundo"; //volver aleatorio para evitar caching
    int msgSize = strlen(msg);

    int r= send(fdcl,msg,msgSize,FLAGS);
    if(r<0) myError("error enviando datos");
    bytesToSend=bytesToSend-r;
    }

}


int main (){

    int r; // for error logging 
    int fdsv,fdcl; //server client file descriptors

    //Get "space" for socket from OS
    fdsv = socket(AF_INET,SOCK_STREAM,SUBPROTOCOL); //INET=ipv4,STREAM=UDP 0 subtype of udp proctol
    if(fdsv<0) myError("error creating socket");

    // Customize server socket
    struct sockaddr_in server;
    server.sin_family = AF_INET; //ip v4
    server.sin_port = htons(PORT);//htons for endianess compat
    server.sin_addr.s_addr = INADDR_ANY; // allow any space in wifi or ethernet
    bzero(server.sin_zero,8); // cause if for byte size compatability with regular sockets 
    

    //Setting up server
    socklen_t socklen = sizeof(struct sockaddr);
    r = bind(fdsv,(struct sockaddr *) &server,socklen);
    if(r==-1) myError("error creating input server channel");
    r = listen(fdsv,BACKLOG); //set as a listener server
    if(r==-1) myError("error creating as server");

    //Accept clients and create channel to respond to it, fork here for multiuser
    struct sockaddr_in client; 
    // socklen = (socklen_t) sizeof(sockaddr_in);
    r = accept(fdsv, (struct sockaddr *) &client,&socklen); 
    if(r==-1) myError("error sending data");

    //Send message
    char * msg = "hello world";
    int msgSize = strlen(msg);
    r= send(fdcl,msg,msgSize,FLAGS);
    if(r!= msgSize) myError("incomplete message send");

    close(fdsv);
    close(fdcl);


}