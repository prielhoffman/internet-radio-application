#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define Buffer_size 1024

int main(int argc , char *argv[])
{
	char buffer[Buffer_size];
	memset(buffer, 0, sizeof(buffer));
	struct sockaddr_in servaddr = {0};
	
	int num_parts = atoi(argv[3]), bind_flag, recv_flag, recv_bytes;
	int parts_counter = 0;

//Create socket	
	int socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
	if(socket_desc == -1)
	{
		perror("failed to create socket");
		return 1;
	}
	
//Prepare the sockaddr_in structure
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	
//Bind
	bind_flag = bind(socket_desc, (const struct sockaddr *)&servaddr, sizeof(servaddr));
	if(bind_flag == -1)
	{
		perror("failed to bind");
		close(socket_desc);
		return 1;
	}
	socklen_t len = 0;
	
//Receive data
	while(parts_counter < num_parts){
		recv_flag = recvfrom(socket_desc, (char *)buffer, Buffer_size, 0,0, &len);
		if(recv_flag == -1)
			{
				perror("recv failed");
				close(socket_desc);
				return 1;
			}
		puts(buffer);
		parts_counter++;
		recv_bytes = recv_bytes + recv_flag;
	}
	
	printf("the number of bytes we needed to receive: %d \n" ,num_parts*Buffer_size);
	printf("the actual amount received: %d \n" ,parts_counter*Buffer_size);

//free all resources
	close(socket_desc);
    return 0;
}
