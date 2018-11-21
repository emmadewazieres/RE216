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

#define MAX_LENGTH_MESSAGE 1000

//Generation nombre aléatoire entre a et b
int rand_a_b(int a,int b){
    srand(time(0));
    return rand()%(b-a)+a;
}

//Creation of the socket
int do_socket(){
  int sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if (sockfd == -1)
  {
      perror("ERROR creation failed");
      exit(EXIT_FAILURE);
  }
  else
    return(sockfd);
}


//Initialisation of the server
struct sockaddr_in init_serv_addr(int port){
  //int portno;
  struct sockaddr_in saddr_in;
  memset(&saddr_in,'\0', sizeof(saddr_in));
  //portno = atoi(port)+1;
  saddr_in.sin_port = htons(port);

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
    //exit(EXIT_FAILURE);
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


//Connection with the server
void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
  int connection = connect(sockfd,addr,addrlen);
  if (connection !=0){
    perror("ERROR connection failed");
    exit(EXIT_FAILURE);
  }
}

//Closing
void do_close(int sockfd){
  int closing = close(sockfd);
  if (closing == -1){
    perror("ERROR closing failed");
    exit(EXIT_FAILURE);
  }
}

//Get what the client has to say
char *readline(){
  char *message = malloc(MAX_LENGTH_MESSAGE);
  fgets(message,MAX_LENGTH_MESSAGE,stdin);
  return(message);
}

//Sending
void do_send(int sockfd, const void *text){
 int sending = send(sockfd, text, MAX_LENGTH_MESSAGE, 0);
 if (sending == -1){
   perror("ERROR sending failed");
   exit(EXIT_FAILURE);
 }
 if (sending != MAX_LENGTH_MESSAGE){
 printf("The message has not been fully transmitted \n");
 fflush(stdout);
 }
}

//Reception
void do_recv(int sockfd, void *message){
  int reception=recv(sockfd,message,MAX_LENGTH_MESSAGE,0);
  if (reception==-1){
    perror("ERROR reception failed");
    exit(EXIT_FAILURE);
  }
  if (reception != MAX_LENGTH_MESSAGE){
    printf("The message has not been fully transmitted \n");
    fflush(stdout);
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

char *supp_last_caractere(char *chaine){
  int length = strlen(chaine);
  char *short_chaine = malloc(MAX_LENGTH_MESSAGE);
  strncpy(short_chaine,chaine,length-1);
  return(short_chaine);
}


int main(int argc,char** argv)
{

    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    //get address info from the server
    struct sockaddr_in sock_host;
    memset(&sock_host,'\0',sizeof(sock_host));
    sock_host.sin_family = AF_INET;
    sock_host.sin_port = htons(atoi(argv[2]));
    inet_aton(argv[1], &sock_host.sin_addr);

    printf("sock_host.sin_port %d\n",sock_host.sin_port);



    //Creation of the socket
    int socket = do_socket();
    printf("socket %d\n",socket);

    //Connection to the server
    do_connect(socket,(struct sockaddr *)&sock_host, sizeof(sock_host));

    // Sending information at the RE216_SERVER
      // IP

    char *name_IP = malloc(MAX_LENGTH_MESSAGE);
    gethostname(name_IP,MAX_LENGTH_MESSAGE);
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

    //Chatting with the server
    char *text = malloc(MAX_LENGTH_MESSAGE);
    char *message = malloc(MAX_LENGTH_MESSAGE);

    do_recv(socket,message);
    printf("The server has told you : %s",message);

    if (strcmp(message,"Too many clients, connection failed. Come back later\n")==0){
      return 0;
    }

    while (strncmp(text,"/nick ",6)!=0)  {
      printf("Please logon with /nick <your pseudo>\n");
      text=readline();
      if (strcmp(text,"/quit\n") == 0){

        do_send(socket,text);
        do_recv(socket,message);
        printf("The server has told you : %s",message);
        return(0);
      }
    }

    do_send(socket,text);
    do_recv(socket,message);
    printf("The server has told you : %s",message);


    while (strcmp(message,"This pseudo already exits, choose another one.\n")==0){
      printf("Please logon with /nick <your pseudo>\n");
      text=readline();
      if (strcmp(text,"/quit\n") == 0){

        do_send(socket,text);
        do_recv(socket,message);
        printf("The server has told you : %s",message);
        return(0);
      }
      do_send(socket,text);
      do_recv(socket,message);
      printf("The server has told you : %s",message);

    }

    //Creation of the pollfd structure
    struct pollfd tab_fd[2];
    memset(tab_fd,0,sizeof(tab_fd));

    //Initialisation of the structure
    tab_fd[0].fd=socket;
    tab_fd[0].events=POLLIN;
    tab_fd[1].fd=0;
    tab_fd[1].events=POLLIN;

    for (;;) { //endless loop

      //Polling
      do_poll(tab_fd);

      if (tab_fd[0].revents==POLLIN){
        do_recv(socket,message);
        printf("%s",message);
        fflush(stdout);


        if(strncmp(message,"/send",5) == 0){
          printf("j ai recu send du grand serveur\n");
          printf("%s",message + 5);
          char *reponse = readline();
          do_send(socket,reponse);
          char *yes = "yes\n";
          if(strcmp(reponse, yes) == 0){

          char *new_port = malloc(MAX_LENGTH_MESSAGE);
          do_recv(socket,new_port);
          printf("new_port %s\n",new_port);
          int port = atoi(new_port);
          printf("jfnnk %d\n",port);


          //get address info from the server
          struct sockaddr_in sock_host_new;
          memset(&sock_host_new,'\0',sizeof(sock_host_new));
          sock_host_new.sin_family = AF_INET;
          sock_host_new.sin_port = htons(port);
          inet_aton(argv[1], &sock_host_new.sin_addr);

          printf("sock_host_new.sin_port %d\n",sock_host_new.sin_port);

          //Creation of the socket
          int socket_new = do_socket();
          printf("socket_new %d\n",socket_new);

          //Connection to the server
          do_connect(socket_new,(struct sockaddr *)&sock_host_new, sizeof(sock_host_new));
          printf("socket_new /send\n");
          char *test = malloc(MAX_LENGTH_MESSAGE);
          do_recv(socket_new,test);
          printf("test %s\n",test);

          do_close(socket_new);
        }

        }
      }

      if (tab_fd[1].revents==POLLIN){


    //while (strcmp(text,"/quit\n") != 0){
        //Getting user input
        text = readline();
        do_send(socket,text);

        if (strcmp(text,"/quit\n") == 0){
          //do_send(socket,text);
          do_recv(socket,message);
          if (strncmp(message,"/ok_to_quit",11)==0){
          printf("The server has told you : %s",message);
          return(0);
        }
          else{
          printf("The server has told you : %s",message);
          }
        }
        if (strcmp(text,"/who\n")==0){
          do_recv(socket,message);
          printf("Online users are :\n%s",message);
        }
        else if (strcmp(text,"/which\n")==0){
          do_recv(socket,message);
          printf("Salons are :\n%s",message);
        }
        else if (strncmp(text,"/whoisin ",9)==0){
          char *name_salon = text +9;
          char *name = supp_last_caractere(name_salon);
          do_recv(socket, message);
          printf("Users in %s are :\n%s",name,message);
        }

        else if (strncmp(text,"/send ",6)==0){

          char *reponse = malloc(MAX_LENGTH_MESSAGE);
          do_recv(socket,reponse);

          char *yes = "yes\n";
          if(strncmp(reponse, yes,3) == 0){

          printf("tu joues le role de serveur\n");


          //if(strncmp(warning_msg, dest_name) == 0){


            //Creation of the socket
            int socket_server;
            socket_server=do_socket();
            printf("socket_server %d\n",socket_server);

            int nbe_aleatoire = rand_a_b(8200,16000);
            printf("nbe_aleatoire = %d\n",nbe_aleatoire);
            char *aleatoire = malloc(MAX_LENGTH_MESSAGE);
            sprintf(aleatoire,"%d",nbe_aleatoire);
            printf("aleatoire %s\n",aleatoire);
            do_send(socket,aleatoire);

            //Initialisation of the servor
            struct sockaddr_in saddr_in;
            saddr_in = init_serv_addr(nbe_aleatoire);



            //Binding
            printf("avant bind \n");

            printf("htons(8080) %d \n",htons(8080));
            printf("htons(8080) %d \n",htons(8080));
            printf("htons(0) %d \n",htons(0));



            do_bind(socket_server,(struct sockaddr *)&saddr_in,sizeof(saddr_in));
            printf("apres bind \n");
            //Listening
            do_listen(socket_server,SOMAXCONN);
            printf("apres listen \n");

            socklen_t taille = sizeof(saddr_in);
            socklen_t* addrlen = &taille;
            int sock_client_new = do_accept(socket_server,(struct sockaddr *)&saddr_in,addrlen);
            printf("apres accept \n");

            //int descript_fichier = open
            char *test = "fonctionne stp\n";
            do_send(sock_client_new,test);


            do_close(sock_client_new);
            do_close(socket_server);
            //close 2 socket
          }

          else{
            printf("refuse de recevoir le fichier");
            printf("%s\n",reponse);
          }

        }
        /*else {
          do_recv(socket,message);
          printf("%s",message);
        }*/


}
}
    return 0;
}
