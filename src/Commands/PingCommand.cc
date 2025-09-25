#include "PingCommand.h"

PingCommand::PingCommand(const std::vector<RespObject> &args)
    : AbstractCommand("PING", args)
{

    if (m_cmd_args.size() != 1)
    {
        this->m_err = "-Error: wrong number of arguments for 'ping' command\r\n";
    }
}

void PingCommand::execute()
{
    m_resp = "PONG";
    m_err = "";
}