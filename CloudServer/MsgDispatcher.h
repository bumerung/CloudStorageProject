#pragma once
#include "Msg.h"
#include "TcpClient.h"
#include <functional>
#include <unordered_map>
#include <iostream>


class MsgDispatcher
{
public:
	typedef std::function<bool(TcpClientPtr, MsgPtr)> MsgHandler;
public:
	void RegisterHandler(MsgType type, MsgHandler handler)
	{
		m_MsgHandlers[type] = handler;
	}
public:
	bool Dispatch(TcpClientPtr client, MsgPtr msg)
	{
		if (m_MsgHandlers.find(msg->Header.Type) == m_MsgHandlers.end())
		{
			//msg handler doesn't exist , report it and ignore the msg.
			std::cerr << "Msg type unknown " << static_cast<int>(msg->Header.Type) << "\n";
			return true;
		}
		return m_MsgHandlers[msg->Header.Type](client, msg);
	}

private:
	std::unordered_map<MsgType, MsgHandler> m_MsgHandlers;
};