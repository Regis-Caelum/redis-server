#include "Server.h"
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
            std::cout << "Client connected.\n";
            std::string client_message = networkService->receive_data(*client_socket);
            if (!client_message.empty())
            {
                std::string response = process_command(client_message);
                networkService->send_data(*client_socket, response);
            }
            networkService->close_client(*client_socket);
        }
    }
}

std::string Server::process_command(const std::string &clientMessage)
{
    auto command = RespParser::parse(clientMessage);
    if (!command)
    {
        return "-Error: unknown command or parsing error\r\n";
    }
    if (command->validate())
    {
        command->run();
        return "+" + command->response() + "\r\n";
    }
    return "-" + command->error() + "\r\n";
}