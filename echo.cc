#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
#include <muduo/base/Thread.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/base/Logging.h>

#include <iostream>
#include <functional>

#include <vector>

using namespace std;
using namespace std::placeholders;

class EchoServer{

	public:
	EchoServer(muduo::net::EventLoop* loop,
				const muduo::net::InetAddress &listenAddr);

	void start(){
		server_.start();
	};

	void setThreadnum(int num)
	{
		server_.setThreadNum(num);
	}

	private:
		void onConnection(const muduo::net::TcpConnectionPtr &con);

		void onMessage(const muduo::net::TcpConnectionPtr &con, 
						muduo::net::Buffer* buf,
						muduo::Timestamp time);
		muduo::net::EventLoop *loop_;
		muduo::net::TcpServer server_;
};

EchoServer::EchoServer(muduo::net::EventLoop *loop,
						const muduo::net::InetAddress &listenAddr)
	:loop_(loop), server_(loop, listenAddr, "EchoServer")
{
	server_.setConnectionCallback(bind(&EchoServer::onConnection, this, _1));
	server_.setMessageCallback(bind(&EchoServer::onMessage, this, _1, _2, _3));
} 

void EchoServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
	LOG_INFO<<"EchoServer-"<<conn->peerAddress().toIpPort()<<"->"
		<<conn->localAddress().toIpPort()<<"is"
		<<(conn->connected() ? "UP":"DOWN");
}


void EchoServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
							muduo::net::Buffer *buf,
							muduo::Timestamp time)
{
	muduo::string msg(buf->retrieveAllAsString());
	LOG_INFO<<conn->name()<<"echo"<<msg.size()<<"bytes,"
			<<"data received at"<<time.toString();

	conn->send(msg);

}

int main(){
	LOG_INFO<<"pid="<<getpid();

	muduo::net::EventLoop loop;
	muduo::net::InetAddress listenAddr(2007);

	EchoServer server(&loop, listenAddr);
	server.setThreadnum(10);											//开10个线程来进行回射

	server.start();
	loop.loop();

}



