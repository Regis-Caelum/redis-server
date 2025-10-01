#include "GetCommand.h"
#include "Dictionary/Dictionary.h"

GetCommand::GetCommand(const std::vector<RespObject> &args)
    : AbstractCommand("GET", args)
{

    if (m_cmd_args.size() != 2)
    {
        m_err = "-Error: wrong number of arguments for 'get' command\r\n";
    }
}

void GetCommand::execute()
{
    if (!m_err.empty())
    {
        return;
    }

    if (m_cmd_args[1].get_type() != RespType::String && m_cmd_args[1].get_type() != RespType::BulkString)
    {
        m_err = "-Error: GET command's key needs to be a string";
        return;
    }

    std::string key = std::get<std::string>(m_cmd_args[1].value);
    Dictionary &dictonary = Dictionary::getInstance();
    auto value = dictonary.get(key);
    if (!value)
    {
        m_err = "-Error: Value not found for the key " + key + "\r\n";
        return;
    }

    m_err = "";
    m_resp = value.value().toString();
}