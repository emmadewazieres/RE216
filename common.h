#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <time.h>
#include <poll.h>

//Declatation of the chained list for the clients
struct client {
  char *pseudo;
  int socket_number;
  char *IP_address;
  int port_number;
  char *date;
  int nbe_salon_in;
  struct client *next;
};

//Declatation of the chained list for the salon
struct salon {
  char *name;
  struct client *client_salon;
  int number_client;
  struct salon *next;
};


#define MAX_LENGTH_MESSAGE 1000

//Creation of the socket
int do_socket();

//Sending
void do_send(int sockfd, const void *text);

//Reception
int do_recv(int sockfd, void *message);

//Erasing the last character of a string
char *supp_last_caractere(char *chaine);

#endif
