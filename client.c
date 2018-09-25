#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<netinet/in.h>

#include<fcntl.h>


/*il faut rajouter l'option rebouting*/

int do_socket(){
  int sockfd;
  int yes=1;
  //create the socket
  sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  printf("%d\n",sockfd);
  //check for validity

  if (sockfd == -1)
  {
      perror("socket");
      exit(EXIT_FAILURE);
  }
  else
    return(sockfd);
}




void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
  int res = connect(sockfd,addr,addrlen);
  if (res !=0){
    fprintf(stderr,"Echec connexion\n");
      exit(EXIT_FAILURE);
  }
}

/*void readline(int socket){
  char *texte_msg;
  scanf("%s",&texte_msg);
  printf("%s\n",&texte_msg);*/
  /*char *texte;
  scanf("%s",&texte);
  int sent = 0;
  do{
    sent += write(socket,&texte + sent, sizeof(texte) - sent);
  }while (sent != sizeof(texte));
  int tens = 0;
  do{
    tens += read(STDIN_FILENO, socket + tens, sizeof(texte) - tens);
  }while (tens != sizeof(texte));
*/

char *readline(){
  char *message = malloc(sizeof(char)*40);
  printf("message à envoyer\n");
  fgets(message,40,stdin);
  printf("dans readline %s\n",message);
  return(message);
}



  //write(socket, &texte, sizeof(texte));
  //read()

int do_send(int sockfd, const void *msg, size_t len, int flags){
 int valeur_send;
 valeur_send = send(sockfd, msg, len, flags);
    //Vérification toutes les donnees ont ete envoyees
 if (valeur_send == -1){
   perror("send");
   exit(EXIT_FAILURE);
 }
 if (valeur_send != len){
 printf("tous les octets n'ont pas ete transmis");
 }
 return(valeur_send);
}

int do_recv(int sockfd, void *buf, int len, unsigned int flags){
  int valeur_recv=recv(sockfd,buf,len,flags);
  if (valeur_recv==-1){
    perror("ERROR reception failed");
    exit(EXIT_FAILURE);
  }
  if (valeur_recv != len){
    printf("The message has not been fully transmitted");
  }
  return (valeur_recv);
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
//sock_host.sin_addr.s_addr = htonl(INADDR_ANY);
inet_aton(argv[1], &sock_host.sin_addr);


//get_addr_info()


//get the socket
int s;
s = do_socket();
printf("s=%d\n",s);

//connect to remote socket
do_connect(s,(struct sockaddr *)&sock_host, sizeof(sock_host));     //a completer avec get @



//get user input


char *texte = malloc(sizeof(char)*40);
while ((strcmp(texte,"/quit\n") != 0)){
texte = readline();
printf("dans main %s\n",texte);


//send message to the server
int valeur_send;
valeur_send = do_send(s,texte,sizeof(texte),0); //mettre bonnes valeurs dans ()
printf("valeur send %d\n",valeur_send);

//handle_client_message()
char *message = malloc(sizeof(char)*40);
int valeur_recv = do_recv(s,message, sizeof(message),0);
printf("valeur recv %d\n",valeur_recv);
printf("le message du serveur est : %s",message);
}


    return 0;


}
