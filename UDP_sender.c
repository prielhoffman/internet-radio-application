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
	
	int num_parts = atoi(argv[3]), parts_counter = 0, socket_desc, send_flag, send_bytes;
	char s_buffer[Buffer_size];
	memset(s_buffer, 0, sizeof(s_buffer));
	struct sockaddr_in servaddr = {0};
	FILE *fp = NULL;
	fp = fopen(argv[4],"r");
	if (fp == NULL) {
		perror("failed to open the file");
		return 1;
	}
	
//Create socket
	socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_desc == -1)
	{
		perror("Could not create socket");
		return 1;
	}

//Prepare the sockaddr_in structure
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);

//Sending data from file
	while(parts_counter < num_parts)
	{
		fread(s_buffer, 1, Buffer_size, fp);	
		send_flag = sendto(socket_desc, (const char *)s_buffer, sizeof(s_buffer),
			0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
		if(send_flag ==-1)
		{
			perror("failed to send");
			fclose(fp);
			close(socket_desc);
			return 1;
		}
		parts_counter++;
		send_bytes = send_bytes + send_flag;
	}

//free all resources
	fclose(fp);
	close(socket_desc);
	return 0;
}
