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


//Adding a client
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

struct client *find_specific_client_pseudo(struct client *client_list, char *pseudo){
  if (client_list==NULL){
    perror("ERROR finding client");
    exit(EXIT_FAILURE);
  }
  struct client *tmp;
  tmp = client_list;

  while(tmp->next != NULL){
    if (strcmp(tmp->pseudo,pseudo)==0){
      printf("Je suis passée dans find specific cllient pseudo\n");
      return tmp;
    }
    tmp = tmp->next;
  }
  if (tmp->next == NULL && strcmp(tmp->pseudo,pseudo)==0){
      return tmp;
    }

}

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
    tmp = tmp->next;
  }
}

void whois(struct client *client_list, int socket_question){
  if (client_list==NULL){
    perror("ERROR whois function");
    exit(EXIT_FAILURE);
  }
  char *message = malloc(MAX_LENGHT_MESSAGE);
  sprintf(message,"Connected with IP address %s and port number %d\n",client_list->pseudo,client_list->IP_address,client_list->port_number);
  do_send(socket_question,message,MAX_LENGHT_MESSAGE,0);
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
             if (current_client->pseudo==NULL){


             char *welcome = "Welcome on the chat !\n";
             do_send(tab_fd[i].fd,welcome,MAX_LENGHT_MESSAGE,0);
              char *realpseudo=message+6; //on blègue le /nick pour le mettre dans la liste
              current_client->pseudo=realpseudo;
              printf("Le pseudo du client n°%d est : %s \n",i,current_client->pseudo);
            }
            else {
              char *realpseudo=message+6; //on blègue le /nick pour le mettre dans la liste
              current_client->pseudo=realpseudo;
              char *change = "Your nickname has been updated.\n";
              do_send(tab_fd[i].fd,change,MAX_LENGHT_MESSAGE,0);
            }
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

            else {
              //we write back to the client

              do_send(tab_fd[i].fd,message,MAX_LENGHT_MESSAGE,0);
              printf("The client n°%d has sent you : %s\n",i,message);
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
