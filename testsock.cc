#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <iostream>
#include <memory>

using namespace std;

int main()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);

	char buf[] ="127.0.0.1";
	struct sockaddr_in sa;
	socklen_t 	len;

	sa.sin_family = AF_INET;
	sa.sin_port   = htobe16(1988);
	unsigned int a; 
	inet_pton(AF_INET, buf, &a);

	bind(fd, reinterpret_cast<sockaddr*>(&sa), sizeof(struct sockaddr_in));

	struct sockaddr_in socknew;
	socklen_t socknew_len = static_cast<socklen_t>(sizeof(struct sockaddr_in));

	getsockname(fd, reinterpret_cast<sockaddr*>(&socknew), &socknew_len);

	cout<<be16toh(socknew.sin_port)<<endl;
}
