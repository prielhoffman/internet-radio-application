#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define Buffer_size 1024

int main(int argc, char *argv[]) {

		char* user_name = strdup(argv[3]);
		char* message = strdup(argv[4]);
		int socket_desc, recv_flag = 0;
		char c_buffer[Buffer_size];
		memset(c_buffer, 0, sizeof(c_buffer));
		struct sockaddr_in servaddr;

		fd_set fdset, rdset;
		FD_ZERO(&fdset);
		FD_ZERO(&rdset);

//Create socket
		socket_desc = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_desc < 0) {
			perror("socket failed");
			free(user_name);
			free(message);
			return 1;
		}

//Prepare the sockaddr_in structure
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(atoi(argv[2])); 
		servaddr.sin_addr.s_addr = inet_addr(argv[1]);  
		memset(servaddr.sin_zero, 0, sizeof(servaddr.sin_zero));

//Connect to the server
		if (connect(socket_desc, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
			perror("connect failed");
			close(socket_desc);
			free(user_name);
			free(message);
			return 1;
		}
		printf("Connected to the server\n");

//set the chat message

		sprintf(c_buffer, "%6s%d%d%s\n", user_name, 0, 0, message); 		
		c_buffer[6] = (char)(sizeof(message));

//Sending data
		if (send(socket_desc, c_buffer, Buffer_size, 0) < 0) {
			perror("failed to send");
			close(socket_desc);
			free(user_name);
			free(message);
			return 1;
		}

		FD_SET(socket_desc, &fdset);
		FD_SET(fileno(stdin), &fdset);

		while (1) {
			
			rdset = fdset;
			if (select(FD_SETSIZE, &rdset, NULL, NULL, NULL) < 0) {
				perror("select() failed");
				close(socket_desc);
				free(user_name);
				free(message);
				return 1;
			}

			if (FD_ISSET(fileno(stdin), &rdset)) 
			{
				printf("%s quiting the chat\n", user_name);
				close(socket_desc);
				free(user_name);
				free(message);
				return 0;
			}
			else {     
				memset(c_buffer, 0, Buffer_size);
				if ((recv_flag = recv(socket_desc, c_buffer, Buffer_size, 0)) > 0) {
					c_buffer[recv_flag] = '\0';
					printf("%s\n", c_buffer);
				}
				if (recv_flag < 0) {
					perror("recv() failed");
					close(socket_desc);
					free(user_name);
					free(message);
					return 1;
				}
				if (recv_flag == 0) {
					printf("server close the connection\n");
					free(user_name);
					free(message);
					return 0;
				}

			}
		}
}
