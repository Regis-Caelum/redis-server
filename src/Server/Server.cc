#include "Server.h"
#include "../Commands/PingCommand.h"
#include "../Commands/SetCommand.h"
#include "../Commands/GetCommand.h"
#include "../Commands/QuitCommand.h"
#include "../Resp/RespParser.h"

Server::Server(std::unique_ptr<AbstractNetworkService> service)
    : networkService(std::move(service)) {}

void Server::listen_for_clients(const std::string &ipAddress, short port)
{
    networkService->listen(ipAddress, port);
    std::cout << "Server listening on port " << port << "...\n";

    while (true)
    {
        if (auto client_socket = networkService->accept_client())
        {
            if (!client_socket.has_value())
            {
                continue;
            }
            std::cout << "Client connected.\n";
            while (true)
            {
                std::string client_message = networkService->receive_data(client_socket.value());
                if (client_message.empty())
                {
                    std::cout << "Client disconnected.\n";
                    break;
                }
                std::string response = process_command(client_message);
                networkService->send_data(client_socket.value(), response);
            }
            networkService->close_client(client_socket.value());
        }
    }
}

std::string Server::process_command(const std::string &clientMessage)
{
    std::optional<std::unique_ptr<AbstractCommand>> command = RespParser::parseRespCommand(clientMessage);
    if (!command.has_value())
    {
        return "-Error: unknown command or parsing error\r\n";
    }

    if (command.value()->error().empty())
        return std::string(command.value()->response());
    else
        return std::string(command.value()->error());
}
