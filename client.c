#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void init_client_addr(int port, struct sockaddr_in *sock_host){
memset(&sock_host,'\0',sizeof(sock_host));
sock_host->sin_family=AF_INET;
sock_host->sin_port=htons(port);
inet_aton("127.0.0.0",sock_host->sin_addr);
}

//Create the socket
int do_socket(int domaine, int type, int protocol){
  int sockfd;
  sockfd=socket(domaine,type,protocol);
  //check for validity
  if (sockfd == -1){
    perror("ERROR socket validity");
    exit(EXIT_FAILURE);
  }
  else {
    return sockfd;
  }
}

//Connect to remote server
int do_connect(int socket,struct sockaddr *address, int address_len){
  int res=connect(socket,(struct sockaddr*)&address,address_len);
  if (res!=0){
    perror("ERROR connection failed");
    exit(EXIT_FAILURE);
  }
}

//send message to the server
ssize_t sendline(int fd, const void *str, size_t maxlen){
  //d'abord on lit le message puis on l'écrit dans le serveur
  int sent = 0;
  while (sent != maxlen){
    sent += write(fd,&str,maxlen-sent);
  }
}

int main(int argc,char** argv){

    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

//get address info from the server
struct sockaddr_in *sock_host;
init_client_addr(argv[2],sock_host);

//get the socket
int s=do_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

//connect to remote socket
do_connect(s,(struct sockaddr *)&sock_host,sizeof(sock_host));

//get user input
char message[255];
printf("Veuillez entrer votre message : ");
fgets(message,255,stdin);

//send message to the server

//handle_client_message()


    return 0;


}
