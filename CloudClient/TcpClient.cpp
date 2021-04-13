#include "TcpClient.h"
#include <iostream>
#include "MsgCryptography.h"
#include "MsgDebugString.h"

TcpClient::TcpClient(boost::asio::io_context& ctx)
	: m_Socket(ctx)
{
	memset(m_Body, 0, MAX_BUFFER_SIZE);
}

TcpClient::~TcpClient()
{
}


bool TcpClient::Connect(const std::string& ipAddress, unsigned int port)
{
	try {
		//try to connect to the server using the ip address and port provided.
		m_Socket.connect(tcp::endpoint(boost::asio::ip::address_v4::from_string(ipAddress), port));
		return true;
	}
	catch (boost::system::error_code error)
	{
		//an exception occurred meaning the connection has failed.
		std::cerr << "Error while connecting : " << error.message() << "\n";
		return false;
	}
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
		m_BodySize = m_Header.Length - sizeof(MsgHeader);//body size is equal to the total size -  the size of the header
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
			MsgPtr msg;
			switch (m_Header.Type)
			{
			case MsgType::MsgDebugString:
			{
				msg = std::make_shared<MsgDebugString>();
				break;
			}
			default:
				msg = std::make_shared<Msg>();
				break;
			}
			msg->Header = m_Header;
			memcpy(msg->Body, m_Body, m_Header.Length);
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
