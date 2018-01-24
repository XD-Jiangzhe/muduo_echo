#include <sys/timerfd.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Channel.h>

#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <memory>
#include <sys/types.h>
#include <iostream>

using namespace muduo;
using namespace muduo::net;

int timerfd;

void timeout(Timestamp arrivaltime)
{
	printf("this is timeout \n");
/*	uint64_t tag;
	::read(timerfd, &tag, sizeof(tag));*/

	int tag;
	int  n=0;
	while( ( n = ::read(timerfd, &tag, sizeof(tag)))>0 )
	{
		std::cout<<n<<std::endl;
		std::cout<<" here"<<std::endl;
	}

}

int main()
{
	muduo::net::EventLoop loop;
	timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC|TFD_NONBLOCK);
	
	muduo::net::Channel channel(&loop, timerfd);
	channel.setReadCallback(std::bind(timeout,std::placeholders::_1));
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));
	howlong.it_value.tv_sec = 1;

	::timerfd_settime(timerfd, 0, &howlong, NULL);

	loop.loop();

	::close(timerfd);
	return 0;
}