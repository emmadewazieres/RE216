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

//Create the socket
int do_socket(int domaine, int type, int protocol){
  int sockfd;
  sockfd = socket(domaine,type,protocol);
  //check for validity
  if (sockfd == -1)
  {
      perror("ERROR socket creation failed");
      exit(EXIT_FAILURE);
  }
  else
    return(sockfd);
}

//Init the server address structure
void init_serv_addr(const char* port, struct sockaddr_in *serv_addr){
  int portno;
  memset(&serv_addr,0, sizeof(serv_addr));
  portno = atol(port);
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
}

//binding
int do_bind(int sock, const struct sockaddr *adr, int adrlen){
  int bind_value;
  bind_value = bind(sock,(struct sockaddr*)&adr,adrlen);
  if (bind_value == -1)
  {
      perror("ERROR binding failed");
      exit(EXIT_FAILURE);
  }
}

//specify the socket to be a server socket and listen for at most 20 concurrent client
int do_listen(int socket){
  int test=listent(socket,20);
  if (test==-1){
    perror("ERROR listening failed");
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

    //Create the socket
    int sock = do_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)

    //Init the server address structure
    struct sockaddr_in saddr_in;
    init_serv_addr(argv[1],saddr_in);

    //perform the binding
    //we bind on the tcp port specified
    do_bind(sock,(struct sockaddr*)&saddr_in,sizeof(saddr_in));


    //specify the socket to be a server socket and listen for at most 20 concurrent client
    do_listen(sock);

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
