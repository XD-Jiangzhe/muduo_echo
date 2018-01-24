#include <sys/timerfd.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Channel.h>
#include <muduo/net/EventLoopThread.h>

#include <muduo/base/Thread.h>
#include <muduo/base/CurrentThread.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>

#include <unistd.h>
#include <stdio.h>
#include <memory>
#include <sys/types.h>
#include <iostream>



using namespace muduo::net;


void runinThread()
{
	printf("runinthread() pid= %d, tid=%d\n",
			getpid(), muduo::CurrentThread::tid() );
}

int main()
{
	printf("main() pid = %d, tid = %d\n",
			getpid(), muduo::CurrentThread::tid() );

	EventLoopThread loopthread;
	EventLoop *loop = loopthread.startLoop();
	
	loop->runInLoop(runinThread);				
	//将runinthread放到创建的IO线程中去执行
	sleep(1);

	loop->runAfter(2, runinThread);			
	//与上面同理，runInloop跨线程调用

	sleep(3);

	loop->quit();

	printf("exit main\n");

	return 0;
}
