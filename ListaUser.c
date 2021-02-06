
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

//#include <server.c>

typedef struct user{
	
    int sock;
    char nick_name[21]; //21 bytes; começa em 485 e termina em 506
    int channel; //4bytes; começa em 507 e termina em 511
	bool role;  //1 Byte; 512;

}user_t;

typedef struct node {
    
    user_t user;
    struct node *next;

} nodeDB_t;

typedef struct listDB {
   
    nodeDB_t *header;
    int size;

}listDB_t;

user_t *newUser()
{
    user_t *new = malloc(sizeof(user_t));
    return new;
}

nodeDB_t *newNodeDB()
{
    nodeDB_t *new = malloc(sizeof(nodeDB_t));
    new->next = NULL;
    return new;
}

listDB_t *newListDB()
{
    listDB_t *new = malloc(sizeof(listDB_t));
    new->header = newNodeDB();
    new->size = 0;
    return new;
}


//Escreve na consola a informacao de um dado pais
void listDB_print(listDB_t *list){
    
    nodeDB_t *current = newNodeDB();
    current = list->header->next;
    while (current!=NULL)
    {
        printf("\"%s\", ",current->user.nick_name);
        current=current->next;
    }
}

void listDB_insert(listDB_t *list, user_t *user){
  

    nodeDB_t *new = newNodeDB();
    new->user = *user;
    new->next=list->header->next;
    list->header->next=new;

    list->size++;
    
}

user_t *listDB_find(listDB_t *list, user_t *user)
{
    nodeDB_t *current = list->header->next;

    if (current==NULL)
    {
        return NULL;
    }

    while (current!=NULL)
    {
        if (strcmp(current->user.nick_name,user->nick_name) == 0)
        {
            user_t *found_user = newUser();
            strcpy(found_user->nick_name,user->nick_name);
            found_user->sock = user->sock;
            found_user->role = user->role;
            found_user->channel = user->channel;

            return found_user;
        }
        current=current->next;
    }


    return NULL;
}

int main(int argc, char const *argv[])
{
    user_t *oi = newUser();
    strcpy(oi->nick_name,"jonny");
    listDB_t *lista = newListDB(); 
    listDB_insert(lista,oi);

    user_t *found = listDB_find(lista,oi);
    printf("%s\n",found->nick_name);
    listDB_print(lista);
    return 0;
}
