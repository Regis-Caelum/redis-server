#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <ws2tcpip.h>
#include <stdexcept>
#include "../Commands/PingCommand.h"

extern PingCommand pingCommand;

#pragma comment(lib, "ws2_32.lib")

struct Server
{
private:
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    const std::vector<AbstractCommand *> commands;

    std::vector<std::string> parseRESP(const std::string &msg);

public:
    Server(const std::string &ipAddress = "0.0.0.0", u_short port = 3000);
    ~Server();
    void listen();
    std::string processCommand(const std::string &clientMessage);
};

inline const std::vector<AbstractCommand *> &getCommands()
{

    static const std::vector<AbstractCommand *> commands = {&pingCommand};
    return commands;
}
