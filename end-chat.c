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

#define BACKLOG 10

int main(){
	struct timeval timeout;
	int result, length = 100, read_len, sockfd, clientfd;
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

	result = listen(sockfd,BACKLOG);
	if(result==-1)
	{
		perror("Error in listening! :(");
		return -1;
	}

	while(1)
	{
		printf("Waiting\n");
		clientfd = accept(sockfd,(struct sockaddr*)&cadd,&t);
		if(clientfd>0){
			FD_SET(clientfd,&outputs);
			test_in = inputs;
			test_out = outputs;
			timeout.tv_sec = 3;
			timeout.tv_usec = 0;
			result = select(clientfd+1, &test_in, &test_out, NULL, &timeout);
			
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
				if(FD_ISSET(0, &test_in))
				{
					ioctl(0, FIONREAD, &read_len);
					if(read_len > 0)
					{
						read_len = read(0, message, 1024*sizeof(char));
						message[read_len] = '\0';
						send(clientfd, message, read_len*sizeof(char), 0);
					}
				}

				if(FD_ISSET(clientfd, &test_out))
				{
					read_len = recv(clientfd, message, 1024*sizeof(char), 0);
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
	}
	return 0;	 
}