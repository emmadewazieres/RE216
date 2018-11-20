#include "server-tools.h"

//Initialisation of the server
struct sockaddr_in init_serv_addr(const char* port){
  int portno;
  struct sockaddr_in saddr_in;
  memset(&saddr_in,'\0', sizeof(saddr_in));
  portno = atoi(port);
  saddr_in.sin_port = htons(portno);
  saddr_in.sin_family = AF_INET;
  saddr_in.sin_addr.s_addr = INADDR_ANY;
  return(saddr_in);
}

//Binding
int do_bind(int sock, const struct sockaddr *adr, int adrlen){
  int bind_value = bind(sock,adr, adrlen);
  if (bind_value == -1)
  {
    perror("ERROR binding failed");
    exit(EXIT_FAILURE);
  }
}

//Listening
int do_listen(int socket,int backlog){
  int listening = listen(socket,backlog);
  if (listening == -1){
    perror("ERROR listening failed");
    exit(EXIT_FAILURE);
  }
  return(listening);
}

//Acceptation
int do_accept(int socket, struct sockaddr* addr, socklen_t *addrlen){
  int sock_client = accept(socket,addr, addrlen);
  if (sock_client == -1){
    perror("ERROR acceptation failed");
    exit(EXIT_FAILURE);
  }
  return(sock_client);
}

//Closing
void do_close(int sockfd){
  int closing = close(sockfd);
  if (closing == -1){
    perror("ERROR closing failed");
    exit(EXIT_FAILURE);
  }
}

//Pooling
void do_poll(struct pollfd *tab_fd){
  int valeur_poll = poll(tab_fd, NUMBER_OF_CONNECTION+1,-1);
  if (valeur_poll == -1){
    perror("ERROR poll failed");
    exit(EXIT_FAILURE);
  }
  if (valeur_poll == 0){
    perror("ERROR time out");
    exit(EXIT_FAILURE);
  }
}



//Initialisation of the chained list
struct client* client_list_init(){
  struct client *client_list_init;
  client_list_init=malloc(sizeof(*client_list_init));
  if (client_list_init==NULL){
    perror("ERROR client chained list");
    exit(EXIT_FAILURE);
  }
  return client_list_init;
}

//Adding a client to the chained list
struct client* add_client(struct client* client_list, char *pseudo, int socket_number){
  struct client *new_client;
  new_client=malloc(sizeof(*new_client));
  if (new_client==NULL){
    perror("ERROR new client not created");
    exit(EXIT_FAILURE);
  }
  new_client->pseudo = pseudo;
  new_client->socket_number = socket_number;
  new_client->nbe_salon_in = 0;
  new_client->next=client_list;
  return new_client;
}

//Deleting a client from the chained list
struct client *delete_client(struct client *client_list,int socket_fd){
  if (client_list==NULL){
    perror("ERROR deleting client");
    exit(EXIT_FAILURE);
  }
  struct client* tmp;
  struct client* ptmp;
  tmp=client_list;
  if (tmp->socket_number==socket_fd){
    client_list=tmp->next;
    free(tmp);
    return client_list;
  }
  ptmp=tmp->next;
  while (ptmp != NULL){
    if (ptmp->next == NULL && ptmp->socket_number == socket_fd){
      tmp->next = NULL;
      return(client_list);
    }
    if (ptmp->socket_number==socket_fd){
      tmp->next=ptmp->next;
      free(ptmp);
      return(client_list);
    }
    tmp=ptmp;
    ptmp=ptmp->next;
  }
  return client_list;
}

//Finding a client with its socket number
struct client *find_specific_client(struct client *client_list, int socket_number){
  if (client_list==NULL){
    perror("ERROR finding client");
    exit(EXIT_FAILURE);
  }
  struct client *tmp;
  tmp = client_list;
  while(tmp->next != NULL){
    if (tmp->socket_number == socket_number){
      return tmp;
    }
    tmp = tmp->next;
  }
  return tmp;
}

//Checking if a client is in the chained list thanks to its pseudo
int check_pseudo(struct client *client_list, char *pseudo){
  if (client_list==NULL){
    perror("ERROR finding client");
    exit(EXIT_FAILURE);
  }
  struct client *tmp;
  tmp = client_list;
  if (strcmp(tmp->pseudo,pseudo)==0){
    return (1);
  }
  tmp = tmp->next;
  while(tmp->next != NULL){
    if (strcmp(tmp->pseudo,pseudo)==0){
      return(1);
    }
    tmp = tmp->next;
  }
  return(0);
}

//Finding a client with its pseudo
struct client *find_specific_client_pseudo(struct client *client_list, char *pseudo){
  if (client_list==NULL){
    perror("ERROR finding client");
    exit(EXIT_FAILURE);
  }
  struct client *tmp;
  tmp = client_list;

  while(tmp->next != NULL){
    if (strcmp(tmp->pseudo,pseudo)==0){
      return tmp;
    }
    tmp = tmp->next;
  }
  if (tmp->next == NULL && strcmp(tmp->pseudo,pseudo)==0){
    return tmp;
  }
}

//Sending the users that are connected (that are in the chained list) to a client
void who(struct client *client_list,int socket_question){
  if (client_list==NULL){
    perror("ERROR who function");
    exit(EXIT_FAILURE);
  }
  struct client *tmp;
  tmp =client_list;
  char *client_pseudo;
  char *list = malloc(MAX_LENGTH_MESSAGE);
  char *list_of_client = malloc(MAX_LENGTH_MESSAGE);
  char *message=malloc(MAX_LENGTH_MESSAGE);
  sprintf(message,"Online users are :\n");
  int i = 0;
  while(tmp->next != NULL){
    i++;
    client_pseudo = tmp->pseudo;
    sprintf(list,"- %s \n",client_pseudo);
    strcat(list_of_client,list);
    tmp = tmp->next;
  }
  strcat(message,list_of_client);
  do_send(socket_question,message);
}

//Sending information about a client to the client that asks for them
void whois(struct client *client_list,char*message,struct client*current_client){
  char *pseudowhois;
  pseudowhois=message+7;
  struct client *user;
  if (check_pseudo(client_list,pseudowhois)==0){
    char *error ="This user doesn't exist\n";
    do_send(current_client->socket_number,error);
  }
  else {
    user=find_specific_client_pseudo(client_list,pseudowhois);
    if (client_list==NULL){
      perror("ERROR whois function");
      exit(EXIT_FAILURE);
    }
    char *message = malloc(MAX_LENGTH_MESSAGE);
    sprintf(message,"Connected since %s with IP address %s and port number %d\n",client_list->date,client_list->IP_address,client_list->port_number);
    do_send(current_client->socket_number,message);
  }
}

//Giving the position of the first space in a string
int position_first_space(char *chaine){
  int lenght = strlen(chaine);
  int compteur = 0;
  int i = 0;
    while(chaine[i]!=' ' && i <=lenght ){
      compteur = compteur +1;
      i++;
    }
    return compteur;
}

//Initialisation of the salon chained list
struct salon* salon_list_init(){
  struct salon *salon_list_init;
  salon_list_init=malloc(sizeof(*salon_list_init));
  if (salon_list_init==NULL){
    perror("ERROR salon chained list");
    exit(EXIT_FAILURE);
  }
  return salon_list_init;
}

//Adding a salon to the salon chained list
struct salon* add_salon(struct salon* salon_list,char *name, char *pseudo, int socket_number){
  struct salon *new_salon;
  new_salon=malloc(sizeof(*new_salon));
  if (new_salon==NULL){
    perror("ERROR new salon not created");
    exit(EXIT_FAILURE);
  }
  new_salon->name = name;
  new_salon->client_salon = client_list_init(salon_list->client_salon);
  new_salon->client_salon = add_client(new_salon->client_salon,pseudo,socket_number);
  new_salon->number_client=1;
  new_salon->next=salon_list;
  return new_salon;
}

//Deleting a salon from the chained list
struct salon *delete_salon(struct salon *salon_list,char *name){
  if (salon_list==NULL){
    perror("ERROR deleting salon");
    exit(EXIT_FAILURE);
  }
  struct salon* tmp;
  struct salon* ptmp;
  tmp=salon_list;
  if (strcmp(tmp->name,name)==0){
    salon_list=tmp->next;
    free(tmp);
    return salon_list;
  }
  ptmp=tmp->next;
  while (ptmp != NULL){
    if ((ptmp->next == NULL) && (strcmp(ptmp->name,name)==0)){
      tmp->next = NULL;
      return(salon_list);
    }
    if (strcmp(ptmp->name,name)==0){
      tmp->next=ptmp->next;
      free(ptmp);
      return(salon_list);
    }
    tmp=ptmp;
    ptmp=ptmp->next;
  }
  return salon_list;
}

//Checking if a salon is in the chained list thanks to its name
int check_name(struct salon *salon_list, char *name){
  if (salon_list==NULL){
    perror("ERROR finding salon");
    exit(EXIT_FAILURE);
  }
  struct salon *tmp;
  tmp = salon_list;
  if (strcmp(tmp->name,name)==0){
    return (1);
  }
  tmp = tmp->next;
  while(tmp->next != NULL){
    if (strcmp(tmp->name,name)==0){
      return(1);
    }
    tmp = tmp->next;
  }
  return(0);
}

//Finding a salon with its name
struct salon *find_specific_salon_name(struct salon *salon_list, char *name){
  if (salon_list==NULL){
    perror("ERROR finding salon");
    exit(EXIT_FAILURE);
  }
  struct salon *tmp;
  tmp = salon_list;

  while(tmp->next != NULL){
    if (strcmp(tmp->name,name)==0){
      return tmp;
    }
    tmp = tmp->next;
  }
  if (tmp->next == NULL && strcmp(tmp->name,name)==0){
    return tmp;
  }
}

//Adding client in a specific salon
void add_client_in_salon(struct salon *salon_list,char *name,struct client *client_to_add){
  struct salon *asked_salon = find_specific_salon_name(salon_list,name);
  printf("dans add salon %s",asked_salon->name);

  asked_salon->client_salon = add_client(asked_salon->client_salon,client_to_add->pseudo,client_to_add->socket_number);

  asked_salon->number_client+=1;
  printf("nombre clientdans salon = %d\n",asked_salon->number_client);
}

//Sending the salons that have been created (that are in the salon chained list) to a client
int which(struct salon *salon_list,int socket_question){
  if (salon_list==NULL){
    perror("ERROR which function");
    exit(EXIT_FAILURE);
  }
  struct salon *tmp;
  tmp = salon_list;
  char *salon_name = malloc(MAX_LENGTH_MESSAGE);
  char *liste_of_salon = malloc(MAX_LENGTH_MESSAGE);
  char *temp = malloc(MAX_LENGTH_MESSAGE);
  char *message = malloc(MAX_LENGTH_MESSAGE);
  sprintf(message,"Salons are :\n");
  if(tmp->next == NULL){
    liste_of_salon = "There is not any salon yet.\n";
    do_send(socket_question,liste_of_salon);
    return 0;
  }
  while(tmp->next != NULL){
    salon_name = tmp->name;
    sprintf(temp,"- %s \n",salon_name);
    strcat(liste_of_salon,temp);
    tmp = tmp->next;
  }
  strcat(message,liste_of_salon);
  do_send(socket_question,message);
  return 0;
}

//Sending the clients that are in a specific salon
void whoisin(struct salon *salon_list,int socket_question, char *message,int current_salon){
  char* name_salon = message + 9;
  if (salon_list==NULL){
    perror("ERROR whoisin function");
  }
  if(current_salon == 0){
    char *text = "This salon doesn't exist.\n";
    do_send(socket_question,text);
  }
  else if(check_name(salon_list,name_salon) == 1){
    struct salon *salon_cible = find_specific_salon_name(salon_list,name_salon);

    struct client *tmp;
    tmp =salon_cible->client_salon;
    char *client_pseudo;
    char *name = supp_last_caractere(name_salon);
    char *list = malloc(MAX_LENGTH_MESSAGE);
    char *list_of_client = malloc(MAX_LENGTH_MESSAGE);
    char *message=malloc(MAX_LENGTH_MESSAGE);
    sprintf(message,"Users in %s are :\n",name);
    int i = 0;
    while(tmp->next != NULL){
      i++;
      client_pseudo = tmp->pseudo;
      sprintf(list,"- %s \n",client_pseudo);
      strcat(list_of_client,list);
      tmp = tmp->next;
    }
    strcat(message,list_of_client);
    do_send(socket_question,message);
  }
  else {
    char *text = "This salon doesn't exist.\n";
    do_send(socket_question,text);
  }
}

/*struct salon *leave_salon_apres_quit(struct salon *salon_list,struct client *current_client,int socket_fd){
  struct salon *tmp_salon;
  tmp_salon = salon_list;
  int cmp = 0;
  if(tmp_salon->next == NULL){
    printf("tmp_salon == NULL\n");
  }
  while(tmp_salon->next != NULL){
    printf("salut\n");
    if(check_pseudo(tmp_salon->client_salon, current_client->pseudo) == 1){
      cmp ++;
      printf("cmp = %d\n",cmp);
      tmp_salon->client_salon=delete_client(tmp_salon->client_salon,socket_fd);
      tmp_salon->number_client-=1;
    }
  tmp_salon = tmp_salon->next;
}
  return (tmp_salon);
}*/

//--------------------------------------------------------------------------------------------------
//Dealing with the client pseudo
void nick(char *message,int current_connection,struct client*current_client,struct client *client_list){
  char *realpseudo=message+6;
  int length=strlen(realpseudo);
  if (current_connection==1){
    if (current_client->pseudo==NULL){
      char *welcome = "Welcome on the chat !\n";
      do_send(current_client->socket_number,welcome);
      current_client->pseudo = realpseudo;
    }
    else {
      char *realpseudo=message+6;
      current_client->pseudo=realpseudo;
      char *change = "Your nickname has been updated.\n";
      do_send(current_client->socket_number,change);
    }
  }
  else {
    if (check_pseudo(client_list->next,realpseudo)==0){
      if (current_client->pseudo==NULL){
        char *welcome = "Welcome on the chat !\n";
        do_send(current_client->socket_number,welcome);
        current_client->pseudo = realpseudo;
      }
      else {
        char *realpseudo=message+6;
        current_client->pseudo=realpseudo;
        char *change = "Your nickname has been updated.\n";
        do_send(current_client->socket_number,change);
      }
    }
    else {
      char *pseudo_error ="This pseudo already exits, choose another one.\n";
      do_send(current_client->socket_number,pseudo_error);
    }
  }
}

//Sending a message to a specific client
void msg(char *message, struct client *client_list,struct client *current_client){
  char *pseudo_msg = message + 5;
  int length_pseudo = position_first_space(pseudo_msg);
  char *dest_pseudo = malloc(MAX_LENGTH_MESSAGE);
  strncpy(dest_pseudo,pseudo_msg,length_pseudo);
  dest_pseudo[length_pseudo] = '\n';
  struct client *dest_client;
  if (check_pseudo(client_list,dest_pseudo)==0){
    char *error ="This user doesn't exist\n";
    do_send(current_client->socket_number,error);
  }
  else {
    char *pseudo_current = malloc(MAX_LENGTH_MESSAGE);
    int length_current = strlen(current_client->pseudo);
    strncpy(pseudo_current,current_client->pseudo,length_current-1);
    char *msg = pseudo_msg + length_pseudo;
    char *sending = malloc(MAX_LENGTH_MESSAGE);
    sprintf(sending,"[%s to you] : %s",pseudo_current,msg);
    dest_client=find_specific_client_pseudo(client_list,dest_pseudo);
    do_send(dest_client->socket_number,sending);
  }
}

//Joining an existing salon
void join(char *message, struct salon*salon_list,struct client *current_client){
  char* name_salon = message + 6;
  if(check_name(salon_list,name_salon) == 1){
    struct salon *cible_salon = find_specific_salon_name(salon_list,name_salon);
    if(check_pseudo(cible_salon->client_salon, current_client->pseudo) == 0){
      char *join = malloc(MAX_LENGTH_MESSAGE);
      sprintf(join,"You joined salon %s ",name_salon);
      add_client_in_salon(salon_list,name_salon,current_client);
      current_client->nbe_salon_in++;
      do_send(current_client->socket_number,join);
    }
    else{
      char *presence = "You are already a member of this salon.\n";
      do_send(current_client->socket_number,presence);
    }
  }
  else{
    char *existence = "This salon doesn't exist.\n";
    do_send(current_client->socket_number,existence);
  }
}
