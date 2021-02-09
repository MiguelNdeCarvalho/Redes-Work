#ifndef _LIST_
#define _LIST_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

//#include <server.c>

typedef struct client_R{
	
	char nick_name[21]; //21 bytes; começa em 485 e termina em 506
  char pass[21]; //21 bytes; começa em 485 e termina em 506
	int channel; //4bytes; começa em 507 e termina em 511
	bool role;  //1 Byte; 512;

}client_R_t;

typedef struct nodeClient {
    
    client_R_t client;
    struct nodeClient *next;

} nodeClient_R_t;

typedef struct listClient {
   
    nodeClient_R_t *header;
    int size;

}listClient_R_t;

struct client_R *newClient();
struct nodeClient *newNodeClient();
struct listClient *newListClient();


//Escreve na consola a informacao de um dado pais
void listClient_print(listClient_R_t *list);

void listClient_insert(listClient_R_t *list, client_R_t *client);

struct client_R *listClient_find(listClient_R_t *list, client_R_t *client);

struct client_R *listClient_remove(listClient_R_t *list, client_R_t *client);

#endif
/*
int main(int argc, char const *argv[])
{
    client_R_t *oi = newClient();
    strcpy(oi->nick_name,"client registed");
    listClient_R_t *lista = newListClient(); 
    listClient_insert(lista,oi);

    client_R_t *found = listClient_find(lista,oi);
    printf("%s\n",found->nick_name);
    listClient_remove(lista,found);
    listClient_print(lista);
    return 0;
}
*/