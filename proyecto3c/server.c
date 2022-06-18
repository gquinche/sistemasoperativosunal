#include <stdio.h>
#include<sys/types.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include "hashs.c"
#include <time.h>
#pragma comment(lib, "ws2_32.lib")
#define LIMITWD 100

char SendBuff[512],RecvBuff[512];


struct buscador{
    float id;
    float tiempo;
}; 
  

int main(int argc, char *argv[]){
  
  time_t current_time;
  struct tm *myTime; 
  // char* c_time_string;


  FILE *fp, *log; 
  //WSADATA wsaData;
  int conn_socket,comm_socket;
  int  comunicacion;
  struct sockaddr_in server;
  struct sockaddr_in client;
  struct hostent *hp;
  int resp,stsize;
  
  //Inicializamos la DLL de sockets
  //resp=WSAStartup(MAKEWORD(1,0),&wsaData);
  //if(resp){
    //printf("Error al inicializar socket\n");
    //getchar();return resp;
  //}
  
  //Obtenemos la IP que usará nuestro servidor... 
  // en este caso localhost indica nuestra propia máquina...
  hp=(struct hostent *)gethostbyname("localhost");

  if(!hp){
    printf("No se ha encontrado servidor...\n");
    getchar();
  }

  // Creamos el socket...
  conn_socket=socket(AF_INET,SOCK_STREAM, 0);
  if(conn_socket==-1) {
    printf("Error al crear socket\n");
    getchar();
  }
  
  memset(&server, 0, sizeof(server)) ;
  //memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
  server.sin_addr.s_addr=INADDR_ANY;
  server.sin_family = hp->h_addrtype;
  server.sin_port = htons(6000);

  // Asociamos ip y puerto al socket
  resp=bind(conn_socket, (struct sockaddr *)&server, sizeof(server));
  if(resp<0){
    printf("Error al asociar puerto e ip al socket\n");
    close(conn_socket);
    exit(-1);
  }

  if(listen(conn_socket, 32)==-1){
    printf("Error al habilitar conexiones entrantes\n");
    close(conn_socket);
    exit(-1);
  }
              
  // Aceptamos conexiones entrantes
  // printf("Esperando conexiones entrantes... \n"); 
  stsize=sizeof(struct sockaddr);
  comm_socket=accept(conn_socket,(struct sockaddr *)&client,&stsize);
  if(comm_socket==-1){
    printf("Error al aceptar conexión entrante\n");
    close(conn_socket);
    exit(-1);
  }

  // printf("Conexión entrante desde: %s\n", inet_ntoa(client.sin_addr)); // descomentar para ver en consola ips           
  // Como no vamos a aceptar más conexiones cerramos el socket escucha
  close(conn_socket);
  char *buf;
  buf=(char *)malloc(LIMITWD*sizeof(char));
  if(getcwd(buf,LIMITWD)==NULL){
   printf("Error al escribir working directory");
   close(comm_socket);
  return -1;
  }  
  char senbuff2[512];
  char indice[512];
  char valor [512];
  struct buscador miBuscador;
  struct mistru Lector;   
  int id;  
  log = fopen("logServer.txt","a");
  char logBuffer[100];
  fp=fopen("tabla_datos.bin","rb");
  const char format[] = "[Fecha %d-%d-%d-%d:%d:%d] Cliente [%s] [%.0f - %.0f - %.0f]\n";
  while (1) 
  {
    recv (comm_socket,RecvBuff,sizeof(RecvBuff), 0);
    memcpy(indice,RecvBuff,1); //lee primer byte indicando mensaje
    memcpy(valor,RecvBuff+1,sizeof(RecvBuff)); //lee mensaje

    switch(atoi(indice)){
      case 1:
       Lector.origen= atof(valor);        
        break; 
      case 2:
        Lector.destino= atof(valor);       
        break;
      case 3:
        Lector.hora= atof(valor);        
        break;
      case 4:
        //time logging

        current_time=time(NULL);
        myTime=localtime(&current_time);
        // printf("local time is %s", asctime(myTime)); //para comprobar lectura del tiempo

        id=myhash(Lector);       
        fseek(fp,id,SEEK_SET); //se ubica en la linea que el hash apunta
        fread(&miBuscador,sizeof(miBuscador),1,fp); // lee en fp el tiempo correspondiente
        
       
        if(miBuscador.id!=Lector.origen || Lector.origen == 0){
          miBuscador.tiempo = -1.0f; }
        snprintf(SendBuff, sizeof(SendBuff), "%f", miBuscador.tiempo); //escribe sobre sendbuff usando formateo de tipo printf
        
        //formateo de log

        snprintf(logBuffer,sizeof(logBuffer),format,myTime->tm_year+1900,myTime->tm_mon+1,myTime->tm_mday,myTime->tm_hour,myTime->tm_min,myTime->tm_sec, inet_ntoa(client.sin_addr),Lector.origen,Lector.destino,Lector.hora);
        
        fwrite(logBuffer,strlen(logBuffer),1,log); // como cada char es un byte podemos usar strlen

        send(comm_socket,SendBuff,sizeof(SendBuff),0);        
        break;
      case 5:
        return 0;
      default:       
       return 0;
    }  
    
  } 
  
  //getchar();          

  // Cerramos el socket de la comunicacion
  fclose(fp);
  fclose(log);
  close(comm_socket);  
  // Cerramos liberia winsock
  
  return (EXIT_SUCCESS);
} 