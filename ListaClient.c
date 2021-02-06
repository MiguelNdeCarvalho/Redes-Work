
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

typedef struct node {
    
    client_R_t user;
    struct node *next;

} nodeClient_R_t;

typedef struct listDB {
   
    nodeClient_R_t *header;
    int size;

}listClient_R_t;

client_R_t *newUser()
{
    client_R_t *new = malloc(sizeof(client_R_t));
    return new;
}

nodeClient_R_t *newNodeDB()
{
    nodeClient_R_t *new = malloc(sizeof(nodeClient_R_t));
    new->next = NULL;
    return new;
}

listClient_R_t *newListDB()
{
    listClient_R_t *new = malloc(sizeof(listClient_R_t));
    new->header = newNodeDB();
    new->size = 0;
    return new;
}


//Escreve na consola a informacao de um dado pais
void listDB_print(listClient_R_t *list){
    
    nodeClient_R_t *current = newNodeDB();
    current = list->header->next;
    while (current!=NULL)
    {
        printf("\"%s\", ",current->user.nick_name);
        current=current->next;
    }
}

void listDB_insert(listClient_R_t *list, client_R_t *user){
  

    nodeClient_R_t *new = newNodeDB();
    new->user = *user;
    new->next=list->header->next;
    list->header->next=new;

    list->size++;
    
}

client_R_t *listDB_find(listClient_R_t *list, client_R_t *user)
{
    nodeClient_R_t *current = list->header->next;

    if (current==NULL)
    {
        return NULL;
    }

    while (current!=NULL)
    {
        if (strcmp(current->user.nick_name,user->nick_name) == 0)
        {
            client_R_t *client_R = newUser();
            strcpy(client_R->nick_name,user->nick_name);
            strcpy(client_R->pass,user->pass);
            client_R->role = user->role;
            client_R->channel = user->channel;

            return client_R;
        }
        current=current->next;
    }


    return NULL;
}

int main(int argc, char const *argv[])
{
    client_R_t *oi = newUser();
    strcpy(oi->nick_name,"client registed");
    listClient_R_t *lista = newListDB(); 
    listDB_insert(lista,oi);

    client_R_t *found = listDB_find(lista,oi);
    printf("%s\n",found->nick_name);
    listDB_print(lista);
    return 0;
}
