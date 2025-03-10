#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h> 

#define MAXDATASIZE 512 // max number of bytes we can get at once
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT "5555"

typedef struct client{
	
	char cmd[483]; // 482 Byte; começa em 0 e termina em 483
	char nick_name[21]; //21 bytes; começa em 484 e termina em 504
	int channel; //4 bytes; começa em 505 e termina em 508
	int role;  //4 Byte; 509-512;

}client_t;


void print_client(client_t *n)
{
	printf("client_status( nick:%s role:%d channel:%d, cmd:%s)\n"
							,n->nick_name,n->role,n->channel,n->cmd);
}

client_t *bufferToClient(char buff[MAXDATASIZE])
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

  char *buf = malloc(MAXDATASIZE);
  bzero(buf, MAXDATASIZE);

  int k = 0;
  memcpy(buf + k, &n->cmd, sizeof(n->cmd));  k += sizeof(n->cmd);
  memcpy(buf + k, &n->nick_name, sizeof(n->nick_name));  k += sizeof(n->nick_name);
  memcpy(buf + k, &n->channel, sizeof(n->channel));  k += sizeof(n->channel);
  memcpy(buf + k, &n->role,sizeof(n->role)); 

  return buf;
} 

int main()
{
	char buffer[MAXDATASIZE];
	struct addrinfo *result;

	int addr_info = getaddrinfo(SERVER_ADDR , SERVER_PORT , NULL, &result);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    int connected = connect(sockfd, result->ai_addr, result->ai_addrlen);

	if(connected != 0)
	{
		printf("There was an error connecting\n");
		return 0;
	}

	bzero(buffer, MAXDATASIZE);

    client_t *cliente = malloc(sizeof(client_t));
	//int numbytes=recv(sockfd, msg_received, MAXDATASIZE-1, 0);
	size_t total = 0; /* everything received */
	while ( 1 ) {
		if ( total == MAXDATASIZE ) break; /* got end-of-stream */
		ssize_t nb = recv( sockfd, buffer, MAXDATASIZE, 0 );
		if ( nb == -1 ) printf( "recv failed" );
		total += nb;
	}
	/* send an ack here */
	//printf( "received and saved total of %zu bytes\n", total );

	
	cliente = bufferToClient(buffer);
	//printf("ja recebi\n");
	print_client(cliente);
    memcpy(&buffer, clientToBuffer(cliente), 512);
    //cliente = bufferToClient(buffer);
    //print_client(cliente);

	while (1)
	{	

		char str[483]; 
		printf("\nCONSOLE:");
		fgets(str,483,stdin); 
		strcpy(cliente->cmd,str); 
		printf("\n");

		cliente->cmd[strlen(cliente->cmd)-1]='\0';
		
		if(!strcmp(cliente->cmd,"recv"))
		{
			total = 0; /* everything received */
			while ( 1 ) {
				if ( total == MAXDATASIZE ) break; /* got end-of-stream */
				ssize_t nb = recv( sockfd, buffer, MAXDATASIZE, 0 );
				if ( nb == -1 ) printf( "recv failed" );
				total += nb;
			}

			client_t *server_response = malloc(sizeof(client_t));
			server_response = bufferToClient(buffer);
			//printf("MSSG: ");
			print_client(server_response);

			if (!strncmp(server_response->cmd,"RPLY",4))
			{
				//cod
				ssize_t size = strlen(server_response->cmd);
				char cod[size];
				memcpy( cod, &server_response->cmd,size);
				cod[size] = '\0';
				memcpy(cod, cod+5,size-4);
				memcpy(&cod[size-4],"\0",1);
				printf("cod:%s n:%ld\n",cod,strlen(cod));

				if (!strncmp(cod,"001",3))
				{
					//output
					size = strlen(server_response->cmd);
					char output[size];
					memcpy( output, &server_response->cmd,size);
					output[size] = '\0';
					memcpy(output, output+9,size-4);
					memcpy(&output[size-4],"\0",1);
					printf("%s\n",output);

					//mudar os dados
					strcpy(cliente->nick_name,server_response->nick_name);
					cliente->role = server_response->role;
					cliente->channel = server_response->channel;
					strcpy(cliente->cmd,"\0");

				}

				if (!strncmp(cod,"301",3))
				{
					//output
					size = strlen(server_response->cmd);
					char output[size];
					memcpy( output, &server_response->cmd,size);
					output[size] = '\0';
					memcpy(output, output+9,size-4);
					memcpy(&output[size-4],"\0",1);
					printf("%s\n",output);

					//mudar os dados
					strcpy(cliente->nick_name,server_response->nick_name);
					cliente->role = server_response->role;
					cliente->channel = server_response->channel;
					strcpy(cliente->cmd,"\0");

				}

				if (!strncmp(cod,"801",3))
				{
					//output
					size = strlen(server_response->cmd);
					char output[size];
					memcpy( output, &server_response->cmd,size);
					output[size] = '\0';
					memcpy(output, output+9,size-4);
					memcpy(&output[size-4],"\0",1);
					printf("%s\n",output);

					//mudar os dados
					cliente->role = server_response->role;
					strcpy(cliente->cmd,"\0");

				}				//cliente->nick_name


			}

			if (!strncmp(server_response->cmd,"MSSG",4))
			{
					//cod
				ssize_t size = strlen(server_response->cmd);
				char cod[size];
				memcpy( cod, &server_response->cmd,size);
				cod[size] = '\0';
				memcpy(cod, cod+5,size-4);
				memcpy(&cod[size-4],"\0",1);
				//printf("cod:%s n:%ld\n",cod,strlen(cod));

				printf("%s\n",cod);

			}
		}
		else
		{
			if (!strcmp(cliente->cmd,"quit")){
				printf("\nCLIENT CLOSED\n");
				close(sockfd);
				return 0;
			}

			memcpy(&buffer, clientToBuffer(cliente), 512);
			
			size_t total = 0;
			int len = MAXDATASIZE;

			while ( total != len ) {
				if ( total == MAXDATASIZE ) break; 
				ssize_t nb = send( sockfd, buffer + total, len - total, 0 );
				total += nb;
			}

			//printf( "seded total of %zu bytes\n", total );

			printf("\nI sent this: ");
			print_client(cliente);

			bzero(buffer, MAXDATASIZE);
			
			
			bzero(buffer, MAXDATASIZE);
			//int numbytes=recv(sockfd, msg_received, MAXDATASIZE-1, 0);
			total = 0; /* everything received */
			while ( 1 ) {
				if ( total == MAXDATASIZE ) break; /* got end-of-stream */
				ssize_t nb = recv( sockfd, buffer, MAXDATASIZE, 0 );
				if ( nb == -1 ) printf( "recv failed" );
				total += nb;
			}

			client_t *server_response = malloc(sizeof(client_t));
			server_response = bufferToClient(buffer);
			printf("\nI received this: ");
			print_client(server_response);
		
			if (!strncmp(server_response->cmd,"RPLY",4))
			{
				//cod
				ssize_t size = strlen(server_response->cmd);
				char cod[size];
				memcpy( cod, &server_response->cmd,size);
				cod[size] = '\0';
				memcpy(cod, cod+5,size-4);
				memcpy(&cod[size-4],"\0",1);
				printf("\n%s\n",cod);

				if (!strncmp(cod,"001",3))
				{
					//output
					size = strlen(server_response->cmd);
					char output[size];
					memcpy( output, &server_response->cmd,size);
					output[size] = '\0';
					memcpy(output, output+9,size-4);
					memcpy(&output[size-4],"\0",1);
					printf("\n%s\n",output);

					//mudar os dados
					strcpy(cliente->nick_name,server_response->nick_name);
					cliente->role = server_response->role;
					cliente->channel = server_response->channel;
					strcpy(cliente->cmd,"\0");

				}

				if (!strncmp(cod,"301",3))
				{
					//output
					size = strlen(server_response->cmd);
					char output[size];
					memcpy( output, &server_response->cmd,size);
					output[size] = '\0';
					memcpy(output, output+9,size-4);
					memcpy(&output[size-4],"\0",1);
					printf("\n%s\n",output);

					//mudar os dados
					strcpy(cliente->nick_name,server_response->nick_name);
					cliente->role = server_response->role;
					cliente->channel = server_response->channel;
					strcpy(cliente->cmd,"\0");

				}

				if (!strncmp(cod,"201",3))
				{
					//output
					size = strlen(server_response->cmd);
					char output[size];
					memcpy( output, &server_response->cmd,size);
					output[size] = '\0';
					memcpy(output, output+9,size-4);
					memcpy(&output[size-4],"\0",1);
					printf("\n%s\n",output);

					//mudar os dados
					strcpy(cliente->nick_name,server_response->nick_name);
					cliente->role = server_response->role;
					cliente->channel = server_response->channel;
					strcpy(cliente->cmd,"\0");

				}
				//cliente->nick_name
				if (!strncmp(cod,"801",3))
				{
					//output
					size = strlen(server_response->cmd);
					char output[size];
					memcpy( output, &server_response->cmd,size);
					output[size] = '\0';
					memcpy(output, output+9,size-4);
					memcpy(&output[size-4],"\0",1);
					printf("\n%s\n",output);

					//mudar os dados
					cliente->role = server_response->role;
					strcpy(cliente->cmd,"\0");

				}	

				if (!strncmp(cod,"901",3))
				{
					//output
					size = strlen(server_response->cmd);
					char output[size];
					memcpy( output, &server_response->cmd,size);
					output[size] = '\0';
					memcpy(output, output+9,size-4);
					memcpy(&output[size-4],"\0",1);
					printf("\n%s\n",output);

					//mudar os dados
					cliente->role = server_response->role;
					strcpy(cliente->cmd,"\0");

				}	

			}

			if (!strncmp(server_response->cmd,"MSSG",4))
			{
				//cod
				ssize_t size = strlen(server_response->cmd);
				char cod[size];
				memcpy( cod, &server_response->cmd,size);
				cod[size] = '\0';
				memcpy(cod, cod+5,size-4);
				memcpy(&cod[size-4],"\0",1);
				//printf("cod:%s n:%ld\n",cod,strlen(cod));

				strcpy(cliente->nick_name,server_response->nick_name);
				cliente->role = server_response->role;
				cliente->channel = server_response->channel;
				
				printf("%s\n",cod);

			}










			
		}
		
		
		
	}

	

	return 0;
}
