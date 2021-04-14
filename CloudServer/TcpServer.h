#pragma once
#include <boost/asio.hpp>
#include <string>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <functional>
#include <boost/bind.hpp>
#include <thread>
#include "tsqueue.h"
#include "TcpClient.h"


class TcpServer
{
public:
	TcpServer( unsigned int port);
	virtual ~TcpServer();

public:
	bool Start();
	void Stop();

private:
	void Accept();


public:
	virtual bool OnConnect(TcpClientPtr client) = 0;
	virtual void OnDisconnect(TcpClientPtr client) = 0;
	virtual void OnMessage(TcpClientPtr client, MsgPtr msg) = 0;
public:
	void Message(TcpClientPtr client, MsgPtr msg);
	void MessageAll(MsgPtr msg, TcpClientPtr ignore = nullptr);
public:
	void Update(bool wait = false);
private:
	void OnAccept(TcpClientPtr client, const boost::system::error_code& err);
private:
	boost::asio::io_context m_Ctx;
	tcp::acceptor m_Acceptor;

	std::thread m_ThreadCtx;

	tsqueue<std::pair<TcpClientPtr,MsgPtr>> m_MsgQueue;
	std::deque<TcpClientPtr> m_Clients;

	unsigned int m_IdCounter;
};
