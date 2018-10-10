#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#define MAX_LENGHT_MESSAGE 1000
#define NUMBER_OF_CONNECTION 2

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

//Pooling
int do_poll(struct pollfd *tab_fd){
  int valeur_poll = poll(tab_fd, NUMBER_OF_CONNECTION+1,-1);
  if (valeur_poll == -1){
    perror("ERROR poll failed");
    exit(EXIT_FAILURE);
  }
  if (valeur_poll == 0){
    perror("ERROR time out");
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


    //Creation of the pollfd structure
    struct pollfd tab_fd[NUMBER_OF_CONNECTION+1];
    memset(tab_fd,0,sizeof(tab_fd));
    //Initialisation of the structure
    tab_fd[0].fd=socket_server;
    tab_fd[0].events=POLLIN;
    for (int i=0;i<=NUMBER_OF_CONNECTION;i++){
      tab_fd[i].events=POLLIN;
    }
    //Listening
    do_listen(socket_server,SOMAXCONN);

    int current_connection=0;

    for (;;) {

    //Polling
    do_poll(tab_fd);

    for(int i=0;i<=NUMBER_OF_CONNECTION+1;i++) {
      if (tab_fd[i].revents==POLLIN){
        if (i==0){
        socklen_t taille = sizeof(saddr_in);
        socklen_t* addrlen = &taille;
        int sock_client = do_accept(socket_server,(struct sockaddr *)&saddr_in,addrlen);
        current_connection+=1;
        for (int j=0;i<=NUMBER_OF_CONNECTION+1;i++){
          if (tab_fd[i].fd==0){
            tab_fd[i].fd=sock_client;
            printf("Connection with client n°%d. %d current connection(s).\n",i,current_connection);
            fflush(stdout);
            break;
          }
        }
        if (current_connection==NUMBER_OF_CONNECTION+1){
          char *error = "Too many clients, connection failed. Come back later\n";
          do_send(sock_client,error,MAX_LENGHT_MESSAGE,0);
          close(sock_client);
          current_connection-=1;
        }
      }
      else {
        char *message = malloc(MAX_LENGHT_MESSAGE);
          do_recv(tab_fd[i].fd,message,MAX_LENGHT_MESSAGE,0);
          printf("The client n°%d has sent you : %s\n",i,message);
          if ((strcmp(message,"/quit\n") != 0)){
            //we write back to the client
            do_send(tab_fd[i].fd,message,MAX_LENGHT_MESSAGE,0);
          }
          else {
            current_connection-=1;
            char *last_message = "Closing connection.\n";
            do_send(tab_fd[i].fd,last_message,MAX_LENGHT_MESSAGE,0);
            printf("Closing client n°%d connection.%d current connection(s).\n",i,current_connection);
            do_close(tab_fd[i].fd);
            tab_fd[i].fd=0;
          }
        }
      }
    }
} //fin boucle infinie


    //clean up server socket
    do_close(socket_server);

    return 0;
}
