// CloudClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "TcpClient.h"
#include "MsgCryptography.h"
#include "MsgDebugString.h"



int main()
{
    boost::asio::io_context ctx;
    TcpClient client(ctx);
    std::cout << "Connecting ...\n";
    
    if (!client.Connect("127.0.0.1", 1101))
    {
        system("PAUSE");
        return -1;
    }
   
    while (client.IsConnected())
    {
        std::cout << "Input Message : ";
        char line[256];
        std::cin.getline( line,sizeof(line));
        std::string debugstring(line);
        auto msg = std::make_shared<MsgDebugString>(debugstring);
        client.Send(msg);
        std::cout << "Sending message...\n";
        ctx.run_one();
    }
    return 0;
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
