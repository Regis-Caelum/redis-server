#include "SetCommand.h"
#include "Dictionary/Dictionary.h"

SetCommand::SetCommand(const std::vector<RespObject> &args)
    : AbstractCommand("SET", args)
{

    if (m_cmd_args.size() != 3)
    {
        m_err = "-Error: wrong number of arguments for 'set' command\r\n";
    }
}

void SetCommand::execute()
{
    if (m_cmd_args[1].get_type() != RespType::String && m_cmd_args[1].get_type() != RespType::BulkString)
    {
        m_err = "-Error: SET command's key needs to be a string";
        return;
    }

    std::string_view key = std::get<std::string>(m_cmd_args[1].value);
    Dictionary &dictonary = Dictionary::getInstance();

    dictonary.set(key, m_cmd_args[2]);

    m_err = "";
    m_resp = m_cmd_args[1].toString();
}