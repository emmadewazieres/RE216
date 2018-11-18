#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#define MAX_LENGTH_MESSAGE 1000
#define NUMBER_OF_CONNECTION 20

void error(const char *msg){
  perror(msg);
  exit(1);
}

//Creation of the socket
int do_socket(){
  int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if (sockfd == -1) //validity check
  {
    perror("ERROR creation failed");
    exit(EXIT_FAILURE);
  }
  else
  return(sockfd);
}

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

//Sending
int do_send(int sockfd, const void *text){
  int sending;
  sending = send(sockfd, text, MAX_LENGTH_MESSAGE, 0);
  if (sending == -1){
    perror("ERROR sending failed");
    exit(EXIT_FAILURE);
  }
  if (sending != MAX_LENGTH_MESSAGE){
    printf("The message has not been fully transmitted \n");
  }
  return(sending);
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
int do_poll(struct pollfd *tab_fd){
  int valeur_poll = poll(tab_fd, NUMBER_OF_CONNECTION+1,-1);
  if (valeur_poll == -1){
    perror("ERROR poll failed");
    exit(EXIT_FAILURE);
  }
  if (valeur_poll == 0){
    perror("ERROR time out");
    exit(EXIT_FAILURE);
  }
  return(valeur_poll);
}

//Declatation of the chained list for the clients
struct client {
  char *pseudo;
  int socket_number;
  int socket_fd;
  char *IP_address;
  int port_number;
  char *date;
  struct client *next;
};

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
  printf("je suis passée dans who");
  struct client *tmp;
  tmp =client_list;
  char *client_pseudo;
  char *list = malloc(MAX_LENGTH_MESSAGE);
  char *list_of_client = malloc(MAX_LENGTH_MESSAGE);
  int i = 0;
  while(tmp->next != NULL){
    i++;
    client_pseudo = tmp->pseudo;
    sprintf(list,"- %s \n",client_pseudo);
    strcat(list_of_client,list);
    printf("list %s",list_of_client);
    printf("nbe passage while du who %d\n",i);
    tmp = tmp->next;
  }
  do_send(socket_question,list_of_client);
}

//Sending information about a client to the client that asks for them
void whois(struct client *client_list, int socket_question){
  if (client_list==NULL){
    perror("ERROR whois function");
    exit(EXIT_FAILURE);
  }
  char *message = malloc(MAX_LENGTH_MESSAGE);
  sprintf(message,"Connected since %s with IP address %s and port number %d\n",client_list->date,client_list->IP_address,client_list->port_number);
  do_send(socket_question,message);
}

int position_first_space(char *chaine){

  int lenght = strlen(chaine);
  printf("lenght %d\n",lenght);
  int compteur = 0;
  int i = 0;
    while(chaine[i]!=' ' && i <=lenght ){
      compteur = compteur +1;
      i++;
    }
    return compteur;
}

//Declatation of the chained list for the salon
struct salon {
  char *name;
  struct client *client_salon;
  int number_client;
  struct salon *next;
};

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
void which(struct salon *salon_list,int socket_question){
  if (salon_list==NULL){
    perror("ERROR which function");
    exit(EXIT_FAILURE);
  }
  struct salon *tmp;
  tmp = salon_list;
  char *salon_name = malloc(MAX_LENGTH_MESSAGE);
  char *liste_of_salon = malloc(MAX_LENGTH_MESSAGE);

  char *temp = malloc(MAX_LENGTH_MESSAGE);
  if(tmp->next == NULL){
    liste_of_salon = "There is not any salon yet.\n";
  }
  while(tmp->next != NULL){
    salon_name = tmp->name;
    sprintf(temp,"- %s \n",salon_name);
    strcat(liste_of_salon,temp);
    tmp = tmp->next;
  }

  do_send(socket_question,liste_of_salon);
}

void whoisin(struct salon *salon_list,int socket_question, char *name,int current_salon){
  if (salon_list==NULL){
    perror("ERROR whoisin function");
  }
  if(current_salon == 0){
    char *text = "This salon doesn't exist.\n";
    do_send(socket_question,text);
  }
  else if(check_name(salon_list,name) == 1){
    struct salon *salon_cible = find_specific_salon_name(salon_list,name);
    printf("avant who slaon %s\n",salon_cible->name);
    if (salon_cible->number_client==1){
      printf("new_salon->number_client %d\n",salon_cible->number_client);
      printf("whoisin une seule personne\n");
      who(salon_cible->client_salon,socket_question);
    }
    else {
    printf("new_salon->number_client %d\n",salon_cible->number_client);
    printf("whoisin 2 personnes\n");
    who(salon_cible->client_salon,socket_question);
    }
  }
  else {
    char *text = "This salon doesn't exist.\n";
    do_send(socket_question,text);
  }
}

char *supp_last_caractere(char *chaine){
  int length = strlen(chaine);
  char *short_chaine = malloc(MAX_LENGTH_MESSAGE);
  strncpy(short_chaine,chaine,length-1);
  return(short_chaine);
}

int main(int argc, char** argv)
{

  if (argc != 2)
  {
    fprintf(stderr, "usage: RE216_SERVER port\n");
    return 1;
  }


  //Creation of the socket
  int socket_server;
  socket_server=do_socket();

  //Initialisation of the servor
  struct sockaddr_in saddr_in;
  saddr_in = init_serv_addr(argv[1]);

  // Initialisation of client list (chained list)
  struct client* client_list = client_list_init();
  struct client* current_client;
  int nbe_client = 0;

  // Initialisation of salon list (chained list)
  struct salon* salon_list = salon_list_init();
  int current_salon=0;

  //Binding
  do_bind(socket_server,(struct sockaddr *)&saddr_in,sizeof(saddr_in));


  //Creation of the pollfd structure
  struct pollfd tab_fd[NUMBER_OF_CONNECTION+1];
  memset(tab_fd,0,sizeof(tab_fd));

  //Initialisation of the structure
  tab_fd[0].fd=socket_server;
  tab_fd[0].events=POLLIN;
  for (int i=0;i<=NUMBER_OF_CONNECTION;i++){
    tab_fd[i].events=POLLIN;
  }

  //Listening
  do_listen(socket_server,SOMAXCONN);
  int current_connection=0;

  for (;;) { //endless loop

    //Polling
    do_poll(tab_fd);

    for(int i=0;i<=NUMBER_OF_CONNECTION+1;i++) {
      if (tab_fd[i].revents==POLLIN){
        if (i==0){
          socklen_t taille = sizeof(saddr_in);
          socklen_t* addrlen = &taille;
          int sock_client = do_accept(socket_server,(struct sockaddr *)&saddr_in,addrlen);
          current_connection+=1;
          for (int j=0;j<=NUMBER_OF_CONNECTION+1;j++){
            if (tab_fd[j].fd==0){
              tab_fd[j].fd=sock_client;
              printf("Connecting with client n°%d. %d current connection(s).\n",j,current_connection);
              fflush(stdout);
              char *hello="Hello client !\n";
              do_send(tab_fd[j].fd,hello);
              client_list=add_client(client_list,NULL,tab_fd[j].fd);
              break;
            }
          }
          if (current_connection==NUMBER_OF_CONNECTION+1){
            int supp = sock_client;
            char *error = "Too many clients, connection failed. Come back later\n";
            do_send(supp,error);
            do_close(supp);
            current_connection-=1;
          }
        }
        else {
          current_client=find_specific_client(client_list,tab_fd[i].fd);
          char *message = malloc(MAX_LENGTH_MESSAGE);
          do_recv(tab_fd[i].fd,message);

          if (strncmp(message,"/nick ",6)==0){
          char *realpseudo=message+6;
          int length=strlen(realpseudo);
          fflush(stdout);
          if (current_connection<=1){
            if (current_client->pseudo==NULL){
              char *welcome = "Welcome on the chat !\n";
              do_send(tab_fd[i].fd,welcome);
              current_client->pseudo=realpseudo;
            }
            else {
              char *realpseudo=message+6;
              current_client->pseudo=realpseudo;
              char *change = "Your nickname has been updated.\n";
              do_send(tab_fd[i].fd,change);
            }
          }
          else {
          if (check_pseudo(client_list->next,realpseudo)==0){
              if (current_client->pseudo==NULL){
                char *welcome = "Welcome on the chat !\n";
                do_send(tab_fd[i].fd,welcome);
                current_client->pseudo=realpseudo;
              }
              else {
                char *realpseudo=message+6;
                current_client->pseudo=realpseudo;
                char *change = "Your nickname has been updated.\n";
                do_send(tab_fd[i].fd,change);
              }
            }
            else {
              char *pseudo_error ="This pseudo already exits, choose another one.\n";
              do_send(tab_fd[i].fd,pseudo_error);
            }
          }
}
          else if (strncmp(message,"/IP ",4)==0){
            char *IP = message + 4;
            current_client->IP_address = IP;
          }

          else if (strncmp(message,"/port ",6)==0){
            char *port = message + 6;
            int port_n = atoi(port);
            current_client->port_number = port_n;
          }

          else if (strncmp(message,"/date ",6)==0){
            char *date = message + 6;
            current_client->date = date;
          }

          else if ((strcmp(message,"/quit\n") == 0)){
            client_list = delete_client(client_list,tab_fd[i].fd);
            current_connection-=1;
            char *last_message = "Closing connection.\n";
            do_send(tab_fd[i].fd,last_message);
            printf("Closing client n°%d connection.%d current connection(s).\n",i,current_connection);
            fflush(stdout);
            do_close(tab_fd[i].fd);
            tab_fd[i].fd=0;
          }

          else if ((strcmp(message,"/who\n") == 0)){
            who(client_list,tab_fd[i].fd);
          }

          else if ((strncmp(message,"/whois \n",7) == 0)){
            char *pseudowhois;
            pseudowhois=message+7;
            struct client *user;
            if (check_pseudo(client_list,pseudowhois)==0){
              char *error ="This user doesn't exist\n";
              do_send(tab_fd[i].fd,error);
            }
            else {
              user=find_specific_client_pseudo(client_list,pseudowhois);
              whois(user,tab_fd[i].fd);
            }
          }

          else if ((strncmp(message,"/all \n",5) == 0)){
            if(current_connection < 2){
              do_send(tab_fd[i].fd,"Il n'y a que toi\n");
            }
            char *message_all = message + 5;
            int length = strlen(current_client->pseudo);
            char *sender = malloc(MAX_LENGTH_MESSAGE);
            strncpy(sender, current_client->pseudo,length-1);
            char *all=malloc(MAX_LENGTH_MESSAGE);
            sprintf(all,"[%s to everyone] : %s\n",sender,message_all);
            for(int k=1;k<=current_connection;k++) {
              if (k != i){
                do_send(tab_fd[k].fd,all);

              }
            }
          }

          else if ((strncmp(message,"/msg \n",5) == 0)){
            char *pseudo_msg = message + 5;

            int length_pseudo = position_first_space(pseudo_msg);
            char *dest_pseudo = malloc(MAX_LENGTH_MESSAGE);
            strncpy(dest_pseudo,pseudo_msg,length_pseudo);
            dest_pseudo[length_pseudo] = '\n';
            struct client *dest_client;
            if (check_pseudo(client_list,dest_pseudo)==0){
              char *error ="This user doesn't exist\n";
              do_send(tab_fd[i].fd,error);
            }
            else {
              char *pseudo_current = malloc(MAX_LENGTH_MESSAGE);
              int length_current = strlen(current_client->pseudo);
              strncpy(pseudo_current,current_client->pseudo,length_current-1);
              char *msg = pseudo_msg + length_pseudo;
              char *sending = malloc(MAX_LENGTH_MESSAGE);
              sprintf(sending,"[%s to you] : %s\n",pseudo_current,msg);
              dest_client=find_specific_client_pseudo(client_list,dest_pseudo);
              do_send(dest_client->socket_number,sending);
            }

          }

          else if ((strncmp(message,"/create \n",8) == 0)){
             char* name = message + 8;
             if((current_salon < 1) || (check_name(salon_list,name) == 0)){
               salon_list=add_salon(salon_list, name,current_client->pseudo,current_client->socket_number);
               current_salon++;
               printf("nbe salon %d\n",current_salon);
               char *creation=malloc(MAX_LENGTH_MESSAGE);
               char *short_name =  malloc(MAX_LENGTH_MESSAGE);
               short_name = supp_last_caractere(name);
               sprintf(creation,"You have created salon %s\n",short_name);
               do_send(tab_fd[i].fd,creation);
             }

             else{
               char *existence = "This name already exits, choose another one.\n";
               do_send(tab_fd[i].fd,existence);
             }
           }

           else if ((strncmp(message,"/join \n",6) == 0)){
             char* name_salon = message + 6;
             printf("name_salon %s\n",name_salon);
             if(check_name(salon_list,name_salon) == 1){
               printf("coucou1 ");
               struct salon *cible_salon = find_specific_salon_name(salon_list,name_salon);
               printf("coucou2 : %s",cible_salon->name);
              if(check_pseudo(cible_salon->client_salon, current_client->pseudo) == 0){
                 printf("coucou3");
                 char *join = malloc(MAX_LENGTH_MESSAGE);
                 sprintf(join,"you joined salon %s ",name_salon);
                 add_client_in_salon(salon_list,name_salon,current_client);
                 printf("ici\n");
                 do_send(tab_fd[i].fd,join);
             }
               else{
                char *presence = "You are already a member of this salon.\n";
                  do_send(tab_fd[i].fd,presence);
                 }
             }
             else{
               char *existence = "This salon doesn't exist.\n";
               do_send(tab_fd[i].fd,existence);
             }


           }

           else if ((strncmp(message,"/which\n",7) == 0)){
             which(salon_list,tab_fd[i].fd);
           }

           else if ((strncmp(message,"/whoisin\n",8) == 0)){
             char* name_salon = message + 9;
             printf("name_salon %s\n",name_salon);
             printf("taille %ld\n",strlen(name_salon));

             /*char *variable=malloc(MAX_LENGTH_MESSAGE);
             variable = who2(client_list,tab_fd[i].fd);
             printf("variable %s\n",variable);*/
             whoisin(salon_list,current_client->socket_number,name_salon,current_salon);
           }

           else if (strncmp(message,"/leave ",7)==0){
             char *name_salon=message +7;
             if (check_name(salon_list,name_salon)==1){
               struct salon* salon_to_update = find_specific_salon_name(salon_list,name_salon);
                 who(salon_to_update->client_salon,tab_fd[i].fd);
               if (check_pseudo(salon_to_update->client_salon,current_client->pseudo)==1){
                 salon_to_update->client_salon=delete_client(salon_to_update->client_salon,tab_fd[i].fd);
                 salon_to_update->number_client-=1;
                 printf("nombre cients dans salon %d\n",salon_to_update->number_client);
                 char *leave = malloc(MAX_LENGTH_MESSAGE);
                 sprintf(leave,"You left salon %s.\n",name_salon);
                 do_send(tab_fd[i].fd,leave);
                 who(salon_to_update->client_salon,tab_fd[i].fd);
               }
               else {
                 char *error = "You are not in this salon, you can't leave it.\n";
                 do_send(tab_fd[i].fd,error);
               }
               if (salon_to_update->number_client==0){
                 salon_list = delete_salon(salon_list,name_salon);
                 current_salon --;
                 printf("current_salon = %d \n",current_salon);
               }
             }
             else {
               char *existence_salon = "This salon doesn't exist.\n";
               do_send(tab_fd[i].fd,existence_salon);
             }
           }



            else if (strncmp(message,"/salon ",7)==0){
              char *name_salon = message + 7;
              printf("name_salon %s\n",name_salon);

              int length_name = position_first_space(name_salon);
              char *dest_name = malloc(MAX_LENGTH_MESSAGE);
              strncpy(dest_name,name_salon,length_name);
              dest_name[length_name] = '\n';
              printf("longueur %d\n",length_name);
              printf("nom %s\n",dest_name);
              char *message_salon = name_salon + length_name + 1;
              printf("message_salon %s\n",message_salon);

              struct salon *dest_salon;
              if (check_name(salon_list,dest_name)==0){
                char *error ="This salon doesn't exist\n";
                do_send(tab_fd[i].fd,error);
              }
              else {

                int length = strlen(current_client->pseudo);
                char *sender = malloc(MAX_LENGTH_MESSAGE);
                strncpy(sender, current_client->pseudo,length-1);
                char *group=malloc(MAX_LENGTH_MESSAGE);
                sprintf(group,"[%s in salon] : %s\n",sender,message_salon);

                dest_salon = find_specific_salon_name(salon_list, dest_name);
                if(check_pseudo(dest_salon->client_salon, current_client->pseudo) == 0){
                  do_send(tab_fd[i].fd,"t es pas dans le salon \n");
                }
                else{
                  int number_client;
                  number_client = dest_salon->number_client;
                  for(int k=1;k<=number_client;k++) {
                    if (k != i){
                      do_send(tab_fd[k].fd,group);

                    }
                  }
                }
              }

            }


            else {
              do_send(tab_fd[i].fd,message);
              printf("The client n°%d has sent you : %s",i,message);
              fflush(stdout);
            }
          }
        }
      }
    }



  //clean up server socket
  do_close(socket_server);

  return 0;
}
