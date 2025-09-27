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
    if (!m_err.empty())
    {
        return;
    }

    if (m_cmd_args[1].get_type() != RespType::String && m_cmd_args[1].get_type() != RespType::BulkString)
    {
        m_err = "-Error: SET command's key needs to be a string";
        return;
    }

    std::string key = std::get<std::string>(m_cmd_args[1].value);
    auto value = m_cmd_args[2];
    Dictionary &dictonary = Dictionary::getInstance();

    dictonary.set(key, value);

    m_err = "";
    m_resp = key;
}