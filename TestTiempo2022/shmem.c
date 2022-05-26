//for shared mem
#include <sys/ipc.h>
#include <sys/shm.h>
//for fork
#include <sys/types.h>
#include <unistd.h>
//for fread,open,putchar... 
#include <stdio.h>
//for wait
#include <sys/wait.h>

#define FLAGS 0
#define ONEPASS 1


void myError(char * message){
    perror(message);
    exit(1);
}



int main(int argc, char ** argv){
    int fdsm;
    char * ap;
    long memToUse;
    if(argc>1) memToUse =argv[1];
    else memToUse =100;
    fdsm=shmget("1234",memToUse,FLAGS);
    if(fdsm<0) myError("couldn't allocate memory from kernel");

    ap=shmat(fdsm,NULL,FLAGS);

    pid=fork();
    if(pid<0) myError("error forking");

    if(pid==0) 
    {
        //server/writer
        FILE * f =fopen("myTest","r");
        fseek(f, 0, SEEK_END);
        if(ftell(f)<memToUse); myError("more data asked than availabable")
        fseek(f, 0, SEEK_SET);  

        // char *data = (char *) malloc(memToUse + 1);   //sizeof not need for char
        data[memToUse] = '\0';

        
        fread(ap, memToUse, ONEPASS, f); //move to RAM
        fclose(f); //we finished reading close

        // for(int i=0;i<memToUse;i++)putchar(data[i]); //to display even if nulls

        free(ap);
    }   
    else
    {   
        //client/reader
        int * waitResponse;
        wait(waitResponse);
        if(waitResponse<0) myError("error exiting writter");

        char buffer[10];
        for(int =k;k<memToUse;k++) putchar(ap[k]);
        while(read(fdread,buffer,PIPELIMIT)!=0) printf("%s",buffer);
        //read here is non blocking but we can detect there was no change
        close(fdread);
    }
        close(fdsm) ;
        free(ap);

    








}