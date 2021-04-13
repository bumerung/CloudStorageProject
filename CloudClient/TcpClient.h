#pragma once
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <functional>
#include <boost/bind.hpp>
#include <memory>
#include <string>
#include "Msg.h"

using namespace boost::asio::ip;

class TcpClient : public std::enable_shared_from_this<TcpClient>
{
	static const unsigned int MAX_BUFFER_SIZE = 1024;
public:
	TcpClient(boost::asio::io_context& ctx);
	~TcpClient();
public:
	bool Connect(const std::string& ipAddress, unsigned int port);
	bool Send(MsgPtr msg);

public:
	bool IsConnected();

public:
	tcp::socket& GetSocket();
public:
	void BeginReceive();
public:
	std::function<void(MsgPtr)> OnReceive;
private:
	void OnReadHeader(const boost::system::error_code& err, unsigned int bytes_transferred);
	void OnReadBody(const boost::system::error_code& err, unsigned int bytes_transferred);

private:
	tcp::socket m_Socket;
	MsgHeader m_Header;
	unsigned char m_Body[MAX_BUFFER_SIZE];
	unsigned short m_BodySize;
};

typedef std::shared_ptr<TcpClient> TcpClientPtr;