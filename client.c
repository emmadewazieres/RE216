#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<time.h>

#define MAX_LENGHT_MESSAGE 1000

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

//Connection with the server
void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
  int connection = connect(sockfd,addr,addrlen);
  if (connection !=0){
    perror("ERROR connection failed");
    exit(EXIT_FAILURE);
  }
}

//Get what the client has to say
char *readline(){
  char *message = malloc(MAX_LENGHT_MESSAGE);
  fgets(message,MAX_LENGHT_MESSAGE,stdin);
  return(message);
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
 fflush(stdout);
 }
 return(sending);
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
    fflush(stdout);
  }
  return (reception);
}


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

    //Chatting with the server
    char *text = malloc(MAX_LENGHT_MESSAGE);
    char *message = malloc(MAX_LENGHT_MESSAGE);

    do_recv(socket,message, MAX_LENGHT_MESSAGE,0);
    printf("The server has told you : %s",message);

    if (strcmp(message,"Too many clients, connection failed. Come back later\n")==0){
      return 0;
    }
    while (strncmp(text,"/nick ",6)!=0){
      printf("Please logon with /nick <your pseudo>\n");
      text=readline();
      if (strcmp(text,"/quit\n") == 0){

        do_send(socket,text,MAX_LENGHT_MESSAGE,0);
        do_recv(socket,message, MAX_LENGHT_MESSAGE,0);
        printf("The server has told you : %s",message);
        return(0);
      }
    }
    do_send(socket,text,MAX_LENGHT_MESSAGE,0);
    do_recv(socket,message, MAX_LENGHT_MESSAGE,0);
    printf("The server has told you : %s",message);

    while (strcmp(text,"/quit\n") != 0){

        //Getting user input
        text = readline();
        do_send(socket,text,MAX_LENGHT_MESSAGE,0);

        if (strcmp(text,"/who\n")==0){

          char *currentco=malloc(MAX_LENGHT_MESSAGE);
          do_recv(socket,currentco, MAX_LENGHT_MESSAGE,0);
          int current_connection=atoi(currentco);
          printf("Online users are :\n");
          for (int i=0;i<current_connection;i++){
            do_recv(socket,message, MAX_LENGHT_MESSAGE,0);
            printf("- %s\n",message);
            fflush(stdout);
          }
        }
        else if (strncmp(text,"/whois \n",7) == 0){
          do_recv(socket,message, MAX_LENGHT_MESSAGE,0);

          printf("%s",message);
        }
        else {
          do_recv(socket,message, MAX_LENGHT_MESSAGE,0);
          printf("The server has told you : %s\n",message);
          fflush(stdout);
        }
      }


    return 0;

}
