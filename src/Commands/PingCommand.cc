#include "PingCommand.h"

PingCommand::PingCommand() : AbstractCommand("PING") {}

PingCommand::PingCommand(const std::vector<RespObject> &args)
    : AbstractCommand("PING")
{

    if (args.size() > 1)
    {
        this->m_err = "-Error: wrong number of arguments for 'ping' command\r\n";
    }
}

void PingCommand::execute(std::unique_ptr<RespObject> cmdArgs)
{
    m_resp = "PONG";
    m_err = "";
}