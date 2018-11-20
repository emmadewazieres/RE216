#include "client-tools.h"

//Connection with the server
void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
  int connection = connect(sockfd,addr,addrlen);
  if (connection !=0){
    perror("ERROR connection failed");
    exit(EXIT_FAILURE);
  }
}

//Pooling
void do_poll(struct pollfd *tab_fd){
  int valeur_poll = poll(tab_fd, 2,-1);
  if (valeur_poll == -1){
    perror("ERROR poll failed");
    exit(EXIT_FAILURE);
  }
  if (valeur_poll == 0){
    perror("ERROR time out");
    exit(EXIT_FAILURE);
  }
}

//Get what the client has to say
char *readline(){
  char *message = malloc(MAX_LENGTH_MESSAGE);
  fgets(message,MAX_LENGTH_MESSAGE,stdin);
  return(message);
}

//Sending info to the server
void info_server(char *name_IP, struct sockaddr_in sock_host, int socket){
  // IP
  char *IP_address = malloc(MAX_LENGTH_MESSAGE);
  sprintf(IP_address,"/IP %s",name_IP);
  do_send(socket,IP_address);
  // port
  char *port = malloc(MAX_LENGTH_MESSAGE);
  sprintf(port,"/port %d",sock_host.sin_port);
  do_send(socket,port);
  // date
  struct tm* date;
  time_t timer;
  time(&timer);
  date = localtime(&timer);
  char *date_s = malloc(MAX_LENGTH_MESSAGE);
  sprintf(date_s,"/date %d/%d/%d at %dh %dm %ds \n", date->tm_mday,date->tm_mon + 1, date->tm_year + 1900, date->tm_hour, date->tm_min, date->tm_sec);
  do_send(socket,date_s);
}
