#pragma once
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <functional>
#include <boost/bind.hpp>
#include <memory>
#include "Msg.h"
#include "tsqueue.h"

using namespace boost::asio::ip;

class TcpClient : public std::enable_shared_from_this<TcpClient>
{
	using pairType = std::pair<std::shared_ptr<TcpClient>, MsgPtr>;
	static const unsigned int MAX_BUFFER_SIZE = 1024;
public:
	TcpClient(boost::asio::io_context& ctx, tsqueue<pairType>& inQ);
	~TcpClient();
public:
	bool Connect(unsigned int id);
	bool Send(MsgPtr msg);
	void Disconnect();
public:
	unsigned int GetId() const;
	bool IsConnected() const;
public:
	tcp::socket& GetSocket();
private:
	void BeginReceive();
private:
	void WriteMsg();

private:
	void OnReadHeader(const boost::system::error_code& err, unsigned int bytes_transferred);
	void OnReadBody(const boost::system::error_code& err, unsigned int bytes_transferred);
private:
	boost::asio::io_context& m_Ctx;
	tcp::socket m_Socket;
	MsgHeader m_Header;
	unsigned char m_Body[MAX_BUFFER_SIZE];
	unsigned short m_BodySize;

	// This queue holds all messages to be sent to the remote side
			// of this connection
	tsqueue<MsgPtr> m_MsgsOut;

	// This references the incoming queue of the parent object
	tsqueue<pairType>& m_MsgsIn;


	unsigned int m_Id = 0;

};

typedef std::shared_ptr<TcpClient> TcpClientPtr;