#pragma once
#include <memory>


enum class MsgType
{
	/// <summary>
	/// Key exchange request to set up the communication cryptography, sent on connection by the server.
	/// </summary>
	MsgExchangeRequest = 1000,
	/// <summary>
	/// Key exchange response by the client to set up the communication cryptography.
	/// </summary>
	MsgExchangeResponse,
	/// <summary>
	/// Msg sent by the server to request authentication from the client to begin the authentication process and identify the user.
	/// </summary>
	MsgAuthRequest,
	/// <summary>
	/// Msg sent by the client containing authentication data to complete the authentication process.
	/// </summary>
	MsgAuthResponse,
	/// <summary>
	/// Msg sent by the server after authentication to inform the client of their respective data directories.
	/// </summary>
	MsgDirectoryInfo,
	/// <summary>
	/// Msg sent by the client to perfrom file related operations.
	/// </summary>
	MsgFileRequest,
	/// <summary>
	/// Msg sent by the server to confirm and begin execution of a file related operation.
	/// </summary>
	MsgFileResponse,

	/// <summary>
	/// Used primiry to debug networking.
	/// </summary>
	MsgDebugString = 2000
};

class MsgHeader
{
public:
	/// <summary>
	/// The length of the message including the header and the body.
	/// </summary>
	unsigned int Length;

	/// <summary>
	/// The type of the message used to identify the message used.
	/// </summary>
	MsgType Type;
};

class Msg
{
public:
	virtual ~Msg() {}
public:
	/// <summary>
	/// The message header containing the message length and type.
	/// </summary>
	MsgHeader Header;

	/// <summary>
	/// A pointer to the data representing the message body.
	/// </summary>
	unsigned char Body[1024];
public:
	unsigned char* GetBuffer()
	{
		return reinterpret_cast<unsigned char*>(&Header);
	}

public:
	operator unsigned char* ()
	{
		return GetBuffer();
	}
	operator char* ()
	{
		return reinterpret_cast<char*>(GetBuffer());
	}
};



typedef std::shared_ptr<Msg> MsgPtr;