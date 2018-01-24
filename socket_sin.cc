#include <netinet/in.h>
#include <iostream>
#include <algorithm>
#include <assert.h>
#include <stddef.h>
#include <boost/static_assert.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
using namespace std;

BOOST_STATIC_ASSERT(offsetof(sockaddr_in, sin_family) == 0);
static_assert(offsetof(sockaddr_in, sin_family) == 0,"hahah");

namespace jz
{
	int connect(int sockfd, const struct sockaddr *seraddr,socklen_t addrlen )
	{
		return ::connect(sockfd, seraddr, sizeof(*seraddr));
	}
}

char * Fgets(char *line, int len, FILE* fp)
{
	memset(line, 0, len);
	return fgets(line, len, fp);
}


#define MAXLINE 4096

#define PORT 2007

void client_send_get(FILE *fp ,int fd)
{
	char sendmsg[MAXLINE],getmsg[MAXLINE];
	int n;	
	while(Fgets(sendmsg, MAXLINE, fp)!=NULL)
	{
		int len = 0;
		while(len !=strlen(sendmsg) )
		{
			cout<<"here"<<endl;
			len += write(fd, &sendmsg[len], strlen(sendmsg)-len);
		}

		int lenw = 0;
		memset(getmsg, 0, MAXLINE);
		if( read(fd, getmsg, MAXLINE)==0)
			return ;
		fputs(getmsg, stdout);
	}
}

void 
client_select(FILE *fp, int fd)
{
	int fp_fd=fileno(fp);
	fd_set fs,temp;
	char buf[MAXLINE];

	FD_ZERO(&fs);
	FD_SET(fp_fd, &fs);
	FD_SET(fd, &fs);

	int n, stdineof=0;

	int max1 =max(fp_fd, fd);
	for(; ;)
	{
		temp = fs;
		select(max1+1, &temp, NULL, NULL, NULL);
		
		if(FD_ISSET(fp_fd, &temp))
		{
			memset(buf, 0, MAXLINE);
			if((n = read(fp_fd, buf, MAXLINE))==0) //读到终端发出的EOF标志
			{
				stdineof = 1;						//终端关闭置1
				shutdown(fd, SHUT_WR);				//关闭tcp的写端，此时已经没有数据可写了
				FD_CLR(fp_fd, &fs );				
			}
			write(fd, buf, n);
		}

		if(FD_ISSET(fd, &temp))
		{
			memset(buf, 0, MAXLINE);
			if((n = read(fd, buf, MAXLINE))==0) //读到对方发出的FIN标志了
			{
				if(stdineof == 1)				//如果终端已经关闭了，此时正常退出客户端程序
					return;
				else 
				{
					cout<<"server is down"<<endl;	//如果终端没有关闭，则此时服务器关闭了
					close(fp_fd);
					return;
				}
			}
			write(fileno(stdout), buf, n);				
		}
	}
}


int main()
{
	char a[] = "127.0.0.1";
	char buf[100];

	cout<<offsetof(sockaddr_in, sin_family)<<endl;

	assert(offsetof(sockaddr_in, sin_family) == 0);

/*	cout<<"hi"<<endl;*/
	cout<<offsetof(sockaddr_in, sin_port)<<endl;
	cout<<offsetof(sockaddr_in, sin_addr)<<endl;

	int fd= socket(AF_INET, SOCK_STREAM, 0);
	assert(fd>0);

	sockaddr_in sock;

	sock.sin_family = AF_INET;
	sock.sin_port = htons(PORT);
	inet_pton(AF_INET, a, buf);

	if(jz::connect(fd, reinterpret_cast<sockaddr*> (&sock)/*(struct sockaddr*)&sock*/, sizeof(sock))<0)
		cout<<"connect fail"<<endl;
	else 
		cout<<"connect	success"<<endl;

	/*client_send_get(stdin, fd);*/
	client_select(stdin, fd);

	return 0;
}