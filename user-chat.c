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
#include<string.h>


typedef struct m{
	int type, length;
	char fname[20];
	char buffer[1024];
}message;

int main()
{
	struct timeval timeout;
	int result, length = 100, read_len, sockfd, index, fd;
	fd_set inputs, outputs, test_in, test_out;
	socklen_t t;
	message msg;

	FD_ZERO(&inputs);
	FD_ZERO(&outputs);

	FD_SET(0, &inputs);

	struct sockaddr_in sadd, cadd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	sadd.sin_family = AF_INET;
	sadd.sin_port = htons(9000);
	sadd.sin_addr.s_addr = inet_addr("127.0.0.1");
	t = sizeof(sadd);
	
	
	result = connect(sockfd, (struct sockaddr*)&sadd, t);
	FD_SET(sockfd, &inputs);
	while(1)
	{
		test_in = inputs;
		timeout.tv_sec = 1500;
		timeout.tv_usec = 0;
		result = select(sockfd+1, &test_in, NULL, NULL, &timeout);
		
		if(result==0)
		{
			//printf("Timeout! :(\n");
			continue;
		}

		else if(result<0)
		{
			perror("Error! :'(\n");
			return -1;
		}

		else
		{
			if(FD_ISSET(0,&test_in))
			{
				ioctl(0, FIONREAD, &read_len);
				msg.length = read(0, msg.buffer, 1024*sizeof(char));
				msg.type = 1;
				if(msg.length <= 0) 
					break;
				msg.buffer[msg.length-1] = '\0';
				if(strstr(msg.buffer, "::SENDFILE"))
				{
					msg.type = 2;
					strcpy(msg.fname, msg.buffer+11);
					printf("Sending the file %s...\n", msg.fname);
					fd = open(msg.fname, 0666);
					if(fd==-1) 
						printf("No such file\n");
					msg.length = read(fd, msg.buffer, 1024*sizeof(char));
					close(fd);
				}
				if(msg.length > 0)
				{
					send(sockfd, &msg, sizeof(message), 0);
				}
			}
	
			if(FD_ISSET(sockfd, &test_in))
			{	
				read_len = read(sockfd, &msg, sizeof(message));
				if(msg.type == 2)
				{
					fd = creat("blabla", 0666);
					write(fd, msg.buffer, msg.length);
					close(fd);
					printf("You have received a file %s. It is now saved on your device.\n", msg.fname);
				}
				else if(read_len>0)
				{
					msg.buffer[msg.length] = '\0';
					printf(">> %s\n", message.buffer);
				}
			}
		}
	}
	return 0;	 
}
