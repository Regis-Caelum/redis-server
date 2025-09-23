#pragma once

#include <iostream>
#include <string>
#include <memory>
#include "../Network/AbstractNetworkService.h"

class Server
{
private:
    std::unique_ptr<AbstractNetworkService> networkService;
    std::string process_command(const std::string &clientMessage);

public:
    Server(std::unique_ptr<AbstractNetworkService> service);
    void listen_for_clients(const std::string &ipAddress, short port);
};