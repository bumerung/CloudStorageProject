// CloudServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "TcpServer.h"
#include <list>
#include "MsgCryptography.h"
#include "MsgDebugString.h"
#include "MsgDispatcher.h"
#include "Log.h"

bool MsgDebugStringHandler(TcpClientPtr client, MsgPtr msgPtr)
{
    auto msg = std::dynamic_pointer_cast<MsgDebugString>(msgPtr);
    
    CLIENT_DEBUG( "Debug string : {}." , msg->GetDebugString());
    return true;
}

class CloudServer : public TcpServer
{
public:
    CloudServer(unsigned int port)
        : TcpServer(port)
    {

        m_Dispatcher.RegisterHandler(MsgType::MsgDebugString, MsgDebugStringHandler);
    }
public:
    bool OnConnect(TcpClientPtr client) override
    {
        //TODO: Add firewall on connection stage!
        return true;
    }

    void OnDisconnect(TcpClientPtr client) override
    {

    }

    void OnMessage(TcpClientPtr client, MsgPtr msg) override
    {
        m_Dispatcher.Dispatch(client, msg);
    }

private:
    MsgDispatcher m_Dispatcher;

};



int main()
{
    CloudServer server(1101);
    server.Start();
    while (true)
    {
        server.Update(true);
    }

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
