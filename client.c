#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include<netinet/in.h>

#include<fcntl.h>


//il faut rajouter l'option rebouting

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



//Je comprends rien à cette fonction !!!!
/*void get_addr_info(const char * address, const char* port, struct addrinfo** res){
  int status;
  struct addrinfo hints, p;

  memset(&hints,0,sizeof(hints));
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_STREAM;

  status= getaddrinfo(address, port, hints,res);

  if (status != 0){  //getaddrinfo renvoie 0 si reussit, sinon differents codes d'erreur
  perror("socket");
  exit(EXIT_FAILURE);
  }

p =

}*/

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

/*
  struct sockaddr_in sin;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_family = AF_INET;
  sin.sin_port = 0;
}
*/
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
texte = readline();
printf("dans main %s\n",texte);


//send message to the server
int valeur_send;
valeur_send = do_send(s,texte,sizeof(texte),0); //mettre bonnes valeurs dans ()
printf("valeur send %d\n",valeur_send);
//handle_client_message()


    return 0;


}
