#include "Server.h"
#include "../Commands/PingCommand.h"
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
    auto respObj = RespParser::parse(clientMessage);
    if (!respObj)
    {
        return "-Error: unknown command or parsing error\r\n";
    }

    if (respObj->get_type() != RespType::Array)
    {
        return "-Error: invalid command format\r\n";
    }

    auto cmdArgs = std::get<std::vector<RespObject>>(respObj->value);

    if (cmdArgs.empty())
    {
        return "-Error: empty command\r\n";
    }

    if (cmdArgs[0].get_type() != RespType::String)
    {
        return "-Error: command name must be a string\r\n";
    }

    std::string commandName = std::get<std::string>(cmdArgs[0].value);
    for (char &c : commandName)
    {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    std::unique_ptr<AbstractCommand> command;
    if (commandName == "PING")
    {
        command = std::make_unique<PingCommand>(cmdArgs);
        command->execute(nullptr);
    }
    else if (commandName == "SET")
    {
        // You would create a SetCommand here with args
    }
    else
    {
        return "-Error: unknown command\r\n";
    }

    return "+" + std::string(command->response()) + "\r\n";
}