#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


#define MAXLINE 4096
#define PORT 	2007

using namespace std;


int fd;

void handler(int signo)
{
	int flag = 0;
	switch(signo)
	{
		case(SIGINT):
			close(fd);
			flag =1;
			break;
		
		case(SIGQUIT):	
			close(fd);
			flag =1;
			break;
		default :
			break;
	}

	if (flag ==1)
		exit(-1);
}

int main()
{

	struct sockaddr_in sock,clisock;
	socklen_t clilen;
	sock.sin_family = AF_INET;
	sock.sin_port = htons(PORT);
	sock.sin_addr.s_addr = htonl(INADDR_ANY);
	
	fd = socket(AF_INET, SOCK_STREAM, 0);

	if(bind(fd, (sockaddr *)&sock, sizeof(sock))<0)
		cout<<"bind fd fail"<<endl;

	struct sigaction sd;
	sigset_t oldmask,newmask;

	sigemptyset(&newmask);
	sigemptyset(&oldmask);

	struct sigaction sg;
	sg.sa_handler= handler;


	listen(fd, 10);

	fd_set now,temp;
	FD_ZERO(&now);
	FD_SET(fd, &now);
	
	int max = fd;
	int clients[FD_SETSIZE];

	for(int c= 0;c<FD_SETSIZE; c++)
		clients[c]=-1;

	int max_cli = -1;

	int newfd,i,n;
	char buf[MAXLINE];

	cout<<getpid()<<endl;

	for(;;)
	{
		temp = now;

		int readn = select(max+1, &temp, NULL, NULL, NULL);
		if(FD_ISSET(fd, &temp))
		{
			newfd = accept(fd, (struct sockaddr*)&clisock, &clilen); 
			cout<<"accept a connection"<<endl;

			for(i =0; i<FD_SETSIZE;i++)
			{
				if(clients[i]<0)
				{
					clients[i] = newfd;
					break;
				}
			}
			FD_SET(newfd, &now);
			if(newfd > max )
				max = newfd;
			if(i > max_cli)
				max_cli = i;
		}

		for(i=0; i<=max_cli; i++)
		{
			if(clients[i]<0)
				continue;
			
			if(FD_ISSET(clients[i], &temp))
			{
				if((n = read(clients[i], buf, MAXLINE))<0)
					cout<<"read error"<<endl;
				else if(n==0)
				{
					close(clients[i]);
					cout<<"a connection is shutdown"<<endl;
					FD_CLR(clients[i], &now);
					clients[i] = -1;
					continue;
				}
				//cout<<n<<endl;

				int len = 0;
				while(len !=  n )				//writen 我自己的写法。。。
				{
					len += write(clients[i], &buf[len], n-len);
				}

			}
		}
	}
	return 0;
}