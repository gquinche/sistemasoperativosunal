#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h> 
#include <sys/wait.h>
#include <stdlib.h>
#define PIPELIMIT 16



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

    if(pid==0) //server
    {
        close(fdread);
        FILE * f =fopen("myTest","r");
        fseek(f, 0, SEEK_END);
        long fsize = memToUse;
        fseek(f, 0, SEEK_SET);  


        char *data = (char *) malloc(fsize + 1);   //sizeof not need for char
        // data[fsize] = '\0';
        // printf("prev%s",data);
        // char * stream= (char *)malloc(PIPELIMIT+1);
        // stream[PIPELIMIT]='\0';
        
        fread(data, fsize, 1, f); //move to RAM
        fclose(f); //we finished reading close
        // printf("after%s",data);
        for(int i=0;i<fsize;i++)putchar(data[i]);

        FILE * pipefile;
        pipefile= fdopen(fdwrite,"w");
        int counter = 100;
        int k = 0;
        while(fsize-k>0){

            // printf("%d ",fsize-k);
            
            r=write(fdwrite,data+k,10);
            // printf("\n %d \n",(r));
            if(r<0) myError("error sending data");
            k=k+r;
    
        }
        // free(stream);
        free(data);
        close(fdwrite);
        fclose(pipefile);
    }
    else{ //reader
        // wait(pid);
        close(fdwrite);
        FILE * pipefile;
        int c;
        char buffer[10];
        pipefile = fdopen(fdread, "r");
        while(0!=read(fdread,buffer,10)) printf("%s",buffer); 
        //read here is non blocking but we can detect there was no change
        close(fdread);
        fclose(pipefile);
    }

    



}