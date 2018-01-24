#include <muduo/net/TcpClient.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Channel.h>
#include <muduo/base/Thread.h>

#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/base/Timestamp.h>
#include <boost/bind.hpp>

#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdio.h>

muduo::net::EventLoop *g_loop;

void print(int n)
{
	std::cout<<n<<std::endl;
	printf("print id pid = %d, tid  = %d\n",
			getpid(), muduo::CurrentThread::tid() );
}

void print123()
{
	std::cout<<"afa"<<std::endl;

}

void threadFunc()
{
	__thread int  t = 0;
	g_loop->runAfter(1.0, boost::bind(print, ++t));
}

int main()
{
	muduo::net::EventLoop loop;
	g_loop = &loop;

	printf("main() pid =%d, tid= %d\n",
			getpid(), muduo::CurrentThread::tid() );

	muduo::Thread t(threadFunc);
	t.start();

	int k=3;
	muduo::Thread t1(boost::bind(print, k));
	t1.start();

	loop.loop();


	return 0;
}