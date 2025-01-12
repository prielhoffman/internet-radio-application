#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define Buffer_size 1024

//assist function
int Welcome(int num_of_port, int max_clients) {
	int serv_socket, opt = 1;
	struct sockaddr_in servaddr;

//creating socket
	serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_socket < 0) {
		perror("socket failed");
		return -1;
	}
	if (setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("socket option failed");
		close(serv_socket);
		return -1;
	}

//Prepare the sockaddr_in structure
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(num_of_port);
	servaddr.sin_addr.s_addr = INADDR_ANY;

//bind
	if ((bind(serv_socket, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
		perror("binding failed");
		close(serv_socket);
		return -1;
	}

	printf("Server is listen to port %d and wait for new client...\n", num_of_port);

//listen
	if ((listen(serv_socket, max_clients)) < 0) {
		perror("listen() failed");
		close(serv_socket);
		return -1;
	}
	return serv_socket;
}

int main(int argc, char *argv[]) {

		int max_clients = (int)atoi(argv[2]), wel_Socket, message_length, i = 0, j = 0, recv_flag = 0, num_of_clients = 0, index = 0,
			*sockets, is_full = 0, num_of_port = atoi(argv[1]);
		
		char s_buffer[Buffer_size];
		char **user_name, *message = NULL;

		struct sockaddr_in *client_addr;
		struct sockaddr_in wel_addr;
		memset(wel_addr.sin_zero, 0, sizeof wel_addr.sin_zero);

		socklen_t welcome_size, client_size;
		fd_set fd_socket, fdset;

//allocating memory 		
		client_addr = (struct sockaddr_in*)malloc((max_clients * (sizeof(struct sockaddr_in))));
		if (client_addr == NULL) {
			perror("malloc() failed");
			return 1;
		}
		
		sockets = (int*)malloc(max_clients*(sizeof(int)));
		if (sockets == NULL) {
			perror("malloc() failed");
			free(client_addr);
			return 1;
		}
		
		for (i = 0; i < max_clients; i++) {
			sockets[i] = 0;
		}

		message = (char*)malloc((256 + 1) * sizeof(char));
		if (message == NULL) {
			perror("malloc() failed");
			free(client_addr);
			free(sockets);
			return 1;
		}
		
		
		user_name = (char**)malloc(max_clients * sizeof(char *));
		if (user_name == NULL) {
			perror("malloc() failed");
			free(client_addr);
			free(sockets);
			free(message);
			return 1;
		}

		for (i = 0; i < max_clients; i++) {
			user_name[i] = (char*)malloc(7 * sizeof(char));
			if (user_name[i] == NULL) {
				for (j = 0; j < i; j++) {
					free(user_name[j]);
				}
				free(client_addr);
				free(sockets);
				free(message);
				free(user_name);
			}
		}

//welcome done in assist function
		wel_Socket = Welcome(num_of_port, max_clients);
		if (wel_Socket < 0) {
			perror("Welcome failed");
			for (j = 0; j < max_clients; j++) {
				free(user_name[j]);
			}
			free(client_addr);
			free(sockets);
			free(message);
			free(user_name);
			return 1;
		}

		FD_ZERO(&fd_socket);
		FD_SET(wel_Socket, &fd_socket);
		FD_SET(fileno(stdin), &fd_socket);

		while (1) {
			
			fdset = fd_socket;
			
			if (select(FD_SETSIZE, &fdset, NULL, NULL, NULL) < 0) {
				perror("select() failed");
				for (j = 0; j < max_clients; j++) {
					free(user_name[j]);
				}
				free(client_addr);
				free(sockets);
				free(message);
				free(user_name);
				return 1;				
			}

			if (FD_ISSET(fileno(stdin), &fdset)) {  
				printf("closing server\n");
				for (j = 0; j < max_clients; j++) {
					free(user_name[j]);
				}
				free(client_addr);
				free(sockets);
				free(message);
				free(user_name);
				return 0;				
			}

			for (i = 0; i < FD_SETSIZE; i++) {
				if (FD_ISSET(i, &fdset)) {
					if (i == wel_Socket) {  
						if ((num_of_clients == max_clients) && !is_full) {  
							close(wel_Socket);
							FD_CLR(wel_Socket, &fd_socket);
							is_full = 1;  
						}
						if (is_full == 0) {   
							index = 0;
							while (index < max_clients && sockets[index] != 0) 
							++index; 
							client_size = sizeof(client_addr[index]);
							memset(&client_addr[index], 0, client_size);
							if ((sockets[index] = accept(wel_Socket, (struct sockaddr*)&client_addr[index], &client_size)) < 0) 
							{
								perror("accept() failed");
								for (j = 0; j < num_of_clients; j++) {
									if (sockets[j] != 0) {
										close(sockets[j]);
									}
									free(user_name[j]);
								}
								free(client_addr);
								free(sockets);
								free(message);
								free(user_name);
								return 1;
							}
							FD_SET((sockets[index]), &fd_socket);
							num_of_clients++;
						}
					}
					else {  
						if ((recv_flag = recv(i, s_buffer, Buffer_size, 0)) < 0) {
							perror("recv() failed");
							for (j = 0; j < num_of_clients; j++) {
								if (sockets[j] != 0) {
									close(sockets[j]);
								}
								free(user_name[j]);
							}
							free(client_addr);
							free(sockets);
							free(message);
							free(user_name);
							return 1;
						}
						else if (recv_flag == 0) { 
							index = 0;
							while (index < num_of_clients && sockets[index] != i) {
								++index;
							}
							printf("%s has left the server.\n", user_name[index]);
							close(sockets[index]);
							sockets[index] = 0;
							num_of_clients--;
							FD_CLR(i, &fd_socket);
							if ((num_of_clients == (max_clients - 1)) && is_full) { 
								wel_Socket = Welcome(num_of_port, max_clients);
								if (wel_Socket < 1) {
									for (j = 0; j < num_of_clients; j++) {
										if (sockets[j] != 0) {
											close(sockets[j]);
										}
										free(user_name[j]);
									}
									free(client_addr);
									free(sockets);
									free(message);
									free(user_name);
									return 1;
								}

								FD_SET(wel_Socket, &fd_socket);
								is_full = 0;
							}
						}
						else {    
							index = 0;
							while (index < num_of_clients && sockets[index] != i)
							{
								++index;
							}
							sprintf(user_name[index], "%6s", s_buffer);
							user_name[index][6] = '\0';
							message_length = s_buffer[6];
							memset(message, 0, 256);
							for (j = 0; j < message_length; j++) {
								message[j] = s_buffer[8 + j];
							}
							message[message_length] = '\0';
							printf("%s has connected to the server\n", user_name[index]);
							memset(s_buffer, 0, Buffer_size);
							sprintf(s_buffer, "User:%s has said:%s\n", user_name[index], message);
							for (j = 0; j < max_clients; j++) {
								if ((sockets[j] != i) && (sockets[j] != 0)) {  
									if (send(sockets[j], s_buffer, Buffer_size, 0) < 0) {
										perror("send() failed");
										for (j = 0; j < num_of_clients; j++) {
											if (sockets[j] != 0) {
												close(sockets[j]);
											}
											free(user_name[j]);
										}
										free(client_addr);
										free(sockets);
										free(message);
										free(user_name);
										return 1;
									}
								}
							}
						}
					}
				}
			}
		}

}


