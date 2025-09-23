#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

class AbstractNetworkService
{
public:
    virtual ~AbstractNetworkService() = default;

    virtual void listen(const std::string &ipAddress, short port) = 0;
    virtual std::optional<int> accept_client() = 0;
    virtual std::string receive_data(int clientSocket) = 0;
    virtual void send_data(int clientSocket, const std::string &data) = 0;
    virtual void close_client(int clientSocket) = 0;
};