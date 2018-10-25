#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

#define MAX_LENGHT_MESSAGE 1000
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
int do_recv(int sockfd, void *buf, int len, unsigned int flags){
  int reception=recv(sockfd,buf,len,flags);
  if (reception==-1){
    perror("ERROR reception failed");
    exit(EXIT_FAILURE);
  }
  if (reception != len){
    printf("The message has not been fully transmitted \n");
  }
  return (reception);
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

//Declatation of the chained list for the salons
struct salon {
  char *name;
  struct client *client_salon;
  int number_client;
  struct salon *next;
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

//Adding a client to the chained list
struct client* add_client(struct client* client_list){
  struct client *new_client;
  new_client=malloc(sizeof(*new_client));
  if (new_client==NULL){
    perror("ERROR new client not created");
    exit(EXIT_FAILURE);
  }
  new_client->next=client_list;
  return new_client;
}

//Adding a salon to the salon chained list
struct salon* add_salon(struct salon* salon_list){
  struct salon *new_salon;
  new_salon=malloc(sizeof(*new_salon));
  if (new_salon==NULL){
    perror("ERROR new salon not created");
    exit(EXIT_FAILURE);
  }
  new_salon->next=salon_list;
  return new_salon;
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

//Deleting a salon from the salon chained list
struct salon *delete_salon(struct salon *salon_list){
  if (salon_list==NULL){
    perror("ERROR deleting salon");
    exit(EXIT_FAILURE);
  }
  struct salon* tmp;
  struct salon* ptmp;
  tmp=salon_list;
  if (tmp->number_client==0){
    salon_list=tmp->next;
    free(tmp);
    return salon_list;
  }
  ptmp=tmp->next;
  while (ptmp != NULL){
    if (ptmp->next == NULL && ptmp->number_client == 0){
      tmp->next = NULL;
      return(salon_list);
    }
    if (ptmp->number_client==0){
      tmp->next=ptmp->next;
      free(ptmp);
      return(salon_list);
    }
    tmp=ptmp;
    ptmp=ptmp->next;
  }
  return salon_list;
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

//Finding a salon with its pseudo
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


//Sending the users that are connected (that are in the chained list) to a client
void who(struct client *client_list,int socket_question,int current_connection){
  if (client_list==NULL){
    perror("ERROR who function");
    exit(EXIT_FAILURE);
  }
  struct client *tmp;
  tmp = client_list;
  char *client_pseudo = malloc(MAX_LENGHT_MESSAGE);
  while(tmp->next != NULL){
    client_pseudo = tmp->pseudo;
    do_send(socket_question,client_pseudo,MAX_LENGHT_MESSAGE,0);
    //printf("longueur %ld\n",strlen(client_pseudo));
    tmp = tmp->next;
  }
}

//Sending information about a client to the client that asks for them
void whois(struct client *client_list, int socket_question){
  if (client_list==NULL){
    perror("ERROR whois function");
    exit(EXIT_FAILURE);
  }
  char *message = malloc(MAX_LENGHT_MESSAGE);
  sprintf(message,"Connected since %s with IP address %s and port number %d\n",client_list->date,client_list->IP_address,client_list->port_number);
  do_send(socket_question,message,MAX_LENGHT_MESSAGE,0);
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

char *supp_last_caractere(char *chaine){
  int length = strlen(chaine);
  char *short_chaine = malloc(MAX_LENGHT_MESSAGE);
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
  // Initialisation of salon list (chained list)
  struct salon* salon_list = salon_list_init();
  int current_salon = 0;


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
              do_send(tab_fd[j].fd,hello,MAX_LENGHT_MESSAGE,0);
              client_list=add_client(client_list);
              client_list->pseudo = NULL;
              client_list->socket_number = tab_fd[j].fd;
              client_list->socket_fd = j;
              break;
            }
          }
          if (current_connection==NUMBER_OF_CONNECTION+1){
            int supp = sock_client;
            char *error = "Too many clients, connection failed. Come back later\n";
            do_send(supp,error,MAX_LENGHT_MESSAGE,0);
            do_close(supp);
            current_connection-=1;
          }
        }
        else {
          current_client=find_specific_client(client_list,tab_fd[i].fd);
          char *message = malloc(MAX_LENGHT_MESSAGE);
          do_recv(tab_fd[i].fd,message,MAX_LENGHT_MESSAGE,0);

          if (strncmp(message,"/nick ",6)==0){
            //printf("massage %s\n",message +6);
          //  char *message_bis = message + 6;
          //  if (check_pseudo(client_list, message_bis) == 1){
            //  printf("message %s\n",message_bis);
              //char *pseudo_already_exists = "pseudo already exists";
            //  printf("%s \n",pseudo_already_exists);
            //  do_send(tab_fd[i].fd,pseudo_already_exists,MAX_LENGHT_MESSAGE,0);
            //}
          //  else{
              if (current_client->pseudo==NULL){
                char *welcome = "Welcome on the chat !\n";
                do_send(tab_fd[i].fd,welcome,MAX_LENGHT_MESSAGE,0);
                char *realpseudo=message+6;
                current_client->pseudo=realpseudo;
              }
              else {
                char *realpseudo=message+6;
                current_client->pseudo=realpseudo;
                char *change = "Your nickname has been updated.\n";
                do_send(tab_fd[i].fd,change,MAX_LENGHT_MESSAGE,0);
              }
          //  }
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
            do_send(tab_fd[i].fd,last_message,MAX_LENGHT_MESSAGE,0);
            printf("Closing client n°%d connection.%d current connection(s).\n",i,current_connection);
            fflush(stdout);
            do_close(tab_fd[i].fd);
            tab_fd[i].fd=0;
          }

          else if ((strcmp(message,"/who\n") == 0)){
            char *currentco=malloc(MAX_LENGHT_MESSAGE);
            sprintf(currentco,"%d",current_connection);
            do_send(tab_fd[i].fd,currentco,MAX_LENGHT_MESSAGE,0);
            who(client_list,tab_fd[i].fd,current_connection);
          }

          else if ((strncmp(message,"/whois \n",7) == 0)){
            char *pseudowhois;
            pseudowhois=message+7;
            struct client *user;
            if (check_pseudo(client_list,pseudowhois)==0){
              char *error ="This user doesn't exist\n";
              do_send(tab_fd[i].fd,error,MAX_LENGHT_MESSAGE,0);
            }
            else {
              user=find_specific_client_pseudo(client_list,pseudowhois);
              whois(user,tab_fd[i].fd);
            }
          }

          else if ((strncmp(message,"/all \n",5) == 0)){
            char *message_all = message + 5;
            char *sender = current_client->pseudo;

            /*char* envoie = strcat(sender, "has told you : ");
            printf("envoie %s\n",envoie);

            printf("sender %s\n",sender);
            printf("message_all %s\n",message_all);*/

            for(int k=1;k<=current_connection;k++) {
              if (k != i){
                do_send(tab_fd[k].fd,message_all,MAX_LENGHT_MESSAGE,0);

              }
            }
          }

          else if ((strncmp(message,"/msg \n",5) == 0)){
            char *chaine_dest_et_msg = message + 5;

            int lenght_dest_pseudo = position_first_space(chaine_dest_et_msg);
            printf("lenght_dest_pseudo %d\n",lenght_dest_pseudo);
            char *dest_pseudo = malloc(MAX_LENGHT_MESSAGE);
            //char dest_pseudo[lenght_dest_pseudo];
            for (int i=0;i<lenght_dest_pseudo;i++){
              dest_pseudo[i] = chaine_dest_et_msg[i];
            }
            dest_pseudo[lenght_dest_pseudo] = '\n';

            printf("dest_pseudo %s\n",dest_pseudo);

            struct client *dest_client;
            if (check_pseudo(client_list,dest_pseudo)==0){
              char *error ="This user doesn't exist\n";
              do_send(tab_fd[i].fd,error,MAX_LENGHT_MESSAGE,0);
            }
            else {
              char *envoie = chaine_dest_et_msg + lenght_dest_pseudo;
              dest_client=find_specific_client_pseudo(client_list,dest_pseudo);
              do_send(dest_client->socket_number,envoie,MAX_LENGHT_MESSAGE,0);
            }

          }

         else if ((strncmp(message,"/create \n",8) == 0)){
           char* name = message + 8;
           printf("name %s\n",name);

           if((current_salon < 1) || (check_name(salon_list,name) == 0)){
             salon_list=add_salon(salon_list);
             current_salon ++;
             salon_list->name = name;
             printf("salon_list->name %s\n",salon_list->name);
             salon_list->client_salon = client_list_init(salon_list->client_salon);
             salon_list->client_salon = add_client(salon_list->client_salon);
             salon_list->client_salon->pseudo = current_client->pseudo;
             printf("salon_list->client_salon->pseudo %s\n",salon_list->client_salon->pseudo);
             printf("1er salon\n");
             char *creation=malloc(MAX_LENGHT_MESSAGE);
             char *short_name =  malloc(MAX_LENGHT_MESSAGE);
             short_name = supp_last_caractere(name);
             sprintf(creation," salon %s creates \n",short_name);
             printf("creation %s\n",creation);
             do_send(tab_fd[i].fd,creation,MAX_LENGHT_MESSAGE,0);

           }

           else{
             char *existence = "This name already exits, choose another one.\n";
             do_send(tab_fd[i].fd,existence,MAX_LENGHT_MESSAGE,0);
             printf("déjà utilisé\n");
        }
         }





          else {
            do_send(tab_fd[i].fd,message,MAX_LENGHT_MESSAGE,0);
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
