#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>

#define STDIN 0
#define SIZE 1024

int max(int x,int y){
	return (x>y)?x:y;	
}

int main(){
	int sockfd;
	struct sockaddr_in servaddr;
	struct in_addr cli_ip;
	unsigned short cli_port;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if(sockfd < 0){
		perror("socket creation failed");
		exit(1);
	}

	servaddr.sin_family = AF_INET;
	inet_aton("127.0.0.1",&servaddr.sin_addr);
	servaddr.sin_port = htons(6000);

	if(connect(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
		perror("cannot connect");
		exit(1);
	}
	
	char user_input[10],cliaddr[20],buf[20];
	int nread,i,n,r,nfds,port;
	fd_set readfds;
	while(1){
		FD_ZERO(&readfds);
		FD_SET(STDIN,&readfds);
		FD_SET(sockfd,&readfds);

		nfds = max(STDIN,sockfd) + 1;

		r= select(nfds, &readfds, 0, 0, 0);
		if(r < 0){
			perror("Error");
			exit(1);		
		}

		if(FD_ISSET(STDIN,&readfds)){
			nread = read(STDIN, user_input, SIZE);
			user_input[nread]='\0';
			send(sockfd, user_input, nread+1, 0);
			printf("Client Message ");
			for(i=0;i<9;i++)
			{
				printf("%c",user_input[i]);
			}
			printf(" sent to server\n");
		}

		if(FD_ISSET(sockfd,&readfds)){
			memset(buf, 0, sizeof(buf));
			recv(sockfd, &cli_ip, sizeof(cli_ip), MSG_WAITALL);
			recv(sockfd, &cli_port, sizeof(cli_port), MSG_WAITALL);
			recv(sockfd, buf, 10, MSG_WAITALL);
			strcpy(cliaddr,inet_ntoa(cli_ip));
			port = ntohs(cli_port);
			printf("Client: Received ");
			for(i=0;i<9;i++)
			{
				printf("%c",buf[i]);
			}
			printf(" from %s:%d\n",cliaddr,port);	
		}			

	}

	close(sockfd);

}
