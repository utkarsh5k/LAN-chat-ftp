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


int main()
{
	struct timeval timeout;
	int result, length = 100, read_len, sockfd;
	fd_set inputs, outputs, test_in, test_out;
	socklen_t t;
	char message[1024];

	FD_ZERO(&inputs);
	FD_ZERO(&outputs);

	FD_SET(0, &inputs);

	struct sockaddr_in sadd, cadd;
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

	result = connect(sockfd, (struct sockaddr*)&cadd, t);
	FD_SET(sockfd, &outputs);
	while(1)
	{
		test_in = inputs;
		test_out = outputs;
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		result = select(sockfd+1, &test_in, &test_out, NULL, &timeout);
		
		if(result==0)
		{
			printf("Timeout! :(\n");
			continue;
		}

		else if(result<0)
		{
			perror("Error! :'(\n");
			return -1;
		}

		else
		{
			if(FD_ISSET(0, &inputs))
			{
				ioctl(0, FIONREAD, &read_len);
				if(read_len > 0)
				{
					read_len = read(0, message, 1024*sizeof(char));
					message[read_len] = '\0';
					send(sockfd, message, 1024*sizeof(char), 0);
				}
			}

			if(FD_ISSET(sockfd, &outputs))
			{
				read_len = recv(sockfd, message, 1024*sizeof(char), 0);
				if(read_len==-1)
				{
					perror("Error in receiving!");
					exit(0);
				}
				else if(read_len > 0)
				{	
					message[read_len] = '\0';
					printf("User2: %s\n", message);
				}
			}
		}
	}
	return 0;	 
}