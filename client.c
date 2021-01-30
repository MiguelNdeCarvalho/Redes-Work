#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define MAXDATASIZE 200 // max number of bytes we can get at once
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT "5555"


int main()
{
	char msg_received[MAXDATASIZE], msg_sent[MAXDATASIZE];
	struct addrinfo *result;

	int addr_info = getaddrinfo(SERVER_ADDR , SERVER_PORT , NULL, &result);
        int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
        int connected = connect(sockfd, result->ai_addr, result->ai_addrlen);

	if(connected != 0)
	{
		printf("There was an error connecting\n");
		return 0;
	}

	for (;;)
	{
		gets(msg_sent);
		send(sockfd, msg_sent, strlen(msg_sent), 0);	

		int numbytes=recv(sockfd, msg_received, MAXDATASIZE-1, 0);
		msg_received[numbytes] = '\0';
		printf("Os dados recebidos são: %s \n",msg_received);		
	}

	close(sockfd);

	return 0;
}
