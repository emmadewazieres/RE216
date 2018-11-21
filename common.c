#include "common.h"

//Initialisation of the server
struct sockaddr_in init_serv_addr(int port){
  struct sockaddr_in saddr_in;
  memset(&saddr_in,'\0', sizeof(saddr_in));
  saddr_in.sin_port = htons(port);
  saddr_in.sin_family = AF_INET;
  saddr_in.sin_addr.s_addr = INADDR_ANY;
  return(saddr_in);
}

//Creation of the socket
int do_socket(){
  int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if (sockfd == -1)
  {
      perror("ERROR creation failed");
      exit(EXIT_FAILURE);
  }
  else
    return(sockfd);
}

//Binding
int do_bind(int sock, const struct sockaddr *adr, int adrlen){
  int bind_value = bind(sock,adr, adrlen);
  if (bind_value == -1)
  {
    perror("ERROR binding failed");
    exit(EXIT_FAILURE);
  }
}

//Acceptation
int do_accept(int socket, struct sockaddr* addr, socklen_t *addrlen){
  int sock_client = accept(socket,addr, addrlen);
  if (sock_client == -1){
    perror("ERROR acceptation failed");
    exit(EXIT_FAILURE);
  }
  return(sock_client);
}

//Listening
int do_listen(int socket,int backlog){
  int listening = listen(socket,backlog);
  if (listening == -1){
    perror("ERROR listening failed");
    exit(EXIT_FAILURE);
  }
  return(listening);
}

//Pooling
void do_poll(struct pollfd *tab_fd,int nb){
  int valeur_poll = poll(tab_fd, nb,-1);
  if (valeur_poll == -1){
    perror("ERROR polling failed");
    exit(EXIT_FAILURE);
  }
  if (valeur_poll == 0){
    perror("ERROR time out");
    exit(EXIT_FAILURE);
  }
}

//Closing
void do_close(int sockfd){
  int closing = close(sockfd);
  if (closing == -1){
    perror("ERROR closing failed");
    exit(EXIT_FAILURE);
  }
}

//Sending
void do_send(int sockfd, const void *text){
 int sending = send(sockfd, text, MAX_LENGTH_MESSAGE, 0);
 if (sending == -1){
   perror("ERROR sending failed");
   exit(EXIT_FAILURE);
 }
 if (sending != MAX_LENGTH_MESSAGE){
 printf("The message has not been fully transmitted.\n");
 fflush(stdout);
 }
}

//Reception
int do_recv(int sockfd, void *message){
  int reception=recv(sockfd,message,MAX_LENGTH_MESSAGE,0);
  if (reception==-1){
    perror("ERROR reception failed");
    exit(EXIT_FAILURE);
  }
  if (reception != MAX_LENGTH_MESSAGE){
    printf("The message has not been fully transmitted.\n");
  }
  return (reception);
}

//Erasing the last character of a string
char *supp_last_caractere(char *chaine){
  int length = strlen(chaine);
  char *short_chaine = malloc(256*sizeof(char));
  strncpy(short_chaine,chaine,length-1);
  return(short_chaine);
}

//Giving the position of the first space in a string
int position_first_space(char *chaine){
  int lenght = strlen(chaine);
  int compteur = 0;
  int i = 0;
  while(chaine[i]!=' ' && i <=lenght ){
    compteur = compteur +1;
    i++;
  }
  return compteur;
}
