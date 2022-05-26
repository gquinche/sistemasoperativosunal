#include <stdio.h>
#include <string.h> //for bzero
#include <sys/types.h>
#include <netinet/in.h> //for socket structs
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define SUBPROTOCOL 0
#define LENADDRESS  8
#define BACKLOG  8 //max users waiting for responde
#define PORT 3535 
#define FLAGS 0
#define UDPLIMIT 65535



int min(int a,int b){
    if(a<b) return a;
    else return b;
}

void myError(char * message){
    perror(message);
    exit(1);
}

void sendData(int charsToSend,int fdcl){
    
    int r;
    // char str[UDPLIMIT]; //for comunication

    // what fits udp

    //what we want to send
    FILE * f =fopen("myTest","r");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  


    char *data = (char*) malloc(fsize + 1);   //sizeof not need for char
    data[fsize] = '\0';
    char * stream= (char *)malloc(UDPLIMIT+1);
    stream[UDPLIMIT]='\0';
    
    fread(data, fsize, 1, f); //move to RAM
    fclose(f); //we finished reading close


    sprintf(stream,"%d is chars to send",charsToSend); //read ints as string
    printf("%s\n",stream);

    r=send(fdcl,stream,strlen(stream),FLAGS);
    if(r!=strlen(stream)) myError("data size not send correctly");
    sleep(1); //to evade conflicting data to send and next messages
    // rcv(fdcl,response,strlen(str),FLAGS)

    int k = 0;
    while(charsToSend>0 && fsize-k>0){
        memcpy(stream,data+k,min(min(UDPLIMIT,fsize-k),charsToSend));
        //to evade discrepancy when using in client strcpy on a shorter string
        r=send(fdcl,stream,min(UDPLIMIT,charsToSend),FLAGS);
        if(r<0) myError("error sending data");
        charsToSend=charsToSend-r;
        k=k+r;
  
    }
    free(stream);
    free(data);
}


int main (int argc, char ** argv){

    int r; // for error logging 
    int fdsv,fdcl; //server client file descriptors


    //Get "space" for socket from OS
    fdsv = socket(AF_INET,SOCK_STREAM,SUBPROTOCOL); //INET=ipv4,STREAM=UDP 0 subtype of udp proctol
    if(fdsv<0) myError("error creating socket");
    if (setsockopt(fdsv, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    myError("setsockopt(SO_REUSEADDR) failed");
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
    fdcl = accept(fdsv, (struct sockaddr *) &client,&socklen); 
    if(r<0) myError("error sending data");


    int dataToSend;
    if(argc>1) dataToSend = atoi(argv[1]);
    else dataToSend=1000; //1kb default i.e 1000 chars

    sendData(dataToSend,fdcl);

    close(fdsv);
    close(fdcl);


}
