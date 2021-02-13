
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

typedef struct client_R{
	
	char nick_name[21]; //21 bytes; começa em 485 e termina em 506
    char pass[21]; //21 bytes; começa em 485 e termina em 506
	int role;  //1 Byte; 512;

}client_R_t;

typedef struct nodeClient {
    
    client_R_t client;
    struct nodeClient *next;

} nodeClient_R_t;

typedef struct listClient {
   
    nodeClient_R_t *header;
    int size;

}listClient_R_t;

client_R_t *newClient()
{
    client_R_t *new = malloc(sizeof(client_R_t));
    return new;
}

nodeClient_R_t *newNodeClient()
{
    nodeClient_R_t *new = malloc(sizeof(nodeClient_R_t));
    new->next = NULL;
    return new;
}

listClient_R_t *newListClient()
{
    listClient_R_t *new = malloc(sizeof(listClient_R_t));
    new->header = newNodeClient();
    new->size = 0;
    return new;
}

void print_R(client_R_t n)
{
	printf("client_status( nick:%s role:%d pass:%s)\n"
							,n.nick_name,n.role,n.pass);
}

//Escreve na consola a informacao de um dado pais
void listClient_print(listClient_R_t *list){
    
    nodeClient_R_t *current = newNodeClient();
    current = list->header->next;
    while (current!=NULL)
    {
        print_R(current->client);
        current=current->next;
    }
    printf("\n");
}

void listClient_insert(listClient_R_t *list, client_R_t *client){
  

    nodeClient_R_t *new = newNodeClient();
    new->client = *client;
    new->next=list->header->next;
    list->header->next=new;

    list->size++;
    
}

client_R_t *listClient_find(listClient_R_t *list, client_R_t *client)
{
    nodeClient_R_t *current = list->header->next;

    if (current==NULL)
    {
        return NULL;
    }

    while (current!=NULL)
    {
        if (strcmp(current->client.nick_name,client->nick_name) == 0)
        {
            client_R_t *client_R = newClient();
            strcpy(client_R->nick_name,current->client.nick_name);
            strcpy(client_R->pass,current->client.pass);
            client_R->role = current->client.role;

            return client_R;
        }
        current=current->next;
    }
    return NULL;
}

client_R_t *listClient_remove(listClient_R_t *list, client_R_t *client)
{
    nodeClient_R_t *prev = list->header;
    nodeClient_R_t *current = list->header->next;

    if (current==NULL)
    {
        return NULL;
    }

    while (current!=NULL)
    {
        if (strcmp(current->client.nick_name,client->nick_name) == 0)
        {
            client_R_t *client_R = newClient();
            strcpy(client_R->nick_name,current->client.nick_name);
            strcpy(client_R->pass,current->client.pass);
            client_R->role = current->client.role;

            prev->next=current->next;
            current=prev;

            return client_R;
        }
        prev=prev->next;
        current=current->next;
    }


    return NULL;
}

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