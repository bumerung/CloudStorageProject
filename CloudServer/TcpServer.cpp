#include "TcpServer.h"
#include <iostream>

TcpServer::TcpServer(unsigned int port)
	: m_Acceptor(m_Ctx, tcp::endpoint(tcp::v4(), port))
{
}

TcpServer::~TcpServer()
{
	Stop();
}

bool TcpServer::Start()
{
	try {
		Accept();
		//run the asio context in a seperate thread.
		m_ThreadCtx = std::thread([this]() {m_Ctx.run(); });
		std::cout << "Server started!\n";
		return true;
	}
	catch (std::exception& e)
	{
		std::cerr << "Error while starting server : " << e.what() << "\n";
		return false;
	}
}

void TcpServer::Stop()
{
	//stop the context.
	m_Ctx.stop();

	//join the thread if it's still runnning.
	if (m_ThreadCtx.joinable()) m_ThreadCtx.join();

	std::cout << "Server stopped!\n";


}

void TcpServer::Accept()
{
	auto client = std::make_shared<TcpClient>(m_Ctx);
	m_Acceptor.async_accept(client->GetSocket(), boost::bind(&TcpServer::OnAccept, this, client, boost::asio::placeholders::error));
}

void TcpServer::Message(TcpClientPtr client, MsgPtr msg)
{
}

void TcpServer::MessageAll(MsgPtr msg, TcpClientPtr ignore)
{
}

void TcpServer::OnAccept(TcpClientPtr client, const boost::system::error_code& err)
{
	if (!err)
	{
		OnConnect(client);
		Accept();
	}
	else
	{
		std::cerr << "Error while accepting connections : " << err.message() << "\n";
	}
}


