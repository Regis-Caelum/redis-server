#include "RespParser.h"
#include "Resp.h"
#include "../Commands/PingCommand.h"
#include "../Commands/SetCommand.h"
#include <stdexcept>
#include <iostream>

AbstractCommand *RespParser::parse(const std::string &message)
{
    if (message.empty() || message[0] != '*')
    {
        return nullptr;
    }

    size_t offset = 1;
    size_t crlf_pos = message.find("\r\n", offset);
    if (crlf_pos == std::string::npos)
    {
        return nullptr;
    }

    int num_args = std::stoi(message.substr(offset, crlf_pos - offset));
    offset = crlf_pos + 2;

    if (num_args <= 0)
    {
        return nullptr;
    }

    std::string commandName;
    std::vector<RespObject> args;

    for (int i = 0; i < num_args; ++i)
    {
        if (message[offset] != '$')
        {
            return nullptr;
        }

        offset++;
        crlf_pos = message.find("\r\n", offset);
        if (crlf_pos == std::string::npos)
        {
            return nullptr;
        }

        int len = std::stoi(message.substr(offset, crlf_pos - offset));
        offset = crlf_pos + 2;

        if (offset + len > message.size() || message.substr(offset + len, 2) != "\r\n")
        {
            return nullptr;
        }

        std::string value = message.substr(offset, len);
        args.emplace_back(value);
        offset += len + 2;
    }

    if (!args.empty())
    {
        commandName = args[0].value.str;
        for (char &c : commandName)
        {
            c = std::toupper(c);
        }
    }
    else
    {
        return nullptr;
    }

    if (commandName == "PING")
    {
        return new PingCommand();
    }
    else if (commandName == "SET")
    {
        return new SetCommand(args);
    }
    else
    {
        std::cerr << "Unknown command received: " << commandName << std::endl;
        return nullptr;
    }
}