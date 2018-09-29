#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include<poll.h>

#define MAX_LENGHT_MESSAGE 1000
#define NBE_CONNEXION 20

void error(const char *msg){
    perror(msg);
    exit(1);
}

//Creation of the socket
int do_socket(){
  int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if (sockfd == -1) //validity check
  {
      perror("ERROR creation failed");
      exit(EXIT_FAILURE);
  }
  else
    return(sockfd);
}

//Initialisation of the server
 struct sockaddr_in init_serv_addr(const char* port){
  int portno;
  struct sockaddr_in saddr_in;
  memset(&saddr_in,'\0', sizeof(saddr_in));
  portno = atoi(port);
  saddr_in.sin_port = htons(portno);
  saddr_in.sin_family = AF_INET;
  saddr_in.sin_addr.s_addr = INADDR_ANY;
  return(saddr_in);
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

//Listening
int do_listen(int socket,int backlog){
  int listening = listen(socket,backlog);
  if (listening == -1){
    perror("ERROR listening failed");
    exit(EXIT_FAILURE);
  }
  return(listening);
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

//Reception
int do_recv(int sockfd, void *buf, int len, unsigned int flags){
  int reception=recv(sockfd,buf,len,flags);
  if (reception==-1){
    perror("ERROR reception failed");
    exit(EXIT_FAILURE);
  }
  if (reception != len){
    printf("The message has not been fully transmitted \n");
  }
  return (reception);
}

//Sending
int do_send(int sockfd, const void *msg, size_t len, int flags){
 int sending;
 sending = send(sockfd, msg, len, flags);
 if (sending == -1){
   perror("ERROR sending failed");
   exit(EXIT_FAILURE);
 }
 if (sending != len){
 printf("The message has not been fully transmitted \n");
 }
 return(sending);
}

//Closing
void do_close(int sockfd){
  int closing = close(sockfd);
  if (closing == -1){
    perror("ERROR closing failed");
    exit(EXIT_FAILURE);
  }
}

int do_poll(struct pollfd *tab_fd, int time_out){
  int valeur_poll = poll(tab_fd, NBE_CONNEXION,time_out);
  printf("poll%d\n",valeur_poll);
  if (valeur_poll == -1){
    perror("ERROR poll failed");
    exit(EXIT_FAILURE);
  }
  if (valeur_poll == 0){
    perror("ERROR depassement temps attente poll");
    exit(EXIT_FAILURE);
  }
  return(valeur_poll);
}

int main(int argc, char** argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }


    //Creation of the socket
    int socket_server;
    socket_server=do_socket();

    //Initialisation of the servor
    struct sockaddr_in saddr_in;
    saddr_in = init_serv_addr(argv[1]);

    //Binding
    do_bind(socket_server,(struct sockaddr *)&saddr_in,sizeof(saddr_in));


    //Création de la structure pollfds
    struct pollfd tab_fd[NBE_CONNEXION + 1];
    memset(tab_fd,0,sizeof(tab_fd));

    //1ere case de tab_fd dédiée à la socket d'écoute du serveur
    tab_fd[0].fd=socket_server;
    tab_fd[0].events=POLLIN;
    printf("valeur fd socket ecoute serveur %d\n",tab_fd[0].fd);


    int compteur_fd;
    compteur_fd = 1;
    int current_place;
    current_place = 1;
    int nbe_connexions_actuelles;
    nbe_connexions_actuelles = 0;

int time_out = 15*1000; //15 secondes #plus que JC !!



while(1){

  do_listen(tab_fd[0].fd,SOMAXCONN);
  printf("penbb\n");

  int valeur_poll = do_poll(tab_fd,time_out);
  printf("valeur poll %d\n",valeur_poll);
printf("salut");
  //Listening
printf("avant for");
  int j;
  for (j = 1; j<22; j++){
printf("dans for %d",j);
    if(tab_fd[0].revents == POLLIN && tab_fd[j].fd == 0){

      //accept connection from client
      int sock_client;
      socklen_t taille = sizeof(saddr_in);
      socklen_t* addrlen = &taille;
      sock_client = do_accept(socket_server,(struct sockaddr *)&saddr_in,addrlen);

      tab_fd[j].fd = sock_client;
      tab_fd[j].revents = POLLIN;
      if (j > NBE_CONNEXION){
        char *trop_message = "trop de connexions, reviens plus tard.\n";
        do_send(sock_client,trop_message,MAX_LENGHT_MESSAGE,0);
        do_close(sock_client);
        tab_fd[j].fd = 0;
      }
    }

      if(tab_fd[j].fd != 0 && tab_fd[j].revents == POLLIN){

    char *message = malloc(MAX_LENGHT_MESSAGE);

while ((strcmp(message,"/quit\n") != 0)){

        do_recv(tab_fd[j].fd,message,MAX_LENGHT_MESSAGE,0);
        printf("The client avec socket n°%d has sent you : %s\n",tab_fd[j].fd,message);

        if ((strcmp(message,"/quit\n") != 0)){
          //we write back to the client
          do_send(tab_fd[j].fd,message,MAX_LENGHT_MESSAGE,0);
          break;
        }
        else {
          char *last_message = "Closing connection.\n";
          do_send(tab_fd[j].fd,last_message,MAX_LENGHT_MESSAGE,0);
          printf("Closing both client avec socket n° %d and server sockets.\n",tab_fd[j].fd);
          do_close(tab_fd[j].fd);
          tab_fd[j].fd = 0;
        }
      }

    }
    //clean up client socket


  }
  }
    //clean up server socket
    do_close(socket_server);

//}
    return 0;
}
