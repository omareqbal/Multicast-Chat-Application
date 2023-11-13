#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>

typedef struct{
	struct in_addr ip;
	unsigned short port;
}cli_ip_port;

int main(){
	int sockfd, newsockfd, clientsockfd[5];
	cli_ip_port clientid[5];
	struct sockaddr_in servaddr, cliaddr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if(sockfd < 0){
		perror("socket creation failed");
		exit(1);
	}
	char cli_ip[20], cli_ip2[20];
	int cli_port, cli_port2;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(6000);

	if(bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
		perror("bind failed");
		exit(1);
	}

	listen(sockfd, 5);
	
	int i,j,k, n, numclient=0, clilen;
	for(i=0;i<5;i++)
		clientsockfd[i] = -1;

	if(fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0){
		perror("error");
		exit(1);
	}
	
	char buf[10];
	char msg[10];
	while(1){
		sleep(1);
		clilen = sizeof(cliaddr);		
		newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &clilen);
		if(newsockfd >= 0){
			clientsockfd[numclient] = newsockfd;
			clientid[numclient].ip = cliaddr.sin_addr;
			clientid[numclient].port = cliaddr.sin_port;
			strcpy(cli_ip,inet_ntoa(clientid[numclient].ip));
			cli_port = ntohs(clientid[numclient].port);
			printf("Server: Received new connection from client %s:%d\n",cli_ip, cli_port);
			numclient++;
		}
		
		for(i=0;i<5;i++){
			n = recv(clientsockfd[i], buf, 10, MSG_DONTWAIT);
			if(n < 0){
				continue;
			}
			memset(msg, 0, sizeof(msg));
			j=0;
			while(1){
				for(k=0;k<n;k++){
					msg[j++] = buf[k];
					if(buf[k]=='\0')
						break;
				}
				if(k<n)
					break;
				n = recv(clientsockfd[i], buf, 10, 0);	
			}
			
			strcpy(cli_ip,inet_ntoa(clientid[i].ip));
			cli_port = ntohs(clientid[i].port);
			printf("Server: Received message ");
			for(k=0;k<9;k++)
			{
				printf("%c",msg[k]);
			}
			printf(" from client %s:%d\n",cli_ip, cli_port);
			
			if(numclient==1){
				printf("Server: Insufficient clients, ");
				for(k=0;k<9;k++)
				{
					printf("%c",msg[k]);
				}
				printf(" from client %s:%d dropped\n",cli_ip,cli_port);
				continue;
			}
			for(j=0;j<5;j++){
				if(j==i || clientsockfd[j]==-1)
					continue;
				send(clientsockfd[j], &clientid[i].ip, sizeof(clientid[i].ip),0);
				send(clientsockfd[j], &clientid[i].port, sizeof(clientid[i].port),0);
				send(clientsockfd[j], msg, 10, 0);
				
				strcpy(cli_ip2,inet_ntoa(clientid[j].ip));
				cli_port2 = ntohs(clientid[j].port);
				printf("Server: Sent message ");
				for(k=0;k<9;k++)
				{
					printf("%c",msg[k]);
				}
				printf(" from client %s:%d to %s:%d\n",cli_ip, cli_port,cli_ip2,cli_port2);		
							
			}				
		}		
	}

	for(i=0;i<5;i++){
		if(clientsockfd[i]!=-1)
			close(clientsockfd[i]);	
	}
	if(newsockfd != -1)
		close(newsockfd);
	close(sockfd);
}
