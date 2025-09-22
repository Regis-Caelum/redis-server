#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <ws2tcpip.h>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")

struct Server
{
private:
    SOCKET serverSocket;
    sockaddr_in serverAddr;

public:
    Server(const std::string &ipAddress = "0.0.0.0", u_short port = 3000);
    ~Server();
    void listen();

private:
    std::string processCommand(const std::string &clientMessage);
};