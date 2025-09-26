#include "QuitCommand.h"

QuitCommand::QuitCommand(const std::vector<RespObject> &args)
    : AbstractCommand("QUIT", args)
{

    if (m_cmd_args.size() != 1)
    {
        this->m_err = "-Error: wrong number of arguments for 'quit' command\r\n";
    }
}

void QuitCommand::execute()
{
    if(!m_err.empty()) {
        return;
    }
    
    m_resp = "-QUIT\r\n";
    m_err = "";
}