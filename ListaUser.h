#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

typedef struct user{
	
    int sock;
    char nick_name[21]; //21 bytes; começa em 485 e termina em 506
    int channel; //4bytes; começa em 507 e termina em 511
	bool role;  //1 Byte; 512;

}user_t;

typedef struct nodeUser {
    
    user_t user;
    struct nodeUser *next;

} nodeUser_t;

typedef struct listUser {
   
    nodeUser_t *header;
    int size;

}listUser_t;

struct user *newUser();
struct listUser *newNodeUser();
struct listUser *newListUser();


//Escreve na consola a informacao de um dado pais
void listUser_print(listUser_t *list);
void listUser_insert(listUser_t *list, user_t *user);
struct user *listUser_find(listUser_t *list, user_t *user);
struct user *listUser_remove(listUser_t *list, user_t *user);

#endif
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