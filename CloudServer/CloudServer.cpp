// CloudServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "TcpServer.h"
#include <list>
#include "MsgCryptography.h"
#include "MsgDebugString.h"
#include "MsgDispatcher.h"

MsgDispatcher dispatcher;
std::list<TcpClientPtr> Clients;


bool MsgDebugStringHandler(TcpClientPtr client, MsgPtr msgPtr)
{
    auto msg = std::dynamic_pointer_cast<MsgDebugString>(msgPtr);
    
    std::cout << "Debug string : " << msg->GetDebugString() << "\n";
    return true;
}

void OnReceive(TcpClientPtr client, MsgPtr msg)
{
    dispatcher.Dispatch(client, msg);
}

bool OnConnect(TcpClientPtr client)
{
    std::cout << "A new client has connected!\n";
    client->OnReceive = std::bind(&OnReceive, client, std::placeholders::_1);
    client->BeginReceive();
    Clients.push_back(client);
    return true;
}

int main()
{
    dispatcher.RegisterHandler(MsgType::MsgDebugString, MsgDebugStringHandler);
    boost::asio::io_context ctx;
    TcpServer server(ctx, 1101);
    server.OnConnection = OnConnect;
    server.Accept();
    std::cout << "Waiting for a connection...\n";
    ctx.run();
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
