#include "WindowsNetworkService.h"

#pragma comment(lib, "ws2_32.lib")

WsaService::WsaService()
{
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        throw std::runtime_error("WSAStartup failed");
    }
}
WsaService::~WsaService()
{
    WSACleanup();
}

TcpSocket::TcpSocket(int family, int type, int protocol)
{
    sock = socket(family, type, protocol);
    if (sock == INVALID_SOCKET)
    {
        throw std::runtime_error("Socket creation failed");
    }
}
TcpSocket::~TcpSocket()
{
    if (sock != INVALID_SOCKET)
    {
        closesocket(sock);
    }
}

SOCKET TcpSocket::get() const { return sock; }

WindowsNetworkService::WindowsNetworkService()
    : wsaService{}, serverSocket(AF_INET, SOCK_STREAM, 0) {}

void WindowsNetworkService::listen(const std::string &ipAddress, short port)
{
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (InetPtonA(AF_INET, ipAddress.c_str(), &serverAddress.sin_addr) != 1)
    {
        throw std::runtime_error("Invalid IP address");
    }
    if (bind(serverSocket.get(), reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        throw std::runtime_error("Bind failed");
    }
    if (::listen(serverSocket.get(), SOMAXCONN) == SOCKET_ERROR)
    {
        throw std::runtime_error("Listen failed");
    }
}

std::optional<int> WindowsNetworkService::accept_client()
{
    sockaddr_in clientAddr{};
    int clientSize = sizeof(clientAddr);
    SOCKET client_socket = accept(serverSocket.get(), reinterpret_cast<sockaddr *>(&clientAddr), &clientSize);
    if (client_socket == INVALID_SOCKET)
    {
        return std::nullopt;
    }
    return static_cast<int>(client_socket);
}

std::string WindowsNetworkService::receive_data(int clientSocket)
{
    char buffer[1024] = {0};
    int bytesReceived = recv(static_cast<SOCKET>(clientSocket), buffer, sizeof(buffer), 0);
    if (bytesReceived > 0)
    {
        return std::string(buffer, bytesReceived);
    }
    return "";
}

void WindowsNetworkService::send_data(int clientSocket, const std::string &data)
{
    send(static_cast<SOCKET>(clientSocket), data.c_str(), static_cast<int>(data.length()), 0);
}

void WindowsNetworkService::close_client(int clientSocket)
{
    closesocket(static_cast<SOCKET>(clientSocket));
}