#include <iostream>

#include "Socket.h"

namespace CONSTS
{
    unsigned short const DEFAULT_PORT = 12121;
    unsigned short const USERS_MAX_NUMBER = 10;
}

Socket::~Socket()
{
    closesocket(m_Socket);
    WSACleanup();
}

bool Socket::Init()
{
    WSADATA wsaData;
    int iResult = 0;

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        std::cout << "Initialization failed with error: " << iResult << std::endl;
        return false;
    }
    return true;
}

bool Socket::Create()
{
    m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_Socket == INVALID_SOCKET)
    {
        std::cout << "Creating socket failed with error: " << WSAGetLastError() << std::endl;
        return false;
    }
    const char broadcastOptionValue = '1';
    if (setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, &broadcastOptionValue, sizeof(broadcastOptionValue)) < 0)
    {
        std::cout << "Error in setting Broadcast option" << std::endl;
        return false;
    }

    //  Put a socket in non-blocking mode
    DWORD nonBlocking = 1;
    if (ioctlsocket(m_Socket, FIONBIO, &nonBlocking) != 0)
    {
        std::cout << "Failed to set non-blocking" << std::endl;
        return false;
    }

    return true;
}

bool Socket::Bind()
{
    bool isSuccess = false;
    sockaddr_in RecvAddr;
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    for (m_Port = CONSTS::DEFAULT_PORT; m_Port < CONSTS::DEFAULT_PORT + CONSTS::USERS_MAX_NUMBER; ++m_Port)
    {
        RecvAddr.sin_port = htons(m_Port);
        int iResult = bind(m_Socket, (SOCKADDR*)& RecvAddr, sizeof(RecvAddr));
        if (iResult != 0)
        {
            std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
            std::cout << "Try again..." << std::endl;
            continue;
        }
        std::cout << "Success: " << m_Port << std::endl;
        isSuccess = true;
        break;
    }
    return isSuccess;
}

void Socket::Send(const std::string& message)
{
    struct sockaddr_in BroadcastAddress;
    BroadcastAddress.sin_family = AF_INET;
    BroadcastAddress.sin_addr.s_addr = INADDR_BROADCAST;

    for (unsigned short i = CONSTS::DEFAULT_PORT; i < CONSTS::DEFAULT_PORT + CONSTS::USERS_MAX_NUMBER; ++i)
    {
        if (i == m_Port)
        {
            continue;
        }
        BroadcastAddress.sin_port = htons(i);
        int iResult = sendto(m_Socket, message.c_str(), message.length() + 1, 0, (sockaddr*)& BroadcastAddress, sizeof(BroadcastAddress));
        if (iResult == SOCKET_ERROR)
        {
            return;
        }
    }
}

void Socket::Receive()
{
    char RecvBuf[1024];
    int BufLen = 1024;
    sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);

    int iResult = recvfrom(m_Socket, RecvBuf, BufLen, 0, (SOCKADDR*)& SenderAddr, &SenderAddrSize);
    if (iResult == SOCKET_ERROR)
    {
        return;
    }
    char IP_Address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(SenderAddr.sin_addr), IP_Address, INET_ADDRSTRLEN);
    std::cout << "Max number of users " << CONSTS::USERS_MAX_NUMBER << std::endl;
    if (iResult > 0)
    {
        std::cout << "\n" << IP_Address << ":" << SenderAddr.sin_port << ": " << RecvBuf << std::endl;
        std::cout << "You: ";
    }
    else if (iResult == 0)
    {
        std::cout << "Connection closed" << iResult << std::endl;
        return;
    }
}
