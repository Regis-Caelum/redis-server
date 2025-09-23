#pragma once
#include "AbstractNetworkService.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>

class WsaService
{
private:
    WSAData wsa_data;

public:
    WsaService();
    ~WsaService();
};

class TcpSocket
{
private:
    SOCKET sock;

public:
    TcpSocket(int family, int type, int protocol);
    ~TcpSocket();
    SOCKET get() const;
};

class WindowsNetworkService : public AbstractNetworkService
{
private:
    WsaService wsaService;
    TcpSocket serverSocket;
    sockaddr_in serverAddress;

public:
    WindowsNetworkService();
    void listen(const std::string &ipAddress, short port) override;
    std::optional<int> accept_client() override;
    std::string receive_data(int clientSocket) override;
    void send_data(int clientSocket, const std::string &data) override;
    void close_client(int clientSocket) override;
};