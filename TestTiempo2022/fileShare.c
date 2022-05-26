#include <stdlib.h> //atoi,etc
#include <stdio.h> // fopen,fileno...
#include <sys/sendfile.h> //for moving between files
#include <fcntl.h>
#include <unistd.h> //for fork
#include <sys/types.h>// for wait
#include <sys/wait.h>
#include <sys/stat.h>

#define BUF_SIZE 4096*1000

void myError(char * message){
    perror(message);
    exit(1);
}
int min(int a,int b){
    if(a<b) return a;
    else return b;
}


int main(int argc, char ** argv){
    long memToUse;
    if(argc>1) memToUse =atoi(argv[1]);
    else memToUse = 100;

    FILE * stream;
    int pid=fork();

    if(pid<0) myError("error forking");
    if(pid==0) 
    {
        //server/writer
        struct stat stat_buf;
        int fromfd = open("myTest", O_RDONLY);
        if(fromfd<0) myError( "error opening file");
        fstat(fromfd, &stat_buf);
        int tofd = open("stream2", O_WRONLY | O_CREAT, stat_buf.st_mode);
        if(tofd<0) myError("error creating file for comunication");
        int n = 1;
        while (n > 0) {

        n = sendfile(tofd, fromfd, 0, min(BUF_SIZE,memToUse));}

        // fseek(f, 0, SEEK_END);
        // if(ftell(f) < memToUse) myError("\nmore data asked than availabable. not");
        // fseek(f, 0, SEEK_SET);  
        
        // char * buffer = malloc(memToUse);
        // // char * offset = buffer;
        // int counter =0;
        // read(fdf,offset,memToUse);
        // while(counter<100 ){
        //     putchar(*(buffer+counter));
        //     counter++;
        // }
        // // fseek(f, 0, SEEK_SET);
        // offset = buffer;
        // write(fdstream,offset,memToUse);
        close(fromfd); //we finished reading close
        close(tofd);

        // for(int i=0;i<memToUse;i++)putchar(data[i]); //to display even if nulls

    }   
    else
    {   
        FILE * stream;
        if(stream<0) myError("error creating file for comunication");

        //client/reader
        int * waitResponse;
        wait(waitResponse);
        if(waitResponse<0) myError("error exiting writter");
        stream = fopen("stream2","r");

        printf("\n");

        char buffer;
        int counter =memToUse;
        while(counter-- ){
            fread(&buffer,1,1,stream);
            putchar(buffer);
            }

      
        // while(read(fileno(stream),buffer,1)!=0 && *buffer != EOF) putchar(*buffer);

        fclose(stream);

    }
}       