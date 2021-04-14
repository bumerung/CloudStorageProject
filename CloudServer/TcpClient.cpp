#include "TcpClient.h"
#include <iostream>
#include "MsgCryptography.h"
#include "MsgDebugString.h"
#include "MsgBuilder.h"
#include "Log.h"

TcpClient::TcpClient(boost::asio::io_context& ctx, tsqueue<std::pair<TcpClientPtr, MsgPtr>>& inQ)
	: m_Ctx(ctx), m_Socket(ctx), m_MsgsIn(inQ)
{
	memset(m_Body, 0, MAX_BUFFER_SIZE);
}

TcpClient::~TcpClient()
{
}



bool TcpClient::Connect(unsigned int id)
{
	if (m_Socket.is_open())
	{
		m_Id = id;
		BeginReceive();
		return true;
	}
	return false;
}

bool TcpClient::Send(MsgPtr msg)
{

	boost::asio::post(m_Ctx, [this, msg]() {

		bool isWriting = !m_MsgsOut.empty();

		m_MsgsOut.push_back(msg);
		if (!isWriting)
			WriteMsg();

		auto length = msg->Header.Length;
		auto msgBuf = reinterpret_cast<char*>(msg->GetBuffer());
		MsgCryptography::Encrypt(msgBuf, length);
		return m_Socket.send(boost::asio::buffer(msgBuf, length)) >= length;
	});

	return true;

}

void TcpClient::Disconnect()
{
	if(IsConnected())
		boost::asio::post(m_Ctx, [this]() { m_Socket.close(); });
}

unsigned int TcpClient::GetId() const
{
	return m_Id;
}


bool TcpClient::IsConnected() const
{
	return m_Socket.is_open();
}

tcp::socket& TcpClient::GetSocket()
{
	return m_Socket;
}
void TcpClient::BeginReceive()
{
	/// <summary>
	/// Begin reading a new message by first reading the header.
	/// </summary>
	m_Socket.async_read_some(boost::asio::buffer(&m_Header, sizeof(MsgHeader)),
		boost::bind(&TcpClient::OnReadHeader, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void TcpClient::WriteMsg()
{
	auto msg = m_MsgsOut.front();
	auto length = msg->Header.Length;
	auto msgBuf = msg->GetBuffer();
	MsgCryptography::Encrypt(reinterpret_cast<char*>(msgBuf), length);

	boost::asio::async_write(m_Socket, boost::asio::buffer(msgBuf, length), [this](std::error_code ec, std::size_t length){
		if (!ec)
		{
			m_MsgsOut.pop_front();
			if (!m_MsgsOut.empty())
				WriteMsg();
		}
		else
		{
			CLIENT_ERROR("Failed to send msg to {} : {}.", m_Id, ec.message().c_str());
			m_Socket.close();
		}
	
	});
}


void TcpClient::OnReadHeader(const boost::system::error_code& err, unsigned int bytes_transferred)
{
	if (!err)
	{
		MsgCryptography::Decrypt(reinterpret_cast<char*>(&m_Header), sizeof(MsgHeader));
		/// <summary>
		/// Header read now read the message body based on the provided length.
		/// </summary>
		/// <param name="err"></param>
		/// <param name="bytes_transferred"></param>
		m_BodySize = m_Header.Length - sizeof(MsgHeader);
		m_Socket.async_read_some(boost::asio::buffer(m_Body, m_BodySize),
			boost::bind(&TcpClient::OnReadBody, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	else
	{
		CLIENT_ERROR("Failed to read msg header for client {}: {}.", m_Id, err.message().c_str());
	}


}

void TcpClient::OnReadBody(const boost::system::error_code& err, unsigned int bytes_transferred)
{
	if (!err)
	{
		//if the read size is lower than the actual body size then read more.
		if (bytes_transferred < m_BodySize)
		{
			m_BodySize -= bytes_transferred;
			//read into the buffer at the offset where the data should resume.
			m_Socket.async_read_some(boost::asio::buffer(m_Body + bytes_transferred, m_BodySize),
				boost::bind(&TcpClient::OnReadBody, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}
		else //read all the of the body now construct the message and send it to be handled
		{
			MsgCryptography::Decrypt(reinterpret_cast<char*>(m_Body), m_Header.Length);

			MsgPtr msg = MsgBuilder::Build(m_Header, m_Body);
			
			m_MsgsIn.push_back(std::make_pair(shared_from_this(), msg));
			//continue receving messages
			BeginReceive();
		}
	}
	else
	{
		CLIENT_ERROR("Failed to read msg body for client {}: {}.", m_Id, err.message().c_str());
	}

}