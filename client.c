#include "client-tools.h"

int main(int argc,char** argv){

  if (argc != 3){
    fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
    return 1;
  }

  //get address info from the server
  struct sockaddr_in sock_host;
  memset(&sock_host,'\0',sizeof(sock_host));
  sock_host.sin_family = AF_INET;
  sock_host.sin_port = htons(atoi(argv[2]));
  inet_aton(argv[1], &sock_host.sin_addr);

  //Creation of the socket
  int socket = do_socket();

  //Connection to the server
  do_connect(socket,(struct sockaddr *)&sock_host, sizeof(sock_host));

  //Creation of the pollfd structure
  struct pollfd tab_fd[2];
  memset(tab_fd,0,sizeof(tab_fd));

  //Initialisation of the structure
  tab_fd[0].fd=socket;
  tab_fd[0].events=POLLIN;
  tab_fd[1].fd=0;
  tab_fd[1].events=POLLIN;

  //Dealing with the pseudo of the user
  char *text = malloc(MAX_LENGTH_MESSAGE);
  char *message = malloc(MAX_LENGTH_MESSAGE);
  do_recv(socket,message);

  if (strcmp(message,"Too many clients, connection failed. Come back later.\n")==0){
    printf("The server has told you : %s",message);
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

  //Once the user has chosen a correct pseudo, his information are sent to the RE216_SERVER
  char *name_IP = malloc(MAX_LENGTH_MESSAGE);
  gethostname(name_IP,MAX_LENGTH_MESSAGE);
  info_server(name_IP,sock_host, socket);

  for (;;) { //the program runs as long as the user doesn't quit

  //Polling
  do_poll(tab_fd,2);

  if (tab_fd[0].revents==POLLIN){
    //Receiving messages from other users or from the server
    do_recv(socket,message);

    if(strncmp(message,"/send",5) == 0){
      printf("%s",message + 5);
      char *answer = readline();
      do_send(socket,answer);
      if(strcmp(answer, "yes\n") == 0){
        char *new_port = malloc(MAX_LENGTH_MESSAGE);
        do_recv(socket,new_port);
        struct sockaddr_in sock_host_new;
        memset(&sock_host_new,'\0',sizeof(sock_host_new));
        sock_host_new.sin_family = AF_INET;
        sock_host_new.sin_port = htons(atoi(new_port));
        inet_aton(argv[1], &sock_host_new.sin_addr);

        //Creation of the socket
        int socket_new = do_socket();

        //Connection to the server
        do_connect(socket_new,(struct sockaddr *)&sock_host_new, sizeof(sock_host_new));

        //Receiving the file
        char *file = malloc(MAX_LENGTH_MESSAGE);
        do_recv(socket_new,file);

        char *buffer = malloc(MAX_LENGTH_MESSAGE);
        do_recv(socket_new,buffer);
        int length = strlen(buffer);

        char *where = malloc(MAX_LENGTH_MESSAGE);
        sprintf(where,"/home/emma/Documents/RE216/src/jalon01/torecv/%s",file);
        int fd = open(where,O_CREAT|O_RDWR, 0777);
        int writing = write(fd,buffer,sizeof(char)*length);
        if(writing == -1){
          printf("The file has not been received.\n");
        }
        else{
          printf("The file has been received.\n");
        }
        //Cleaning up the socket
        do_close(socket_new);
      }
    }
    else {
      printf("%s",message);
      fflush(stdout);
    }
  }

  if (tab_fd[1].revents==POLLIN){
    //Sending messages
    text = readline();

    if (strcmp(text,"/quit\n") == 0){
      do_send(socket,text);
      do_recv(socket,message);
      if (strcmp(message,"Closing connection.\n")==0){
        printf("%s",message);
        free(text);free(message);
        return(0);
      }
    }

    else if (strncmp(text,"/send ",6)==0){
      //Checking if the file exists and can be opened
      char *dest_and_file = text + 6;
      int length_dest = position_first_space(dest_and_file);
      char *dest_name = malloc(MAX_LENGTH_MESSAGE);
      strncpy(dest_name,dest_and_file,length_dest);
      dest_name[length_dest] = '\n';

      char *file = dest_and_file + length_dest +1;
      file = supp_last_caractere(file);
      char *where = malloc(MAX_LENGTH_MESSAGE);
      sprintf(where,"/home/emma/Documents/RE216/src/jalon01/tosend/%s",file);

      int fd = open(where,O_RDWR);
      if(fd == -1){
        printf("The file can't be opened.\n");
      }
      else {
        do_send(socket,text);
        char *answer = malloc(MAX_LENGTH_MESSAGE);
        do_recv(socket,answer);
        if(strcmp(answer, "Your request was accepted.\n") == 0){
          //Creation of the socket
          int socket_server;
          socket_server=do_socket();
          int random = rand_a_b(8200,16000);
          char *port = malloc(MAX_LENGTH_MESSAGE);
          sprintf(port,"%d",random);
          do_send(socket,port);

          //Initialisation of the servor
          struct sockaddr_in saddr_in;
          saddr_in = init_serv_addr(random);

          //Binding
          do_bind(socket_server,(struct sockaddr *)&saddr_in,sizeof(saddr_in));
          //Listening
          do_listen(socket_server,SOMAXCONN);

          //Accepting
          socklen_t taille = sizeof(saddr_in);
          socklen_t* addrlen = &taille;
          int sock_client_new = do_accept(socket_server,(struct sockaddr *)&saddr_in,addrlen);

          //Sending the file
          char *buffer = malloc(MAX_LENGTH_MESSAGE);
          do_send(sock_client_new,file);
          int lecture = read(fd,buffer,MAX_LENGTH_MESSAGE);
          do_send(sock_client_new,buffer);
          printf("The file has been sent.\n");
          //Cleaning up sockets
          do_close(sock_client_new);
          do_close(socket_server);
        }
        else{
          printf("%s",answer);
        }
      }
    }
    else {
      do_send(socket,text);
    }

  }
}
}
