#include "Server.h"
#include "../Resp/RespParser.h"
#include "../Commands/AbstractCommand.h"
#include <iostream>
#include <winsock2.h>
#include <string>

Server::Server(const std::string &ipAddress, u_short port)
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        throw std::runtime_error("WSAStartup failed");

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        WSACleanup();
        throw std::runtime_error("Socket creation failed");
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    if (InetPtonA(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr) != 1)
    {
        closesocket(serverSocket);
        WSACleanup();
        throw std::runtime_error("Invalid IP address");
    }

    if (bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
    {
        closesocket(serverSocket);
        WSACleanup();
        throw std::runtime_error("Bind failed");
    }
}

Server::~Server()
{
    if (serverSocket != INVALID_SOCKET)
        closesocket(serverSocket);

    WSACleanup();
}

void Server::listen()
{
    if (::listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Listen failed\n";
        return;
    }

    char ipStr[INET_ADDRSTRLEN];

    std::cout << "Server listening on port " << ntohs(serverAddr.sin_port) << "...\n";

    while (true)
    {
        sockaddr_in clientAddr{};
        int clientSize = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddr), &clientSize);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept failed\n";
            continue;
        }

        if (!InetNtopA(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN))
        {
            strcpy_s(ipStr, "Unknown");
        }

        std::cout << "Client connected from " << ipStr << ":" << ntohs(clientAddr.sin_port) << "\n";

        char buffer[1024] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0)
        {
            std::string clientMessage(buffer, bytesReceived);
            std::string response = processCommand(clientMessage);

            send(clientSocket, response.c_str(), static_cast<int>(response.length()), 0);
        }

        closesocket(clientSocket);
    }
}

std::string Server::processCommand(const std::string &clientMessage)
{
    AbstractCommand *resp = RespParser::parse(clientMessage);
    if (!resp)
        return "-Error: unknown command\r\n";

    if (resp->validate())
    {
        resp->run();
        std::string response = resp->response();
        delete resp;
        return "+" + response + "\r\n";
    }

    std::string error_message = resp->error();
    delete resp;
    return "-" + error_message + "\r\n";
}