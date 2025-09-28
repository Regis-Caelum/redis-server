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

    if (cmdArgs[0].get_type() != RespType::BulkString)
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
        command->execute();
    }
    else if (commandName == "SET")
    {
        command = std::make_unique<SetCommand>(cmdArgs);
        command->execute();
    }
    else if (commandName == "GET")
    {
        command = std::make_unique<GetCommand>(cmdArgs);
        command->execute();
    }
    else if (commandName == "QUIT")
    {
        command = std::make_unique<QuitCommand>(cmdArgs);
        command->execute();
    }
    else
    {
        return "-Error: unknown command\r\n";
    }

    if (command->error().empty())
        return std::string(command->response());
    else
        return std::string(command->error());
}
