#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<fcntl.h>
#include<sys/select.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

typedef struct m{
	int type, length;
	char fname[20];
	char buffer[1024];
}message;

#define BACKLOG 10

int main()
{
	struct timeval timeout;
	int result, length = 100, read_len, sockfd, u1, u2, maxfd;
	fd_set inputs, outputs, test_in, test_out;
	socklen_t t;
	message msg;

	FD_ZERO(&inputs);
	FD_ZERO(&outputs);

	FD_SET(0, &inputs);

	struct sockaddr_in sadd, user1, user2;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	sadd.sin_family = AF_INET;
	sadd.sin_port = htons(9000);
	sadd.sin_addr.s_addr = inet_addr("127.0.0.1");
	t = sizeof(sadd);

	result = bind(sockfd, (struct sockaddr*)&sadd, sizeof(sadd)); 
	if(result==-1)
	{
		perror("Error in binding! :(");
		return -1;
	}

	result = listen(sockfd,BACKLOG);
	if(result==-1)
	{
		perror("Error in listening! :(");
		return -1;
	}
//The server must establish connection with both the users before it starts receiving messages. 
	u1 = accept(sockfd,(struct sockaddr*)&user1,&t);
	strcpy(msg.buffer, "Waiting for the other user to connect...");
	msg.type = 1;
	msg.length = strlen(msg.buffer);
	write(u1, &msg, sizeof(message));
	
	u2 = accept(sockfd,(struct sockaddr*)&user2,&t);
	
	strcpy(msg.buffer, "You are now connected!");
	msg.type = 1;
	msg.length = strlen(msg.buffer);
	write(u1, &msg, sizeof(message));
	write(u2, &msg, sizeof(message));
	
	FD_SET(u1, &inputs);
	FD_SET(u2, &inputs);
	
	maxfd = u1 > u2 ? u1: u2;
	
	while(1)
	{
		test_in = inputs;
		test_out = outputs;
		timeout.tv_sec = 30;
		timeout.tv_usec = 0;
		result = select(maxfd+1, &test_in, NULL, NULL, &timeout);

		if(result<0)
		{
			perror("Error! :'(\n");
			return -1;
		}
		else if(result == 0)
			continue;
		else
		{
			if(FD_ISSET(u1, &test_in))
			{
				read_len = read(u1, &msg, sizeof(message));
				if(read_len > 0)
				{
					write(u2, &msg, sizeof(message));
				}
			}

			if(FD_ISSET(u2, &test_in))
			{
				read_len = read(u2, &msg, sizeof(message));
				if(read_len > 0)
				{	
					write(u1, &msg, sizeof(message));
				}
			}
		}
	}
}