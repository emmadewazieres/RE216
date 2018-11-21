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

//Get what the client has to say
char *readline();

//Sending info to the server
void info_server(char *name_IP, struct sockaddr_in sock_host, int socket);

//Generating a random number between a and b
int rand_a_b(int a,int b);

#endif
