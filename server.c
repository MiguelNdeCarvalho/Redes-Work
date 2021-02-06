#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <time.h>        
#include <stdbool.h>

#include <ListaDB.c>

#define PORT 5555
#define BUFSIZE 512

typedef struct client{
	
	char cmd[485]; // 485 Byte; começa em 0 e termina em 484
	char nick_name[21]; //21 bytes; começa em 485 e termina em 506
	int channel; //4bytes; começa em 507 e termina em 511
	bool role;  //1 Byte; 512;

}client_t;



bool validCharacters(char *name) {
  
  int size = strlen(name); 

  for (int i = 0; i<size;i++) {
    //printf("i:%d\n",name[i]);
    if (!((name[i] >= 48 && name[i] <=  57) || (name[i] >= 65 && name[i] <= 122) || name[i]==10)) {
      return false;
    }
  }
  return true;
}

void print_client(client_t *n)
{
	printf("client_status( nick:%s role:%d channel:%d, cmd:%s)\n"
							,n->nick_name,n->role,n->channel,n->cmd);
}

client_t *bufferToClient(char buff[BUFSIZE])
{
  client_t *new_client = malloc(sizeof(client_t));;
  
  memcpy(&new_client->cmd, buff + 0, 485);
  memcpy(&new_client->nick_name, buff + 485, 21);
  memcpy(&new_client->channel, buff + 506, 4);
  memcpy(&new_client->role, buff + 511, 1);
  
  return new_client;
}

char *clientToBuffer(client_t *n)
{

  char *buf = malloc(BUFSIZE);
  bzero(buf, BUFSIZE);

  int k = 0;
  memcpy(buf + k, &n->cmd, sizeof(n->cmd));  k += sizeof(n->cmd);
  memcpy(buf + k, &n->nick_name, sizeof(n->nick_name));  k += sizeof(n->nick_name);
  memcpy(buf + k, &n->channel, sizeof(n->channel));  k += sizeof(n->channel);
  memcpy(&buf[511], &n->role, 1); 

  return buf;
} 


int main(int argc, char const *argv[]) 
{
  fd_set all_fds;   // master file descriptor list
  fd_set sel_fds;   // temp file descriptor list for select()
  int maxfd;        // maximum file descriptor number
  
  int server_fd, new_socket; 
  struct sockaddr_in address;
  
  int opt = 1;      // for setsockopt() SO_REUSEADDR, below
  int addrlen = sizeof(address); 
  char buffer[BUFSIZE];
  int bytes;

  //utilizadores registados
    int size_DB = 2;
    client_R_t server_DB[20];

    //admin
    client_R_t *admin = malloc(sizeof(client_R_t));
    strcpy(admin->nick_name,"admin\0");
    strcpy(admin->pass,"1234\0"); // the irony...
    admin->role = 2;
    admin->channel = 0;
    server_DB[0] = *admin;

    //user
    client_R_t *user = malloc(sizeof(client_R_t));
    strcpy(admin->nick_name,"joao\0");
    strcpy(admin->pass,"1234\0");
    admin->role = 1;
    admin->channel = 0;
    server_DB[1] = *user;

  //utilizadores ativos


  int server_channels[4] = {1,2,3,4};

  void insert_user()
  {

  }


  // Creating socket file descriptor 
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
    perror("socket failed"); 
    exit(EXIT_FAILURE); 
  } 
  
  // Forcefully attaching socket to the port 1300 
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                 &opt, sizeof(opt))) { 
    perror("setsockopt failed"); 
    exit(EXIT_FAILURE); 
  } 
  address.sin_family = AF_INET; 
  address.sin_addr.s_addr = INADDR_ANY; 
  address.sin_port = htons( PORT ); 
  
  // Bind the socket to the network address and port
  if (  bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0  ) { 
    perror("bind failed"); 
    exit(EXIT_FAILURE); 
  } 
  if (listen(server_fd, 3) < 0) { 
    perror("listen failed"); 
    exit(EXIT_FAILURE); 
  }

  FD_ZERO(&all_fds);            // clear the master and temp sets
  FD_SET(server_fd, &all_fds);  // add socket to set of fds

  maxfd = server_fd;            // for now, the max is this one
  
  while(1) {  // Server loop
    int i;
    
    sel_fds = all_fds; // copy set of fds (because select() changes it)
    
    // Wait for data in ONE or MORE sockets
    if ( select(maxfd+1, &sel_fds, NULL, NULL, NULL) == -1 ) {
      perror("select failed");
      exit(EXIT_FAILURE);
    }

    // If we got here, there's data somewhere...
    // Let's find where

    for(i=0; i<=maxfd; i++) {
      if (FD_ISSET(i, &sel_fds)) { // we got one!!!
        // Now if it's the main socket, a client is connecting
        // If not, a client must have sent data (or disconnected)

        if (i == server_fd) {
          // A connection is ready to be accept()ed
          if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                                   (socklen_t*)&addrlen))<0) { 
            perror("accept failed"); 
            exit(EXIT_FAILURE); 
          }

          printf("Client connected.\n");
          client_t *cliente = malloc(sizeof(client_t));
          strcpy(cliente->nick_name,"Anonimo\0");
          strcpy(cliente->cmd,"\0");
          cliente->role = 0;
          cliente->channel = 0;

          //print_client(cliente);
          memcpy(&buffer, clientToBuffer(cliente), 512);
          //cliente = bufferToClient(buffer);
          //print_client(cliente);

          size_t total = 0;
          int len = BUFSIZE;

          while ( total != len ) {
              ssize_t nb = send( new_socket, buffer + total, len - total, 0 );
              if ( nb == -1 ) printf( "send failed" );
              total += nb;
          }

          //printf("enviei %ldB\n",total);
          //free(cliente);
          //print_client(cliente);
          //sleep(1);

          // But now we have to monitor this socket too
          FD_SET(new_socket, &all_fds);

          // and update the maxfd, if necessary
          maxfd = new_socket > maxfd ? new_socket : maxfd;          
        }
        else {
          // here, we have data in one client, must recv() it
          bzero(buffer, BUFSIZE);

          //printf("----------á espera de data-----------\n");
          bytes = recv(i, buffer, BUFSIZE, 0);
          client_t *cliente_request = bufferToClient(buffer);
          printf("Client request:");
          print_client(cliente_request);


          if ( bytes == 0 ) { // client disconnected, too bad...
            printf("Client disconnected.\n");
            FD_CLR(i, &all_fds);
            // nevermind the maxfd...
          }
          else {

                client_t *server_response = malloc(sizeof(client_t));

                strcpy(server_response->nick_name,cliente_request->nick_name);
                server_response->role = cliente_request->role;
                server_response->channel = cliente_request->channel;


                if (!strncmp(cliente_request->cmd,"quit",4)){
                  
                  strcpy(cliente_request->cmd,"ok\0");
                  memcpy(&buffer, clientToBuffer(cliente_request), 512);
                  send(i, buffer, BUFSIZE, 0 );
                  close(i);
                  FD_CLR(i, &all_fds);  
                }
                else if (!strncmp(cliente_request->cmd,"MSSG",4))
                {


                  ssize_t size = strlen(cliente_request->cmd);
                  //printf("size: %ld\n",strlen(txt));

                  if (size>=481)
                  {
                    strcpy(cliente_request->cmd,"RPLY 103 - Erro. Mensagem demasiado longa.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                    free(server_response);
                  
                  }
                  else if (size<=6)
                  {
                    strcpy(cliente_request->cmd,"RPLY 102 - Erro. Não há texto na mensagem.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                    free(server_response);
                  }
                  else
                  {


                    strcpy(server_response->cmd,cliente_request->cmd);
                    //loop que envia o server_response a cada um dos sockets 


                    strcpy(cliente_request->cmd,"RPLY 101 - mensagem enviada com sucesso.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  


                }
                else if (!strncmp(cliente_request->cmd,"NICK",4))
                {

                    //txt
                    ssize_t size = strlen(cliente_request->cmd);
                    char txt[21];
                    memcpy( txt, &cliente_request->cmd,size);
                    txt[size] = '\0';
                    memcpy(txt, txt+5,size-5);
                    memcpy(&txt[size-5],"\0",1);
                    printf("txt: %s\n",txt);

                    //printf("inv: %d\n",invalidCharacters(txt));
                    if(strlen(txt)>=21)
                    {
                      strcpy(cliente_request->cmd,"RPLY 003 - Erro: Nome pedido não válido\0");
                      memcpy(&buffer, clientToBuffer(cliente_request), 512);
                      send(i, buffer, BUFSIZE, 0 );
                    }
                    else if(validCharacters(txt))
                    {
                      strcpy(cliente_request->cmd,"RPLY 003 - Erro: Nome pedido não válido\0");
                      memcpy(&buffer, clientToBuffer(cliente_request), 512);
                      send(i, buffer, BUFSIZE, 0 );
                    }
                    else
                    {
                      strcpy(cliente_request->cmd,"Nome atribuído com sucesso\0");
                      memcpy(&buffer, clientToBuffer(cliente_request), 512);
                      send(i, buffer, BUFSIZE, 0 );
                    }
                    
                    
      


                }
                else if (!strncmp(cliente_request->cmd,"LIST",4))
                {
                  strcpy(cliente_request->cmd,"LIST [1,2,3,4]\0");
                  memcpy(&buffer, clientToBuffer(cliente_request), 512);
                  send(i, buffer, BUFSIZE, 0 );

                }else if (!strncmp(cliente_request->cmd,"JOIN",4))
                {
                  ssize_t size = strlen(cliente_request->cmd);
                  char txt[21];
                  memcpy( txt, &cliente_request->cmd,size);
                  txt[size] = '\0';
                  memcpy(txt, txt+5,size-5);
                  memcpy(&txt[size-5],"\0",1);

                  cliente_request->channel = atoi(txt);
                  printf("n: %d\n",cliente_request->channel);

                  if (cliente_request->role == 0)
                  {
                    strcpy(cliente_request->cmd,"RPLY 303 - Erro. Não pode mudar para o canal\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  else if (cliente_request->channel < 0 && cliente_request->channel > 4)
                  {
                    strcpy(cliente_request->cmd,"RPLY 302 – Erro. canal não existente\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  else
                  {

                    //mudar na tabela de utilizadores ativos

                    //mensagem global MSSG "server :> entrou neste canal"
                    // ou MSSG "server :> deixou este canal"

                    strcpy(cliente_request->cmd,"RPLY 301 - Mudança de canal com sucesso\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  
        
                }
                else if (!strncmp(cliente_request->cmd,"PASS",4))
                {

                }
                else if (!strncmp(cliente_request->cmd,"WHOS",4))
                {

                }
                else if (!strncmp(cliente_request->cmd,"KICK",4))
                {

                }
                else if (!strncmp(cliente_request->cmd,"REGS",4))
                {

                }
                else if (!strncmp(cliente_request->cmd,"OPER",4))
                {

                }
                else if (!strncmp(cliente_request->cmd,"QUIT",4))
                {

                }
                else
                {
                  //strcpy(cliente->nick_name,"Anonimo\0");
                  strcpy(cliente_request->cmd,"Invalid CMD\0");
                  memcpy(&buffer, clientToBuffer(cliente_request), 512);
                  send(i, buffer, BUFSIZE, 0 );
                }
                free(server_response);
          }
        }
      }
    }
  }
  
  return 0; 
} 