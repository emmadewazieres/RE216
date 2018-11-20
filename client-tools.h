#ifndef CLIENTTOOLS_H_
#define CLIENTTOOLS_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "common.h"

//Connection with the server
void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

//Pooling
void do_poll(struct pollfd *tab_fd);

//Get what the client has to say
char *readline();

//Sending info to the server
void info_server(char *name_IP, struct sockaddr_in sock_host, int socket);

#endif
