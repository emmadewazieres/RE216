#include "client-tools.h"

int main(int argc,char** argv)
{

  if (argc != 3)
  {
    fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
    return 1;
  }

  //get address info from the server
  struct sockaddr_in sock_host;
  memset(&sock_host,'\0',sizeof(sock_host));
  sock_host.sin_family = AF_INET;
  sock_host.sin_port = htons(atoi(argv[2]));
  inet_aton(argv[1], &sock_host.sin_addr);

  //Creation of the socket
  int socket = do_socket();

  //Connection to the server
  do_connect(socket,(struct sockaddr *)&sock_host, sizeof(sock_host));

  //Creation of the pollfd structure
  struct pollfd tab_fd[2];
  memset(tab_fd,0,sizeof(tab_fd));

  //Initialisation of the structure
  tab_fd[0].fd=socket;
  tab_fd[0].events=POLLIN;
  tab_fd[1].fd=0;
  tab_fd[1].events=POLLIN;

  //Chatting with the server
  char *text = malloc(MAX_LENGTH_MESSAGE);
  char *message = malloc(MAX_LENGTH_MESSAGE);
  do_recv(socket,message);

  if (strcmp(message,"Too many clients, connection failed. Come back later.\n")==0){
    printf("The server has told you : %s",message);
    return 0;
  }

  while (strncmp(text,"/nick ",6)!=0)  {
    printf("Please logon with /nick <your pseudo>\n");
    text=readline();
    if (strcmp(text,"/quit\n") == 0){
      do_send(socket,text);
      do_recv(socket,message);
      printf("The server has told you : %s",message);
      return(0);
    }
  }
  do_send(socket,text);
  do_recv(socket,message);
  printf("The server has told you : %s",message);

  while (strcmp(message,"This pseudo already exits, choose another one.\n")==0){
    printf("Please logon with /nick <your pseudo>\n");
    text=readline();
    if (strcmp(text,"/quit\n") == 0){
      do_send(socket,text);
      do_recv(socket,message);
      printf("The server has told you : %s",message);
      return(0);
    }
    do_send(socket,text);
    do_recv(socket,message);
    printf("The server has told you : %s",message);
  }
  // Sending information at the RE216_SERVER
  char *name_IP = malloc(MAX_LENGTH_MESSAGE);
  gethostname(name_IP,MAX_LENGTH_MESSAGE);
  info_server(name_IP,sock_host, socket);
  for (;;) { //endless loop

    //Polling
    do_poll(tab_fd);

    if (tab_fd[0].revents==POLLIN){
      //Receiving messages
      do_recv(socket,message);
      printf("%s",message);
      fflush(stdout);
    }

    if (tab_fd[1].revents==POLLIN){
      //Getting user input
      text = readline();
      do_send(socket,text);

      if (strcmp(text,"/quit\n") == 0){
        do_recv(socket,message);
        if (strcmp(message,"Closing connection.\n")==0){
          printf("%s",message);
          free(text);free(message);
          return(0);
        }
        else{
          printf("The server has told you : %s",message);
        }
      }
    }
  }
}
