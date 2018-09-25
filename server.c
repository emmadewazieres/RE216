#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int do_socket(){
  int sockfd;
  int yes=1;
  //create the socket
  sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  printf("%d\n",sockfd);
  //check for validity

  if (sockfd == -1)
  {
      perror("socket");
      exit(EXIT_FAILURE);
  }
  else
    return(sockfd);
}

 struct sockaddr_in init_serv_addr(const char* port){

  int portno;
  //clean the serv_add structure
  struct sockaddr_in saddr_in;
  memset(&saddr_in,'\0', sizeof(saddr_in));

  //cast the port from a string to an int
  portno = atoi(port);
  saddr_in.sin_port = htons(portno);
  //internet family protocol
  saddr_in.sin_family = AF_INET;

  //we bind to any ip form the hostname
  saddr_in.sin_addr.s_addr = INADDR_ANY;

  return(saddr_in);

}

int do_bind(int sock, const struct sockaddr *adr, int adrlen){

  int bind_value;
  bind_value = bind(sock,adr, adrlen);

  if (bind_value == -1)
  {
      perror("bind");
      exit(EXIT_FAILURE);
  }
}

int do_listen(int socket,int backlog){
  int valeur_listen;
  valeur_listen = listen(socket,backlog);
  if (valeur_listen == -1){
    perror("listen");
    exit(EXIT_FAILURE);
  }
  return(valeur_listen);
}

int do_accept(int socket, struct sockaddr* addr, socklen_t *addrlen){
  int valeur_accept;
  valeur_accept = accept(socket,addr, addrlen);
  printf("valeur_accept%d\n",valeur_accept);
  if (valeur_accept == -1){
    perror("accept");
    exit(EXIT_FAILURE);
  }
  return(valeur_accept);
}


int main(int argc, char** argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }


    //create the socket, check for validity!
    int socket_serveur;
    socket_serveur=do_socket();


    //init the serv_add structure
    struct sockaddr_in saddr_in;

    saddr_in = init_serv_addr(argv[1]);


    //perform the binding
    //we bind on the tcp port specified
    //Explication parametre 2 page 65
    do_bind(socket_serveur,(struct sockaddr *)&saddr_in,sizeof(saddr_in));
    printf("brbh");

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    //listen()
    int valeur_listen;
    valeur_listen = do_listen(socket_serveur,SOMAXCONN);


    for (;;)
    {

        //accept connection from client
        int valeur_accept;
        socklen_t taille = sizeof(saddr_in);
        socklen_t* addrlen = &taille;
        valeur_accept = do_accept(socket_serveur,(struct sockaddr *)&saddr_in,addrlen);
        printf("valeur_accept dans main%d\n",valeur_accept);

        //read what the client has to say
        //do_read()

        //we write back to the client
        //do_write()

        //clean up client socket
    }

    //clean up server socket

    return 0;
}
