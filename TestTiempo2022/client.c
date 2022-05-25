#include <stdio.h>
#include <string.h> //for bzero
#include <sys/types.h>
#include <netinet/in.h> //for socket structs
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h> //for transforming ip to bytes
// #include randomgenerator

#define SUBPROTOCOL 1
#define LENADDRESS  8
#define BACKLOG  8 //max users waiting for responde
#define PORT  3535 
#define FLAGS  0
#define MAXSIZEMESSAGE  10


void myError(const char * message){
    perror(message);
    exit(1);
}

int main(){
    
    int r; // for error logging 
    int fdsv,fdcl; //server client file descriptors

    //Get "space" for socket from OS
    fdcl = socket(AF_INET,SOCK_STREAM,SUBPROTOCOL); //INET=ipv4,STREAM=UDP
    if(fdcl<0) myError("error creating socket");

    // // Customize server socket
    struct sockaddr_in client;
    client.sin_family = AF_INET; //ip v4
    client.sin_port = htons(PORT);//htons for endianess compat
    inet_aton("192.168.0.4",&client.sin_addr); // put address of availabable socket
    bzero(client.sin_zero,8); // cause if for byte size compatability with regular sockets 

    //Setting up client
    
    socklen_t socklen = (socklen_t) sizeof( struct sockaddr);
    r = connect(fdcl,(struct sockaddr *) &client,socklen);
    if(r==-1) myError("error connecting to server");

    //Recieve and print message
    char buffer[MAXSIZEMESSAGE+1]; 
    // r = recv(fdcl,buffer,(size_t) MAXSIZEMESSAGE,FLAGS);
    r = recv(fdcl,buffer,MAXSIZEMESSAGE,FLAGS);
    if(r<0) myError("error reciving message");
    buffer[r]= '\0';
    printf("Mensaje es %s, \n",buffer);
    // if(r!= msgSize) myError("incomplete message send");

    close(fdcl);


}