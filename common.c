#include "common.h"

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

//Sending
void do_send(int sockfd, const void *text){
 int sending = send(sockfd, text, MAX_LENGTH_MESSAGE, 0);
 if (sending == -1){
   perror("ERROR sending failed");
   exit(EXIT_FAILURE);
 }
 if (sending != MAX_LENGTH_MESSAGE){
 printf("The message has not been fully transmitted \n");
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
    printf("The message has not been fully transmitted \n");
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
