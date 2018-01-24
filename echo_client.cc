/*
这是一个基于muduo实现的echo客户端，当connection连接建立时，输入用户想要发送给tcpsever的数据，
tcpserver将收到的数据返回给echo_client,触发了echo_client的可读事件，将buf中的数据读出，回显到stdout中


*/

#include <muduo/net/TcpClient.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/Channel.h>


#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/base/Timestamp.h>

#include <memory>
#include <unistd.h>
#include <stdio.h>
#include <iostream>

using namespace muduo::net;
using namespace std;
using namespace std::placeholders;

#define MAXLINE 4096

char buffer[MAXLINE];
char writebuf[MAXLINE];


class EchoClient{

	public:

	EchoClient(EventLoop *loop, InetAddress addr);
	
	void connect(){tcl_.connect();}

	 TcpConnectionPtr getconnect(){return tcl_.connection();}

	private:

		EventLoop *loop_;
		TcpClient tcl_;
		void onMessage(const muduo::net::TcpConnectionPtr &con, 
						muduo::net::Buffer* buf,
						muduo::Timestamp time);
		void onConnection(const muduo::net::TcpConnectionPtr &con);

};

EchoClient::EchoClient(EventLoop *loop, InetAddress addr):loop_(loop), tcl_(loop, addr, "client")
{
	tcl_.setMessageCallback(bind(&EchoClient::onMessage, this, _1, _2, _3));			//消息到来了
	tcl_.setConnectionCallback(bind(&EchoClient::onConnection, this, _1));

}

void EchoClient::onMessage(const muduo::net::TcpConnectionPtr &con, 
							muduo::net::Buffer* buf,
							muduo::Timestamp time)
{
	bzero(writebuf, MAXLINE);
	muduo::string getmsg(buf->retrieveAllAsString()); 

	strcpy(writebuf, getmsg.c_str());

	::write(fileno(stdout), writebuf, strlen(writebuf));

	bzero(buffer, MAXLINE);

	if(fgets(buffer, MAXLINE, stdin))
	{
		
		muduo::string msg(buffer);
		con->send(msg);


	}
}

void EchoClient::onConnection(const muduo::net::TcpConnectionPtr &con)
{
	bzero(buffer, MAXLINE);

	if(fgets(buffer, MAXLINE, stdin))
	{		
		muduo::string msg(buffer);
		con->send(msg);


	}

}

int main()
{
	muduo::net::EventLoop loop;

	muduo::net::InetAddress inaddr(2007);
	EchoClient tcl(&loop, inaddr);

	TcpConnectionPtr tptr = tcl.getconnect();

	tcl.connect();
	loop.loop();



	return 0;
}
