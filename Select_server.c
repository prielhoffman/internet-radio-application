#define _CRT_SECURE_NO_WARNINGS
#define Buffer_size 1024
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>

#define Buffer_size 1024


int main(int argc, char *argv[]){

	int inputfd, i, sockets[argc - 1], ports[argc - 1], num_of_sockets = argc - 1, client_socket, receiveing;
	char buffer[Buffer_size];
	memset(buffer, '\0', sizeof(buffer));
	char input;
	struct sockaddr_in server_addr;

//create an array of port numbers
	for (i = 1; i < argc; i++) {
		ports[i - 1] = atoi(argv[i]);
	}

//Prepare the sockaddr_in structure
	socklen_t addr_size;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY); 
	memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));
	
	fd_set fdset;
	FD_ZERO(&fdset);

//creating sockets
	for(i = 0; i < num_of_sockets; i++){
		client_socket = socket(AF_INET, SOCK_DGRAM,0);
		sockets[i] = client_socket;
		FD_SET(sockets[i], &fdset);
		if (client_socket == -1){
			for(i ; i >= 0 ; i--){
				close(sockets[i]);
			}
			FD_ZERO(&fdset);
			perror("failed to create socket");
			return 1;
		}

//Prepare the sockaddr_in structure
		server_addr.sin_port = htons(ports[i]); 
		addr_size = sizeof(server_addr);
		if(bind(client_socket, (struct sockaddr *) &server_addr,sizeof(server_addr))<0){
			for(i=0; i < num_of_sockets; i++){
				close(sockets[i]);
			}
			FD_ZERO(&fdset);
			perror("failed to bind");
			return 1;
		}
	}
		
	while(1){
	
		inputfd = select(FD_SETSIZE, &fdset, NULL, NULL, NULL);
		
		if(inputfd == -1){
			for(i=0; i < num_of_sockets;i++){
				close(sockets[i]);
			}
			FD_ZERO(&fdset);
			perror("select() failed");
			return 1;
		}
		else{
			for(i = 0; i < num_of_sockets; i++){
				if(FD_ISSET(sockets[i], &fdset)){
					printf("Client with port %d has sent a message\n", ports[i]);
					receiveing = recv(sockets[i], buffer, Buffer_size, 0);
					if(receiveing == -1){
						for(i=0 ; i < num_of_sockets; i++){
						close(sockets[i]);
						}
						FD_ZERO(&fdset);
						perror("recv failed");
						return 1;
					}
					memset(buffer, '\0', sizeof(buffer));
				 }
			}
		}
		for(i=0 ; i< num_of_sockets; i++){
			FD_SET(sockets[i], &fdset);
		}
	}
	for(i = 0; i < num_of_sockets; i++){
		close(sockets[i]);
	}
	FD_ZERO(&fdset);
	return 0;
}
