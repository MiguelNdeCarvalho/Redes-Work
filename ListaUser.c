
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>


typedef struct user{
	
    int sock;
    char nick_name[21]; //21 bytes; começa em 485 e termina em 506
    int channel; //4bytes; começa em 507 e termina em 511
	int role;  //1 Byte; 512;

}user_t;

typedef struct nodeUser {
    
    user_t user;
    struct nodeUser *next;

} nodeUser_t;

typedef struct listUser {
   
    nodeUser_t *header;
    int size;

}listUser_t;

user_t *newUser()
{
    user_t *new = malloc(sizeof(user_t));
    return new;
}

nodeUser_t *newNodeUser()
{
    nodeUser_t *new = malloc(sizeof(nodeUser_t));
    new->next = NULL;
    return new;
}

listUser_t *newListUser()
{
    listUser_t *new = malloc(sizeof(listUser_t));
    new->header = newNodeUser();
    new->size = 0;
    return new;
}

void print_user(user_t n)
{
	printf("client_user( nick:%s role:%d channel:%d, sock:%d)\n"
							,n.nick_name,n.role,n.channel,n.sock);
}
//Escreve na consola a informacao de um dado pais
void listUser_print(listUser_t *list){
    
    nodeUser_t *current = newNodeUser();
    current = list->header->next;
    while (current!=NULL)
    {
        print_user(current->user);
        current=current->next;
    }
    printf("\n");

}

void listUser_insert(listUser_t *list, user_t *user){
  

    nodeUser_t *new = newNodeUser();
    new->user = *user;
    new->next=list->header->next;
    list->header->next=new;

    list->size++;
    
}

user_t *listUser_find(listUser_t *list, user_t *user, int i)
{
    nodeUser_t *current = list->header->next;

    if (current==NULL)
    {
        return NULL;
    }

    while (current!=NULL)
    {
        if (strcmp(current->user.nick_name,user->nick_name) == 0 && current->user.sock==i)
        {
            user_t *found_user = newUser();
            strcpy(found_user->nick_name,user->nick_name);
            found_user->role = user->role;
            found_user->channel = user->channel;

            return found_user;
        }
        current=current->next;
    }


    return NULL;
}

user_t *listUser_find_name(listUser_t *list, user_t *user)
{
    nodeUser_t *current = list->header->next;

    if (current==NULL)
    {
        return NULL;
    }

    while (current!=NULL)
    {
        if (strcmp(current->user.nick_name,user->nick_name) == 0 )
        {
            user_t *found_user = newUser();
            strcpy(found_user->nick_name,user->nick_name);
            found_user->role = user->role;
            found_user->channel = user->channel;

            return found_user;
        }
        current=current->next;
    }


    return NULL;
}

user_t *listUser_remove(listUser_t *list, user_t *user,int i)
{
    nodeUser_t *prev = list->header;
    nodeUser_t *current = list->header->next;

    if (current==NULL)
    {
        return NULL;
    }

    while (current!=NULL)
    {
        if (strcmp(current->user.nick_name,user->nick_name) == 0 && current->user.sock==i)
        {
            user_t *client_R = newUser();
            strcpy(client_R->nick_name,user->nick_name);
            client_R->sock = user->sock;
            client_R->role = user->role;
            client_R->channel = user->channel;

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
    user_t *oi = newUser();
    strcpy(oi->nick_name,"jonny");
    listUser_t *lista = newListUser(); 
    listUser_insert(lista,oi);

    user_t *found = listUser_find(lista,oi);
    printf("%s\n",found->nick_name);
    listUser_print(lista);
    return 0;
}
*/