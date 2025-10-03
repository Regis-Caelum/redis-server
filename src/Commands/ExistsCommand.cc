#include "ExistsCommand.h"

ExistsCommand::ExistsCommand(const std::vector<RespObject> &args)
    : AbstractCommand("EXISTS", args)
{
    if (m_cmd_args.size() != 2)
    {
        this->m_err = "-Error: wrong number of arguments for 'exists' command\r\n";
    }
}

void ExistsCommand::execute()
{
    if (!m_err.empty())
    {
        return;
    }

    auto keyObj = m_cmd_args[1];
    if (keyObj.get_type() != RespType::String && keyObj.get_type() != RespType::BulkString)
    {
        m_err = "-Error: key must be a string\r\n";
        return;
    }

    std::string key = std::get<std::string>(keyObj.value);
    auto &dict = Dictionary::getInstance();
    auto valueOpt = dict.get(key);

    if (valueOpt.has_value())
    {
        m_resp = "#t\r\n";
    }
    else
    {
        m_resp = "#f\r\n";
    }
    m_err = "";
}