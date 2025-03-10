#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <time.h>        
#include <stdbool.h>

#include "ListaClient.h"
#include "ListaUser.h"

#define PORT 5555
#define BUFSIZE 512

typedef struct client{
	
	char cmd[483]; // 482 Byte; começa em 0 e termina em 482
	char nick_name[21]; //21 bytes; começa em 483 e termina em 503
	int channel; //4 bytes; começa em 504 e termina em 507
	int role;  //4 Byte; 508-511;

}client_t;

bool validCharacters(char *name) {
  
  int size = strlen(name); 

  for (int i = 0; i<size;i++) {
    //printf("i:%d\n",name[i]);
    if (!((name[i] >= 48 && name[i] <=  57) || (name[i] >= 65 && name[i] <=  90) || (name[i] >= 97 && name[i] <=  122))) {
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

void print_User(user_t *n)
{
	printf("client_user( nick:%s ,role:%d ,channel:%d, auth: %d, sock:%d)\n"
							,n->nick_name,n->role,n->channel,n->auth,n->sock);
}

void print_ClientR(client_R_t *n)
{
	printf("client_ClientR( nick:%s role:%d pass:%s)\n"
							,n->nick_name,n->role,n->pass);
}


client_t *bufferToClient(char buff[BUFSIZE])
{
  client_t *new_client = malloc(sizeof(client_t));;
  
  memcpy(&new_client->cmd, buff + 0, 482);
  memcpy(&new_client->nick_name, buff + 483, 21);
  memcpy(&new_client->channel, buff + 504, 4);
  memcpy(&new_client->role, buff + 508, 4);
  
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
  memcpy(&buf[508], &n->role,4); 

  return buf;
} 

/*
bool valideUser()
{

}
*/

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
    listClient_R_t *database = newListClient();
    
    //admin
    client_R_t *admin = malloc(sizeof(client_R_t));
    strcpy(admin->nick_name,"admin\0");
    strcpy(admin->pass,"1234\0"); // the irony...
    admin->role = 2;
    listClient_insert(database,admin);

    //user
    client_R_t *user = malloc(sizeof(client_R_t));
    strcpy(user->nick_name,"joao\0");
    strcpy(user->pass,"1234\0");
    user->role = 1;
    listClient_insert(database,user);

    printf("Database: ");
    listClient_print(database);
    
    //utilizadores ativos
    listUser_t *active_users = newListUser();
    printf("Online Useres: NONE\n");
    listUser_print(active_users);

    int n_anonimo = 0;

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
  if (listen(server_fd, 5) < 0) { 
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
          
          char n[21];
          sprintf(n, "%d", n_anonimo);
          n_anonimo++;

          char name[21] = "";
          strcat(name,"Anonimo_");
          strcat(name,n);

          
          client_t *cliente = malloc(sizeof(client_t));
          strcpy(cliente->nick_name,name);
          strcpy(cliente->cmd,"MSSG server :> novo utilizador - Anonimo_\0");
          strcat(cliente->cmd,n);
          cliente->role = 0;
          cliente->channel = 0;
          
          user_t *new_user = newUser();
          new_user->sock = new_socket;
          strcpy(new_user->nick_name,name);
          new_user->role = 0;
          new_user->channel = 0;     
          new_user->auth = 0;
          
          printf("new_socket:%d\n",new_user->sock);
          
          //MSSG "server :> novo utilizador "se for um utilizador novo, ou

          //enviar a todos
          nodeUser_t *current = active_users->header->next;
          listUser_print(active_users);

          while (current!=NULL)
          {
            
            if (current->user.sock!=i  && current->user.channel==new_user->channel)
            {
              user_t *send_msg = newUser();
              strcpy(send_msg->nick_name,current->user.nick_name);
              send_msg->channel = current->user.channel;
              send_msg->role = current->user.role;
              //printf("sock:%d | i:%d\n",current->user.sock,i);

              memcpy(&buffer, clientToBuffer(cliente), 512);
              send(current->user.sock, buffer, BUFSIZE, 0 );
              free(send_msg);
            }
            current = current->next;

          }

          print_client(cliente);
          listUser_insert(active_users,new_user);
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

          bytes = recv(i, buffer, BUFSIZE, 0);
          client_t *cliente_request = bufferToClient(buffer);
          printf("Online users: ");
          listUser_print(active_users);
          printf("Client request: ");
          print_client(cliente_request);

          //valideUser();

          if ( bytes == 0 ) { // client disconnected, too bad...
            printf("\nClient disconnected.\n");
            FD_CLR(i, &all_fds);
            // nevermind the maxfd...
          }
          else {
                printf("----------------------------------------\n");

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

                    listUser_print(active_users);


                  //verificar se o nome é usado
                  user_t *new = newUser();
                  strcpy(new->nick_name,cliente_request->nick_name);
                  new = listUser_find(active_users,new,i);

                  printf("new: %s %d\n",new->nick_name,new->auth);
                  print_User(new);

                  ssize_t size = strlen(cliente_request->cmd);
                  //printf("size: %ld\n",strlen(txt));

                  if (!strncmp(cliente_request->nick_name,"Anonimo",7))
                  {
                    strcpy(cliente_request->cmd,"MSSG Insira o nome primeiro\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );

                  }
                  else if (size>=481)
                  {
                    strcpy(cliente_request->cmd,"RPLY 103 - Erro. Mensagem demasiado longa.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  
                  }
                  else if (size<=6)
                  {
                    strcpy(cliente_request->cmd,"RPLY 102 - Erro. Não há texto na mensagem.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                    free(server_response);
                  }
                  else if (new->auth==0)
                  {
                    strcpy(cliente_request->cmd,"MSSG Autenticação necessária\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  else
                  {
                    ssize_t size = strlen(cliente_request->cmd);
                    char txt[21];
                    memcpy( txt, &cliente_request->cmd,size);
                    txt[size] = '\0';
                    memcpy(txt, txt+5,size-5);
                    memcpy(&txt[size-5],"\0",1);
                    //printf("txt:%s n:%ld\n",txt,strlen(txt));
 
                    strcpy(server_response->cmd,cliente_request->cmd);

                    //criar a mssg
                    char msg[485] = "";
                    strcat(msg,"MSSG ");
                    strcat(msg,cliente_request->nick_name);
                    strcat(msg," :> ");
                    strcat(msg,txt);
                    strcpy(server_response->cmd,msg);
                   

                    //print_client(server_response);
                    printf("oi\n");
                    //enviar a todos
                    nodeUser_t *current = active_users->header->next;
                    listUser_print(active_users);

                    while (current!=NULL)
                    {
                      
                      if (strcmp(current->user.nick_name,cliente_request->nick_name)!=0 && current->user.channel==cliente_request->channel)
                      {
                        //printf("current: %s\n",current->user.nick_name);
                        strcpy(server_response->nick_name,current->user.nick_name);
                        server_response->channel = current->user.channel;
                        server_response->role = current->user.role;
                        //printf("sock:%d | i:%d\n",current->user.sock,i);
  
                        memcpy(&buffer, clientToBuffer(server_response), 512);
                        send(current->user.sock, buffer, BUFSIZE, 0 );
                      }
                      current = current->next;

                    }
                    

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
                  printf("txt:%s n:%ld\n",txt,strlen(txt));

                  //printf("inv: %d\n",validCharacters(txt));
                  
                  if(strlen(txt)==0)
                  {
                    strcpy(cliente_request->cmd,"RPLY 002 - Erro: Falta introdução do nome.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  else if(strlen(txt)>=21)
                  {
                    strcpy(cliente_request->cmd,"RPLY 003 - Erro: Nome pedido não válido\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  else if(!validCharacters(txt))
                  {
                    strcpy(cliente_request->cmd,"RPLY 003 - Erro: Nome pedido não válido\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  else
                  {


                    user_t *new1 = newUser();
                    strcpy(new1->nick_name,txt);
                    //printf("find i:%d\n",i);
                    new1 = listUser_find_name(active_users,new1);
                    

                    

                    listUser_print(active_users);
                    //printf("new:%s new1:%s request:%s\n",new->nick_name,new1->nick_name,txt);

                    if (new1==NULL)
                    {
                      
                      //mudar na tabela de utilizadores ativos
                      user_t *new_user = newUser();
                      strcpy(new_user->nick_name,cliente_request->nick_name);
                      new_user = listUser_remove(active_users,new_user,i);
                      //print_user(new_user);
                      new_user->sock = i;
                      new_user->role = cliente_request->role;
                      new_user->channel = cliente_request->channel;
                      strcpy(new_user->nick_name,txt);

                      listUser_insert(active_users,new_user);

                      listUser_print(active_users);

                      //MSSG "server :> novo utilizador "se for um utilizador novo, ou
                      
                      //MSSG "server :> <nome_antigo> mudou o seu nome para " se já tinha nome atribuído.
                      //criar a mssg
                      char msg[481] = "";
                      strcat(msg,"MSSG server :> ");
                      strcat(msg,cliente_request->nick_name);
                      strcat(msg," mudou o seu nome para ");
                      strcat(msg,txt);
                      strcpy(server_response->cmd,msg);

                      //enviar a todos
                      nodeUser_t *current = active_users->header->next;
                      listUser_print(active_users);

                      while (current!=NULL)
                      {
                        
                        //printf("%s %s %d %d\n",current->user.nick_name,txt,current->user.channel,cliente_request->channel);
                        if (strcmp(current->user.nick_name,txt)!=0 && current->user.channel==cliente_request->channel)
                        {
                          printf("current: %s\n",current->user.nick_name);
                          strcpy(server_response->nick_name,current->user.nick_name);
                          server_response->channel = current->user.channel;
                          server_response->role = current->user.role;
                          //printf("sock:%d | i:%d\n",current->user.sock,i);
    
                          memcpy(&buffer, clientToBuffer(server_response), 512);
                          send(current->user.sock, buffer, BUFSIZE, 0 );
                        }
                        current = current->next;

                      }


                      strcpy(cliente_request->nick_name,txt);
                      strcpy(cliente_request->cmd,"RPLY 001 - Nome atribuído com sucesso\0");
                      memcpy(&buffer, clientToBuffer(cliente_request), 512);
                      send(i, buffer, BUFSIZE, 0 );
 
                    }
                    else 
                    {

                      strcpy(cliente_request->cmd,"RPLY 004 - Erro: nome já em uso (num outro utilizador registado ou em uso por um utilizador não registado, e o comando não tem qualquer efeito\0");
                      memcpy(&buffer, clientToBuffer(cliente_request), 512);
                      send(i, buffer, BUFSIZE, 0 );
                    }
                    

                  }
                    
        

                }
                else if (!strncmp(cliente_request->cmd,"LIST",4))
                {
                  strcpy(cliente_request->cmd,"MSSG CHANNELS: [1,2,3,4]\0");
                  memcpy(&buffer, clientToBuffer(cliente_request), 512);
                  send(i, buffer, BUFSIZE, 0 );

                }
                else if (!strncmp(cliente_request->cmd,"JOIN",4))
                {
                  //txt
                  ssize_t size = strlen(cliente_request->cmd);
                  char txt[21];
                  memcpy( txt, &cliente_request->cmd,size);
                  txt[size] = '\0';
                  memcpy(txt, txt+5,size-5);
                  memcpy(&txt[size-5],"\0",1);

                  int prev = cliente_request->channel;
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
                    //printf("---------------------\n");
                    print_client(server_response);
                    listUser_print(active_users);

                    //mudar na tabela de utilizadores ativos
                    user_t *new_user = newUser();
                    strcpy(new_user->nick_name,cliente_request->nick_name);
                    new_user = listUser_remove(active_users,new_user,i);
                    
                    //mensagem global MSSG "server :> entrou neste canal"

                    //criar a mssg
                    char msg[481] = "";
                    strcat(msg,"MSSG ");
                    strcat(msg,cliente_request->nick_name);
                    strcat(msg," :> deixou este canal");
                   


                    //enviar a todos
                    nodeUser_t *current = active_users->header->next;

                    while (current!=NULL)
                    {
                      //printf("current: %s,channel: %d==%d \n",current->user.nick_name, current->user.channel, cliente_request->channel);
                      if (current->user.channel==prev)
                      {
                        //printf("current: %s\n",current->user.nick_name);
                        strcpy(server_response->nick_name,current->user.nick_name);
                        server_response->channel = current->user.channel;
                        server_response->role = current->user.role;
                        strcpy(server_response->cmd,msg);

                        memcpy(&buffer, clientToBuffer(server_response), 512);
                        send(current->user.sock, buffer, BUFSIZE, 0 );
                      }
                      current = current->next;

                    }

                    //listUser_print(active_users);


                    //criar a mssg
                    strcpy(msg,"");
                    strcat(msg,"MSSG ");
                    strcat(msg,cliente_request->nick_name);
                    strcat(msg," :> entrou neste canal");
                   
                    // ou MSSG "server :> deixou este canal"

                    //enviar a todos
                    current = active_users->header->next;

                    while (current!=NULL)
                    {
                      //printf("current: %s,channel: %d==%d \n",current->user.nick_name, current->user.channel, cliente_request->channel);
                      if (strcmp(current->user.nick_name,cliente_request->nick_name)!=0 && current->user.channel==cliente_request->channel)
                      {
                        //printf("current: %s\n",current->user.nick_name);
                        strcpy(server_response->nick_name,current->user.nick_name);
                        server_response->channel = current->user.channel;
                        server_response->role = current->user.role;
                        strcpy(server_response->cmd,msg);
                        //printf("sock:%d | i:%d\n",current->user.sock,i);
  
                        memcpy(&buffer, clientToBuffer(server_response), 512);
                        send(current->user.sock, buffer, BUFSIZE, 0 );
                      }
                      current = current->next;

                    }
                    
                    new_user->sock = i;
                    new_user->channel = cliente_request->channel;
                    new_user->role = cliente_request->role;
                    listUser_insert(active_users,new_user);
                    
                    strcpy(server_response->nick_name,new_user->nick_name);
                    server_response->channel = new_user->channel;
                    server_response->channel = new_user->role;

                    strcpy(server_response->cmd,"RPLY 301 - Mudança de canal com sucesso\0");
                    memcpy(&buffer, clientToBuffer(server_response), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  
        
                }
                else if (!strncmp(cliente_request->cmd,"PASS",4))
                {

                  //txt
                  ssize_t size = strlen(cliente_request->cmd);
                  char txt[21];
                  memcpy( txt, &cliente_request->cmd,size);
                  txt[size] = '\0';
                  memcpy(txt, txt+5,size-5);
                  memcpy(&txt[size-5],"\0",1);
                  printf("txt:%s n:%ld\n",txt,strlen(txt));

                  //verificar se o nome é usado
                  client_R_t *new = newClient();
                  strcpy(new->nick_name,cliente_request->nick_name);
                  new = listClient_find(database,new);

                  
                  print_ClientR(new);
 
                  printf("nick_pass: %s, input: %s\n",new->nick_name,txt);

                  if (!strncmp(cliente_request->nick_name,"Anonimo",7))
                  {
                    strcpy(cliente_request->cmd,"RPLY 202 - Erro. Nome não está definido.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  else if (new==NULL)
                  {

                    strcpy(server_response->cmd,"MSSG Utilizador não existe\0");
                    memcpy(&buffer, clientToBuffer(server_response), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  else if (!strcmp(new->pass,txt))
                  {
                    strcpy(server_response->nick_name,new->nick_name);                    
                    server_response->role = new->role;
                    server_response->channel=cliente_request->channel;

                    //mudar na tabela de utilizadores ativos
                    user_t *new_user = newUser();
                    strcpy(new_user->nick_name,new->nick_name);
                    new_user = listUser_remove(active_users,new_user,i);
                    
                    strcpy(new_user->nick_name,new->nick_name);
                    new_user->role = new->role;
                    new_user->channel = cliente_request->channel;
                    new_user->sock = i;
                    new_user->auth = 1;
                    print_User(new_user);
                    listUser_insert(active_users,new_user);
                    listUser_print(active_users);

                    strcpy(server_response->cmd,"RPLY 201 - Autenticação com sucesso.\0");
                    memcpy(&buffer, clientToBuffer(server_response), 512);
                    send(i, buffer, BUFSIZE, 0 ); 

                  }
                  else
                  {
                    strcpy(cliente_request->cmd,"RPLY 203 - Erro. Password incorreta.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );                 
                  }

                }
                else if (!strncmp(cliente_request->cmd,"WHOS",4))
                {
                  listUser_print(active_users);
                  //criar a mssg
                  char msg[485] = "";
                  char n[21];

                  strcat(msg,"MSSG IN CHANNEl ");
                  sprintf(n, "%d", cliente_request->channel);
                  strcat(msg,n);
                  strcat(msg,": ");

                  
                  //enviar a todos
                  nodeUser_t *current = active_users->header->next;

                  while (current!=NULL)
                  {
                    //printf("current: %s\n",current->user.nick_name);
                    if (current->user.channel==cliente_request->channel)
                    {
                      strcat(msg,current->user.nick_name);
                      strcat(msg,", ");

                    }
                    current = current->next;

                  }   

                  strcpy(server_response->nick_name, cliente_request->nick_name);
                  server_response->channel = cliente_request->channel;
                  server_response->role = cliente_request->role;

                  strcpy(server_response->cmd,msg);
                  memcpy(&buffer, clientToBuffer(server_response), 512);
                  send(i, buffer, BUFSIZE, 0 );

                }
                else if (!strncmp(cliente_request->cmd,"KICK",4))
                {
                  //txt
                  ssize_t size = strlen(cliente_request->cmd);
                  char txt[21];
                  memcpy( txt, &cliente_request->cmd,size);
                  txt[size] = '\0';
                  memcpy(txt, txt+5,size-5);
                  memcpy(&txt[size-5],"\0",1);

                  printf("txt: %s\n",txt);

                  if(strlen(txt)==0)
                  {
                    strcpy(cliente_request->cmd,"RPLY 002 - Erro: Falta introdução do nome.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }
                  else if (cliente_request->role!=2)
                  {
                    
                    strcpy(server_response->cmd,"RPLY 602 – Erro. Ação não autorizada, utilizador cliente não é um operador.\0");
                    memcpy(&buffer, clientToBuffer(server_response), 512);
                    send(i, buffer, BUFSIZE, 0 );

                  }
                  else if (!strcmp(txt,"admin"))
                  {
                    
                    strcpy(server_response->cmd,"MSSG KICKAR O ADMIN?!?.... Nem penses....\0");
                    memcpy(&buffer, clientToBuffer(server_response), 512);
                    send(i, buffer, BUFSIZE, 0 );

                  }
                  else
                  {

                    //verificar se o nome é usado
                    client_R_t *new = newClient();
                    strcpy(new->nick_name,txt);
                    new = listClient_find(database,new);

                    if (new==NULL)
                    {
                      strcpy(server_response->cmd,"MSSG Utilizador não existe\0");
                      memcpy(&buffer, clientToBuffer(server_response), 512);
                      send(i, buffer, BUFSIZE, 0 );
                    }
                    else
                    {
                      listClient_remove(database,new);

                      //criar a mssg
                      char msg[485] = "";
                      strcat(msg,"MSSG ");
                      strcat(msg,txt);
                      strcat(msg," :> foi expulso");

                      //enviar a todos
                      nodeUser_t *current = active_users->header->next;

                      while (current!=NULL)
                      {
                        //printf("current: %s\n",current->user.nick_name);
                        if (strcmp(current->user.nick_name,cliente_request->nick_name)!=0)
                        {
                          strcpy(server_response->nick_name,current->user.nick_name);
                          server_response->channel = current->user.channel;
                          server_response->role = current->user.role;
                          strcpy(server_response->cmd,msg);

                          memcpy(&buffer, clientToBuffer(server_response), 512);
                          send(current->user.sock, buffer, BUFSIZE, 0 ); 
                        }
                        current = current->next;

                      }   

                      strcpy(server_response->nick_name, cliente_request->nick_name);
                      server_response->channel = cliente_request->channel;
                      server_response->role = cliente_request->role;
    
                      strcpy(server_response->cmd,"RPLY 601 – Utilizador expulso.\0");
                      memcpy(&buffer, clientToBuffer(server_response), 512);
                      send(i, buffer, BUFSIZE, 0 );
                    }
                    
                  }

                }
                else if (!strncmp(cliente_request->cmd,"REGS",4))
                {
                  if (cliente_request->role==2)
                  {
                    //txt
                    ssize_t size = strlen(cliente_request->cmd);
                    char txt[42];
                    memcpy( txt, &cliente_request->cmd,size);
                    txt[size] = '\0';
                    memcpy(txt, txt+5,size-5);
                    memcpy(&txt[size-5],"\0",1);

                    printf("txt: %s\n",txt);

                    int z=0;
                    while (txt[z]!=' ')
                    {
                      z++;
                    }
                    
                    //nick
                    char nick[21];
                    memcpy( nick, &txt,z);
                    nick[z] = '\0';

                    printf("nick: %s, len:%ld\n",nick,strlen(nick));
                    //pass
                    size = strlen(txt);
                    char pass[21];
                    memcpy( pass, txt+z+1,size-z);
                    pass[size-z] = '\0';

                    printf("pass: %s, len:%ld\n",pass,strlen(pass));


                    if(strlen(txt)==0)
                    {
                      strcpy(cliente_request->cmd,"RPLY 002 - Erro: Falta introdução do nome.\0");
                      memcpy(&buffer, clientToBuffer(cliente_request), 512);
                      send(i, buffer, BUFSIZE, 0 );
                    }
                    else if(strlen(pass)==0)
                    {
                      strcpy(cliente_request->cmd,"RPLY 004 Falta introdução da pass.\0");
                      memcpy(&buffer, clientToBuffer(cliente_request), 512);
                      send(i, buffer, BUFSIZE, 0 );
                    }
                    else
                    {

                      //verificar se o nome é usado
                      client_R_t *new = newClient();
                      strcpy(new->nick_name,nick);
                      new = listClient_find(database,new);


                      if (new==NULL)
                      {
                        client_R_t *new_client = newClient();
                        strcpy(new_client->nick_name,nick);
                        strcpy(new_client->pass,pass);
                        new_client->role = 1;
                      

                        listClient_insert(database,new_client);
                        listClient_print(database);

                        strcpy(server_response->cmd,"RPLY 701 – Utilizador foi registado com sucesso.\0");
                        memcpy(&buffer, clientToBuffer(server_response), 512);
                        send(i, buffer, BUFSIZE, 0 );
                      }
                      else
                      {
                        strcpy(server_response->cmd,"RPLY 701 – Utilizador já existe.\0");
                        memcpy(&buffer, clientToBuffer(server_response), 512);
                        send(i, buffer, BUFSIZE, 0 );
                      }

                    }
                  
                  }
                  else
                  {
                    strcpy(server_response->cmd,"RPLY 702 – Erro. Ação não autorizada, utilizador cliente não é um operador.\0");
                    memcpy(&buffer, clientToBuffer(server_response), 512);
                    send(i, buffer, BUFSIZE, 0 );
                  }

                  listUser_print(active_users);

                }
                else if (!strncmp(cliente_request->cmd,"OPER",4))
                {
                  //txt
                  ssize_t size = strlen(cliente_request->cmd);
                  char txt[21];
                  memcpy( txt, &cliente_request->cmd,size);
                  txt[size] = '\0';
                  memcpy(txt, txt+5,size-5);
                  memcpy(&txt[size-5],"\0",1);

                  printf("txt: %s\n",txt);

                  client_R_t *new = newClient();
                  strcpy(new->nick_name,txt);
                  new = listClient_find(database,new);

                  user_t *new1 = newUser();
                  strcpy(new1->nick_name,cliente_request->nick_name);
                  new1 = listUser_find(active_users,new1,i);



                  if (cliente_request->role==2)
                  {
                    if(strlen(txt)==0)
                    {
                      strcpy(cliente_request->cmd,"RPLY 002 - Erro: Falta introdução do nome.\0");
                      memcpy(&buffer, clientToBuffer(cliente_request), 512);
                      send(i, buffer, BUFSIZE, 0 );
                    }
                    else if (new==NULL)
                    {
                      strcpy(cliente_request->cmd,"RPLY 804 – Erro. Ação não autorizada, utilizador (nome) não é um utilizador registado.\0");
                      memcpy(&buffer, clientToBuffer(cliente_request), 512);
                      send(i, buffer, BUFSIZE, 0 );    
                    }
                    else
                    {
                      user_t *new1 = newUser();
                      strcpy(new1->nick_name,txt);
                      new1 = listUser_find_name(active_users,new1);



                      print_User(new1);
                      
                      if (new1->auth==0)
                      {

                        strcpy(cliente_request->cmd,"RPLY 803 – Erro. Ação não autorizada, utilizador cliente não está autenticado.\0");
                        memcpy(&buffer, clientToBuffer(cliente_request), 512);
                        send(i, buffer, BUFSIZE, 0 );                      
                      }
                      else
                      {
                        new1 = listUser_remove(active_users,new1,new1->sock);
                        new1->role = 2;
                        listUser_insert(active_users,new1);

                        new = listClient_remove(database,new);
                        new->role=2;
                        listClient_insert(database,new);

                        print_ClientR(new);

                        strcpy(server_response->nick_name,new1->nick_name);
                        server_response->channel = new1->channel;
                        server_response->role = new1->role;

                        
                        strcpy(server_response->cmd,"RPLY 801 – Foi promovido a operador.\0");
                        memcpy(&buffer, clientToBuffer(server_response), 512);
                        send(new1->sock, buffer, BUFSIZE, 0 ); 


                        //criar a mssg
                        char msg[485] = "";
                        strcat(msg,"MSSG Server :> ");
                        strcat(msg,txt);
                        strcat(msg,"  promovido a operador.");

                        //enviar a todos
                        nodeUser_t *current = active_users->header->next;

                        while (current!=NULL)
                        {
                          if (strcmp(current->user.nick_name,txt)!=0 && strcmp(current->user.nick_name,cliente_request->nick_name)!=0)
                          {
                            //printf("current: %s\n",current->user.nick_name);
                            strcpy(server_response->nick_name,current->user.nick_name);
                            server_response->channel = current->user.channel;
                            server_response->role = current->user.role;
                            strcpy(server_response->cmd,msg);

                            memcpy(&buffer, clientToBuffer(server_response), 512);
                            send(current->user.sock, buffer, BUFSIZE, 0 );                         
                          }

                          current = current->next;

                        }   

                        strcpy(cliente_request->cmd,"MSSG Cliente foi promovido\0");
                        memcpy(&buffer, clientToBuffer(cliente_request), 512);
                        send(i, buffer, BUFSIZE, 0 ); 


                      }

                      //update cliente
                      
                    }
                    
                    
                  }
                  else
                  {
                    strcpy(cliente_request->cmd,"RPLY 802 – Erro. Ação não autorizada, utilizador cliente não é um operador.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );                  
                  }

                }
                else if (!strncmp(cliente_request->cmd,"QUIT",4))
                {
                  client_R_t *new = newClient();
                  strcpy(new->nick_name,cliente_request->nick_name);
                  new = listClient_find(database,new);

                  user_t *new1 = newUser();
                  strcpy(new1->nick_name,cliente_request->nick_name);
                  new1 = listUser_find(active_users,new1,i);

                  if (cliente_request->role==2)
                  {

                    new = listClient_remove(database,new);
                    new->role=1;
                    listClient_insert(database,new);

                    print_ClientR(new);

                    new1 = listUser_remove(active_users,new1,new1->sock);
                    new1->role = 1;
                    listUser_insert(active_users,new1);

                    print_User(new1);



                    //update cliente
                    strcpy(server_response->nick_name,new1->nick_name);
                    server_response->channel = new1->channel;
                    server_response->role = new1->role;

                    
                    strcpy(server_response->cmd,"RPLY 901 – Deixou de ser operador.\0");
                    memcpy(&buffer, clientToBuffer(server_response), 512);
                    send(new1->sock, buffer, BUFSIZE, 0 ); 


                    //criar a mssg
                    char msg[485] = "";
                    strcat(msg,"MSSG Server :> ");
                    strcat(msg,cliente_request->nick_name);
                    strcat(msg,"  deixou de ser operador.");

                    //enviar a todos
                    nodeUser_t *current = active_users->header->next;

                    while (current!=NULL)
                    {
                      if (strcmp(current->user.nick_name,cliente_request->nick_name)!=0)
                      {
                        //printf("current: %s\n",current->user.nick_name);
                        strcpy(server_response->nick_name,current->user.nick_name);
                        server_response->channel = current->user.channel;
                        server_response->role = current->user.role;
                        strcpy(server_response->cmd,msg);

                        memcpy(&buffer, clientToBuffer(server_response), 512);
                        send(current->user.sock, buffer, BUFSIZE, 0 );                         
                      }

                      current = current->next;

                    }   

                  }
                  else
                  {
                    strcpy(cliente_request->cmd,"902 – Erro. Ação não autorizada, utilizador cliente não é um operador.\0");
                    memcpy(&buffer, clientToBuffer(cliente_request), 512);
                    send(i, buffer, BUFSIZE, 0 );                  
                  }
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