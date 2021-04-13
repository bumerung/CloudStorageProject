#include "TcpClient.h"
#include <iostream>
#include "MsgCryptography.h"
#include "MsgDebugString.h"
#include "MsgBuilder.h"

TcpClient::TcpClient(boost::asio::io_context& ctx)
	: m_Socket(ctx)
{
	memset(m_Body, 0, MAX_BUFFER_SIZE);
}

TcpClient::~TcpClient()
{
}



bool TcpClient::Send(MsgPtr msg)
{
	auto length = msg->Header.Length;
	auto msgBuf = reinterpret_cast<char*>(msg->GetBuffer());
	MsgCryptography::Encrypt(msgBuf, length);
	return m_Socket.send(boost::asio::buffer(msgBuf, length)) >= length;
}


bool TcpClient::IsConnected()
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
		std::cerr << "Error reading msg header : " << err.message() << "\n";
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
			
			OnReceive(msg);
			//continue receving messages
			BeginReceive();
		}
	}
	else
	{
		std::cerr << "Error reading msg body : " << err.message() << "\n";
	}

}