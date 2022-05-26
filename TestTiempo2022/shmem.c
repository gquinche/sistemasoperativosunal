#include <sys/ipc.h> //for shared mem
#include <sys/shm.h>
#include <sys/types.h> //for fork
#include <unistd.h>
#include <stdio.h> //for fread,open,putchar... 
#include <sys/wait.h> //for wait
#include <stdlib.h> // for exit

#define FLAGS 0666
#define ONEPASS 1
#define KEY 1234 //didn't work
#define ABSMEM NULL

void myError(char * message){
    perror(message);
    exit(1);
}



int main(int argc, char ** argv){
    int fdsm;
    char * ap;
    long memToUse;
    if(argc>1) memToUse =atoi(argv[1]);
    else memToUse = 100;    fdsm=shmget(IPC_PRIVATE,memToUse+1,FLAGS);
    if(fdsm<0) myError("couldn't allocate memory from kernel");

    ap=(char *) shmat(fdsm,ABSMEM,FLAGS); //null for not es

    int pid=fork();
    if(pid<0) myError("error forking");

    if(pid==0) 
    {
        //server/writer
        FILE * f =fopen("myTest","r");
        fseek(f, 0, SEEK_END);
        // printf("%ld\n",ftell(f));
        // printf("%ld\n",memToUse);
        // printf("difference was%ld",ftell(f) - memToUse);
        if(ftell(f) < memToUse) myError("\nmore data asked than availabable. not");
        fseek(f, 0, SEEK_SET);  

        // char *data = (char *) malloc(memToUse + 1);   //sizeof not need for char
        ap[memToUse] = '\0';

        
        fread(ap, memToUse, ONEPASS, f); //move to RAM
        fclose(f); //we finished reading close

        // for(int i=0;i<memToUse;i++)putchar(data[i]); //to display even if nulls

    }   
    else
    {   
        //client/reader
        int * waitResponse;
        wait(waitResponse);
        if(waitResponse<0) myError("error exiting writter");

        char buffer[10];
        for(int k=0;k<memToUse;k++) putchar(ap[k]);
        //read here is non blocking but we can detect there was no change
    }
        close(fdsm) ;
        shmdt(ap);

    








}