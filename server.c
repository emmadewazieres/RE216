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

void init_serv_addr(const char* port, struct sockaddr_in *serv_addr){

  int portno;
  //clean the serv_add structure
  memset(serv_addr,0, sizeof(serv_addr));

  //cast the port from a string to an int
  portno = atol(port);
  serv_addr->sin_port = htons(portno);
  //internet family protocol
  serv_addr->sin_family = AF_INET;

  //we bind to any ip form the hostname
  serv_addr->sin_addr.s_addr = INADDR_ANY;
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
    struct sockaddr_in *saddr_in;
    init_serv_addr(argv[2],saddr_in);

    //perform the binding
    //we bind on the tcp port specified
    //Explication parametre 2 page 65
    do_bind(socket_serveur,(struct sockaddr*)&saddr_in,sizeof(saddr_in));

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    //listen()

    for (;;)
    {

        //accept connection from client
        //do_accept()

        //read what the client has to say
        //do_read()

        //we write back to the client
        //do_write()

        //clean up client socket
    }

    //clean up server socket

    return 0;
}
