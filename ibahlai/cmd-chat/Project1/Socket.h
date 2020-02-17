#pragma once
#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

class Socket
{
public:
    Socket() = default;
    ~Socket();
    bool Init();
    bool Create();
    bool Bind();
    void Send(const std::string& message);
    void Receive();

private:
    SOCKET m_Socket = INVALID_SOCKET;
    unsigned int m_Port = 0;
};

