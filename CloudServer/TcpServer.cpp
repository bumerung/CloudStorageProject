#include "TcpServer.h"
#include <iostream>
#include "Log.h"


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
		SERVER_TRACE("Server started!");
		return true;
	}
	catch (std::exception& e)
	{
		SERVER_ERROR("Failed to start server: {}.", e.what());
		return false;
	}
}

void TcpServer::Stop()
{
	//stop the context.
	m_Ctx.stop();

	//join the thread if it's still runnning.
	if (m_ThreadCtx.joinable()) m_ThreadCtx.join();

	SERVER_TRACE("Server stopped!");


}

void TcpServer::Accept()
{
	
	auto client = std::make_shared<TcpClient>(m_Ctx, m_MsgQueue);
	m_Acceptor.async_accept(client->GetSocket(), boost::bind(&TcpServer::OnAccept, this, client, boost::asio::placeholders::error));
}

void TcpServer::Message(TcpClientPtr client, MsgPtr msg)
{
	if (client && client->IsConnected())
	{
		client->Send(msg);
	}
	else
	{
		//triger disconnection event.
		OnDisconnect(client);

		//invalidate the client to be removed.
		client.reset();

		//finally remove the client from the container.
		m_Clients.erase(
			std::remove(m_Clients.begin(), m_Clients.end(), client), m_Clients.end());
	}
}

void TcpServer::MessageAll(MsgPtr msg, TcpClientPtr ignore)
{
	bool hasInvalidClients = false;
	for (auto& client : m_Clients)
	{
		if (client && client->IsConnected())
		{
			if (client != ignore)
				client->Send(msg);
		}
		else
		{
			//trigger on disconnection event.
			OnDisconnect(client);
			//invalidate client to be erased later.
			client.reset();

			//indicate that invalid clients exist.
			hasInvalidClients = true;
		}
	}

	//remove all invalid clients
	if (hasInvalidClients)
		m_Clients.erase(std::remove(m_Clients.begin(), m_Clients.end(), nullptr), m_Clients.end());

}

void TcpServer::Update(bool wait)
{
	if (wait) m_MsgQueue.wait();

	while (!m_MsgQueue.empty())
	{
		//get the message and remove it from the queue.
		auto pair = m_MsgQueue.pop_front();

		//trigger the message received event to be processed.
		OnMessage(pair.first, pair.second);
	}
}

void TcpServer::OnAccept(TcpClientPtr client, const boost::system::error_code& err)
{
	if (!err)
	{
		if (OnConnect(client) && client->Connect(m_IdCounter++))
		{
			m_Clients.push_back(std::move(client));
			SERVER_TRACE("Client approved Id: {}.", m_Clients.back()->GetId());
		}
		else
		{
			SERVER_TRACE("Connection denied from {}.", client->GetSocket().remote_endpoint().address().to_string().c_str());
		}
		Accept();
	}
	else
	{
		SERVER_ERROR("Error while accepting connections : {}.", err.message().c_str());
	}
}


