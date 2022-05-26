#include <stdio.h>
#include <string.h> //for bzero
#include <sys/types.h>
#include <netinet/in.h> //for socket structs
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h> //for transforming ip to bytes
// #include randomgenerator

#define SUBPROTOCOL 0
#define LENADDRESS  8
#define BACKLOG  8 //max users waiting for responde
#define PORT  3535
#define FLAGS  0
#define UDPLIMIT 65535


void myError(const char * message){
    perror(message);
    exit(1);
}
// int argv,char ** argc
int main(int argc,char ** argv){
    int r; // for error logging 
    int fdsv,fdcl; //server client file descriptors


    //Get "space" for socket from OS
    fdcl = socket(AF_INET,SOCK_STREAM,SUBPROTOCOL); //INET=ipv4,STREAM=UDP
    if (setsockopt(fdcl, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    myError("setsockopt(SO_REUSEADDR) failed");
    if(fdcl<0) myError("error creating socket");

    // // Customize server socket
    struct sockaddr_in client;
    client.sin_family = AF_INET; //ip v4
    client.sin_port = htons(PORT);//htons for endianess compat
    inet_aton("127.0.0.1",&client.sin_addr); // put address of availabable socket
    bzero(client.sin_zero,8); // cause if for byte size compatability with regular sockets 

    //Setting up client
    
    socklen_t socklen = (socklen_t) sizeof( struct sockaddr);
    r = connect(fdcl,(struct sockaddr *) &client,socklen);
    if(r==-1) myError("error connecting to server");

    int maxData;
    if(argc>1) maxData = atoi(argv[1]);
    else maxData=1000; //1kb default i.e 1000 chars
    char * data = (char *)malloc(maxData+1);
    data[maxData]= '\0'; //asummes no more than 1 gb of transit
    char * stream = (char *)malloc(UDPLIMIT+1);
    stream[UDPLIMIT] = '\0';

    //recieve size of comunication
    r = recv(fdcl,stream,UDPLIMIT,FLAGS);
    if(r<0) myError("error recibiendo");
    stream[r]='\0'; //this is crucial as there could be trash before getting to a null char in stream 
    int dataToRecieve=atoi(stream);
    printf("data to get is %d bytes!",dataToRecieve);
    if(dataToRecieve>maxData) myError("asking reciving more data that space availabable");



    int k = 0;
    int counter = 0;
    while(dataToRecieve>0 && counter++<1000000){


        r = recv(fdcl,stream,UDPLIMIT,FLAGS);
        if(r<0) myError("error recibiendo");
        stream[r]='\0'; //evade trash before getting to a real null char in stream 
        memcpy(data+k,stream,r);
        k=k+r;
        dataToRecieve=dataToRecieve-r;
        

    }


    //OUTPUT FINAL STRING
    // for(int i=0;i<=maxData;i++){
    //     if(data[i]<31) putchar('%'); 
    //     else putchar(data[i]);
    // }
    printf("\n%s\n",data);
    close(fdcl);
    free(stream);
    free(data);


}
