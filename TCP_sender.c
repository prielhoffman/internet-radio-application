#include<stdio.h>
#include<string.h>	
#include<sys/socket.h>
#include<arpa/inet.h>	
#include<unistd.h>

#define Buffer_size 1024	

int main(int argc , char *argv[])
{
	int data_counter = 0, num_parts = atoi(argv[3]);
	int socket_desc , new_socket , c, send_bytes = 0;
	struct sockaddr_in server , client;
	FILE *fp = NULL;
	fp = fopen(argv[4],"r");
	if (fp == NULL) {
		perror("failed to open the file");
		return 1;
	}
	char s_buffer[Buffer_size];
	memset(s_buffer, 0, sizeof(s_buffer));
	
//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		perror("Could not create socket");
		return 1;
	}
	
//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(atoi(argv[2]));
	
//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("bind failed");
		close(socket_desc);
		return 1;
	}
	puts("bind done");
	
//Listen
	if (listen(socket_desc, 1) == -1) {
		perror("listen failed");
		close(socket_desc);
		return 1;
	}
	
//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	if(new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))
		puts("Connection accepted");
	else if (new_socket < 0)
	{
		perror("accept failed");
		close(socket_desc);
		return 1;
	}
//Sending data from file
	while(data_counter < num_parts)
	{
		fread(s_buffer,1, Buffer_size, fp);
		if(send(new_socket , s_buffer , Buffer_size, 0)<0){
			perror("failed to send");
			fclose(fp);
			close(socket_desc);
			close(new_socket);
			return 1;
		}
		data_counter++;
		send_bytes = send_bytes + Buffer_size;
	}
	printf("number of bytes sent: %d", send_bytes);
//free all resources
	fclose(fp);
	close(socket_desc);
	close(new_socket);
	return 0;
}

