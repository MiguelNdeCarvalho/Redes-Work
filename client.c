#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define MAXDATASIZE 200 // max number of bytes we can get at once 


int main()
{  
	char buf[MAXDATASIZE];
	struct addrinfo *result;

	int addr_info = getaddrinfo("time.nist.gov", "13" , NULL, &result);
        int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
        int connected = connect(sockfd, result->ai_addr, result->ai_addrlen);

	if(connected == 0)
	{
		int numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0);
		buf[numbytes] = '\0';
	}
	else
	{
		printf("There was an error connecting\n");
		return 0;
	}
	
	printf("Os dados recebidos são: %s \n",buf);
	close(sockfd);

	return 0;
}
