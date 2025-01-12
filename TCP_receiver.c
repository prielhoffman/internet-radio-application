#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define Buffer_size 1024

int main(int argc , char *argv[])
{ 

	int socket_desc, data_counter = 0, num_parts = atoi(argv[3]), recv_bytes = 0, recv_flag;
	struct sockaddr_in servaddr;
	char r_buffer[Buffer_size];
	memset(r_buffer, 0, sizeof(r_buffer));

//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc == -1)
	{
		perror("Could not create socket");
		return 1;
	}
	
//Prepare the sockaddr_in structure
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	
//Connect to the server
	if (connect(socket_desc , (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect error");
		close(socket_desc);
		return 1;
	}
	puts("Connected to the server");
	
//Receive a reply from the server
	while(data_counter < num_parts){
		recv_flag = recv(socket_desc, r_buffer, Buffer_size, MSG_WAITALL);
		if(recv_flag == -1)
		{
			perror("recv failed");
			close(socket_desc);
			return 1;
		}
		data_counter++;
		puts(r_buffer);
		recv_bytes = recv_bytes + recv_flag;
	}
	printf("the number of bytes we needed to receive: %d \n" ,num_parts*Buffer_size);
	printf("the actual amount received: %d \n" , recv_bytes);

//free all resources
	close(socket_desc);
	return 0;
}
	


