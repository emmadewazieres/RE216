#include "server-tools.h"

int main(int argc, char** argv){

  if (argc != 2){
    fprintf(stderr, "usage: RE216_SERVER port\n");
    return 1;
  }

  //Creation of the socket
  int socket_server;
  socket_server=do_socket();

  //Initialisation of the servor
  struct sockaddr_in saddr_in;
  saddr_in = init_serv_addr(atoi(argv[1]));

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
  for (int i=0;i<=NUMBER_OF_CONNECTION;i++){
    tab_fd[i].events=POLLIN;
  }

  //Listening
  do_listen(socket_server,SOMAXCONN);
  int current_connection=0;

  for (;;) { //endless loop

    //Polling
    do_poll(tab_fd,NUMBER_OF_CONNECTION+1);

    for(int i=0;i<=NUMBER_OF_CONNECTION+1;i++) {
      if (tab_fd[i].revents==POLLIN){
        if (tab_fd[0].revents==POLLIN){
          socklen_t taille = sizeof(saddr_in);
          socklen_t* addrlen = &taille;
          int sock_client = do_accept(socket_server,(struct sockaddr *)&saddr_in,addrlen);
          current_connection+=1;
          for (int j=1;j<=NUMBER_OF_CONNECTION+1;j++){
            if (tab_fd[j].fd==0){
              tab_fd[j].fd=sock_client;
              printf("Connecting with client n°%d. %d current connection(s).\n",j,current_connection);
              fflush(stdout);
              do_send(tab_fd[j].fd,"hello");
              client_list=add_client(client_list,NULL,tab_fd[j].fd);
              break;
            }
          }
          if (current_connection==NUMBER_OF_CONNECTION+1){
            int supp = sock_client;
            do_send(supp,"Too many clients, connection failed. Come back later.\n");
            do_close(supp);
            current_connection-=1;
          }
        }

        else {
          current_client=find_specific_client(client_list,tab_fd[i].fd);
          char *message = malloc(MAX_LENGTH_MESSAGE);
          do_recv(tab_fd[i].fd,message);

          if (strncmp(message,"/nick ",6)==0){
            nick(message,current_connection,current_client,client_list);
          }

          else if (strncmp(message,"/IP ",4)==0){
            char *IP = message + 4;
            current_client->IP_address=IP;
          }

          else if (strncmp(message,"/port ",6)==0){
            char *port = message + 6;
            int port_n = atoi(port);
            current_client->port_number=port_n;
          }

          else if (strncmp(message,"/date ",6)==0){
            char *date = message + 6;
            current_client->date=date;
          }

          else if ((strcmp(message,"/quit\n") == 0)){
            if(current_client->nbe_salon_in != 0){
              char *pb_quit=malloc(MAX_LENGTH_MESSAGE);
              sprintf(pb_quit,"You are in %d salon(s). Please leave it/them before quitting.You can use the command '/mysalon' if you don't remember which salon(s) you are in.\n",current_client->nbe_salon_in);
              do_send(tab_fd[i].fd,pb_quit);
            }
            else{
              client_list = delete_client(client_list,tab_fd[i].fd);
              current_connection-=1;
              do_send(tab_fd[i].fd,"Closing connection.\n");
              printf("Closing client n°%d connection.%d current connection(s).\n",i,current_connection);
              do_close(tab_fd[i].fd);
              tab_fd[i].fd=0;
            }
          }

          else if ((strcmp(message,"/who\n") == 0)){
            who(client_list,tab_fd[i].fd);
          }

          else if ((strncmp(message,"/whois \n",7) == 0)){
            whois(client_list,message,current_client);
          }

          else if ((strncmp(message,"/all \n",5) == 0)){
            if(current_connection == 1){
              do_send(tab_fd[i].fd,"The server has told you : You are the only one on the chat, you can't send a broadcast message. Wait until someone logs in.\n");
            }
            else {
              char *message_all = message + 5;
              int length = strlen(current_client->pseudo);
              char *sender = malloc(MAX_LENGTH_MESSAGE);
              strncpy(sender, current_client->pseudo,length-1);
              char *all=malloc(MAX_LENGTH_MESSAGE);
              sprintf(all,"[%s to everyone] : %s",sender,message_all);
              for(int k=1;k<=current_connection;k++) {
                if (k != i){
                  do_send(tab_fd[k].fd,all);
                }
              }
            }
          }

          else if ((strncmp(message,"/msg \n",5) == 0)){
            msg(message,client_list,current_client);
          }

          else if ((strncmp(message,"/create \n",8) == 0)){
            char* name = message + 8;
            if((current_salon < 1) || (check_name(salon_list,name) == 0)){
              salon_list=add_salon(salon_list, name,current_client->pseudo,current_client->socket_number);
              current_salon++;
              current_client->nbe_salon_in++;
              char *creation=malloc(MAX_LENGTH_MESSAGE);
              char *short_name =  malloc(MAX_LENGTH_MESSAGE);
              short_name = supp_last_caractere(name);
              sprintf(creation,"You have created salon %s\n",short_name);
              do_send(tab_fd[i].fd,creation);
            }
            else{
              do_send(tab_fd[i].fd,"This name already exits, choose another one.\n");
            }
          }

          else if ((strncmp(message,"/join \n",6) == 0)){
            join(message,salon_list,current_client);
          }

          else if ((strncmp(message,"/which\n",7) == 0)){
            which(salon_list,tab_fd[i].fd);
          }

          else if ((strncmp(message,"/whoisin\n",8) == 0)){
            whoisin(salon_list,current_client->socket_number,message,current_salon);
          }

          else if ((strncmp(message,"/mysalon\n",8) == 0)){
            mysalon(salon_list,current_client);
          }

          else if (strncmp(message,"/leave ",7)==0){
            char *name_salon=message +7;
            if (check_name(salon_list,name_salon)==1){
              struct salon* salon_to_update = find_specific_salon_name(salon_list,name_salon);
              if (check_pseudo(salon_to_update->client_salon,current_client->pseudo)==1){
                salon_to_update->client_salon=delete_client(salon_to_update->client_salon,tab_fd[i].fd);
                salon_to_update->number_client--;
                current_client->nbe_salon_in--;
                char *leave = malloc(MAX_LENGTH_MESSAGE);
                char *name=supp_last_caractere(name_salon);
                sprintf(leave,"You left salon %s.\n",name);
                do_send(tab_fd[i].fd,leave);
              }
              else {
                do_send(tab_fd[i].fd,"You are not in this salon, you can't leave it.\n");
              }
              if (salon_to_update->number_client==0){
                salon_list = delete_salon(salon_list,name_salon);
                current_salon --;
              }
            }
            else {
              do_send(tab_fd[i].fd,"This salon doesn't exist.\n");
            }
          }

          else if (strncmp(message,"/salon ",7)==0){
            char *name_salon = message + 7;
            int length_name = position_first_space(name_salon);
            char *dest_name = malloc(MAX_LENGTH_MESSAGE);
            strncpy(dest_name,name_salon,length_name);
            dest_name[length_name] = '\n';
            char *message_salon = name_salon + length_name + 1;
            struct salon *dest_salon;
            if (check_name(salon_list,dest_name)==0){
              do_send(tab_fd[i].fd,"This salon doesn't exist\n");
            }
            else {
              int length = strlen(current_client->pseudo);
              char *sender = malloc(MAX_LENGTH_MESSAGE);
              strncpy(sender, current_client->pseudo,length-1);
              char *group=malloc(MAX_LENGTH_MESSAGE);
              sprintf(group,"[%s in salon] : %s",sender,message_salon);
              dest_salon = find_specific_salon_name(salon_list, dest_name);
              if(check_pseudo(dest_salon->client_salon, current_client->pseudo) == 0){
                do_send(tab_fd[i].fd,"You are not in this salon.\n");
              }
              else if (dest_salon->number_client == 1){
                do_send(tab_fd[i].fd,"You are alone in this salon.\n");
              }
              else{
                for(int k=1;k<=dest_salon->number_client;k++) {
                  if (k != i){
                    do_send(tab_fd[k].fd,group);
                  }
                }
              }
            }
          }

          else if (strncmp(message,"/send ",6)==0){
              char *destinataire_and_file = message + 6;
              int length_destinataire = position_first_space(destinataire_and_file);
              char *dest_name = malloc(MAX_LENGTH_MESSAGE);
              strncpy(dest_name,destinataire_and_file,length_destinataire);
              dest_name[length_destinataire] = '\n';
              char *fichier = destinataire_and_file + length_destinataire;
              if(check_pseudo(client_list, dest_name) == 0){
                char *warning = "Warning! This user doesn't exist, you can't contact him.\n";
                do_send(tab_fd[i].fd,warning);
              }
              else if(strcmp(current_client->pseudo, dest_name) == 0){
                char *warning = "Warning! You can't send a file to yourself.\n";
                do_send(tab_fd[i].fd,warning);
              }
              else{
                struct client *client_to_send;
                client_to_send = find_specific_client_pseudo(client_list, dest_name);
                char *pseudo = supp_last_caractere(current_client->pseudo);
                char *send = malloc(MAX_LENGTH_MESSAGE);
                sprintf(send,"/send %s wants to send you a file. Do you accept ?\n",pseudo);
                do_send(client_to_send->socket_number,send);
                char *reponse = malloc(MAX_LENGTH_MESSAGE);
                do_recv(client_to_send->socket_number,reponse);
                if(strcmp(reponse, "yes\n") == 0){
                  do_send(tab_fd[i].fd,"Your request was accepted.\n");
                  char *message_port = malloc(MAX_LENGTH_MESSAGE);
                  do_recv(tab_fd[i].fd,message_port);
                  do_send(client_to_send->socket_number,message_port);
                }
                else{
                  do_send(tab_fd[i].fd,"Your request was not accepted, the file has not been sent.\n");
                }
              }
            }

          else {
            char *send = malloc(MAX_LENGTH_MESSAGE);
            sprintf(send,"[server] : %s",message);
            do_send(tab_fd[i].fd, send);
            fflush(stdout);
          }
          //free(message);
        }
      }
    }
  }
  //clean up server socket
  do_close(socket_server);
  return 0;
}
