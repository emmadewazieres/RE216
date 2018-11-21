#ifndef SERVERTOOLS_H_
#define SERVERTOOLS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include "common.h"

#define NUMBER_OF_CONNECTION 20

//Initialisation of the chained list
struct client* client_list_init();

//Adding a client to the chained list
struct client* add_client(struct client* client_list, char *pseudo, int socket_number);

//Deleting a client from the chained list
struct client *delete_client(struct client *client_list,int socket_fd);

//Finding a client with its socket number
struct client *find_specific_client(struct client *client_list, int socket_number);

//Checking if a client is in the chained list thanks to its pseudo
int check_pseudo(struct client *client_list, char *pseudo);

//Finding a client with its pseudo
struct client *find_specific_client_pseudo(struct client *client_list, char *pseudo);

//Sending the users that are connected (that are in the chained list) to a client
void who(struct client *client_list,int socket_question);

//Sending information about a client to the client that asks for them
void whois(struct client *client_list,char*message,struct client*current_client);

//Initialisation of the salon chained list
struct salon* salon_list_init();

//Adding a salon to the salon chained list
struct salon* add_salon(struct salon* salon_list,char *name, char *pseudo, int socket_number);

//Deleting a salon from the chained list
struct salon *delete_salon(struct salon *salon_list,char *name);

//Checking if a salon is in the chained list thanks to its name
int check_name(struct salon *salon_list, char *name);

//Finding a salon with its name
struct salon *find_specific_salon_name(struct salon *salon_list, char *name);

//Adding client in a specific salon
void add_client_in_salon(struct salon *salon_list,char *name,struct client *client_to_add);

//Sending the salons that have been created (that are in the salon chained list) to a client
void which(struct salon *salon_list,int socket_question);

//Sending the clients that are in a specific salon
void whoisin(struct salon *salon_list,int socket_question, char *message,int current_salon);

//Sending in which salon(s) the client asking is
void mysalon(struct salon *salon_list,struct client* current_client);

//Dealing with the client pseudo
void nick(char *message,int current_connection,struct client*current_client,struct client* client_list);

//Sending a message to a specific client
void msg(char *message, struct client *client_list,struct client *current_client);

//Joining an existing salon
void join(char *message, struct salon*salon_list,struct client *current_client);

#endif
