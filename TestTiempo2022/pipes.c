#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h> 
#include <sys/wait.h>
#include <stdlib.h>
#define PIPELIMIT 10
#define ONEPASS 1



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
    // printf("%d",argc);
    if(argc>1) memToUse=atoi(argv[1]);
    else memToUse=100;
    printf("using %ld bytes of memory!\n",memToUse);

    int fdpipe[2];
    int r,pid;
    r = pipe(fdpipe);
    if(r<0) myError("error creating pipe");

    int fdread = fdpipe[0];
    int fdwrite = fdpipe[1];

      pid=fork();
    if(pid<0) myError("error forking");

    if(pid==0) 
    {
        //server/writer
        close(fdread);
        FILE * f =fopen("myTest","r");
        fseek(f, 0, SEEK_END);
        if(ftell(f)<memToUse) myError("more data asked than availabable, not");
        fseek(f, 0, SEEK_SET);  

        char *data = (char *) malloc(memToUse + 1);   //sizeof not need for char
        data[memToUse] = '\0';

        
        fread(data, memToUse, ONEPASS, f); //move to RAM
        fclose(f); //we finished reading close

        // for(int i=0;i<memToUse;i++)putchar(data[i]); //to display even if nulls

        int counter = 100;
        int k = 0;
        while(memToUse>k){

            r=write(fdwrite,data+k,PIPELIMIT);
            if(r<0) myError("error sending data");
            k=k+r;
    
        }
        free(data);
        close(fdwrite);
    }
    else
    {   //client/reader
        close(fdwrite);
        int c;
        char buffer[10];
        while(read(fdread,buffer,PIPELIMIT)!=0) printf("%s",buffer); 
        //read here is non blocking but we can detect there was no change
        close(fdread);
    }

    



}