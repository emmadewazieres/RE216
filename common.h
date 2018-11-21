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

//Initialisation of the server
struct sockaddr_in init_serv_addr(int port);

//Creation of the socket
int do_socket();

//Binding
int do_bind(int sock, const struct sockaddr *adr, int adrlen);

//Listening
int do_listen(int socket,int backlog);

//Acceptation
int do_accept(int socket, struct sockaddr* addr, socklen_t *addrlen);

//Pooling
void do_poll(struct pollfd *tab_fd,int nb);

//Closing
void do_close(int sockfd);

//Sending
void do_send(int sockfd, const void *text);

//Reception
int do_recv(int sockfd, void *message);

//Erasing the last character of a string
char *supp_last_caractere(char *chaine);

//Giving the position of the first space in a string
int position_first_space(char *chaine);

#endif
