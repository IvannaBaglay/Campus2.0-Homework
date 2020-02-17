#include <iostream>
#include <thread>

#include "Socket.h"

void ThreadSendFunction(Socket& socket, bool& isFinish)
{
    std::cout << "Enter (q) to exit" << std::endl;
    while (!isFinish)
    {
        std::string message;
        std::cout << "You: ";
        std::getline(std::cin, message);
        if (message != "q")
        {
            socket.Send(message);
        }
        else
        {
            isFinish = true;
        }
    }
};


int main()
{
    Socket socket;

    if (!socket.Init())
    {
        std::cout << "Init socket failed" << std::endl;
        return 1;
    }
    if (!socket.Create())
    {
        std::cout << "Create socket failed" << std::endl;
        return 1;
    }
    if (!socket.Bind())
    {
        std::cout << "Bind failed" << std::endl;
        return 1;
    }

    bool isFinish = false;

    std::thread sendThread(ThreadSendFunction, std::ref(socket), std::ref(isFinish));

    while (!isFinish)
    {
        socket.Receive();
    }

    sendThread.join();


}