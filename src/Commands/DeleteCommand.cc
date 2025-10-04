#include "DeleteCommand.h"
#include <sstream>
#include <ranges>

DeleteCommand::DeleteCommand(const std::vector<RespObject> &args)
    : AbstractCommand("DEL", args)
{
    if (m_cmd_args.size() < 2)
    {
        this->m_err = "-Error: wrong number of arguments for 'delete' command\r\n";
    }
}

void DeleteCommand::execute()
{
    if (!m_err.empty())
    {
        return;
    }

    for (auto &arg : m_cmd_args)
    {
        if (arg.get_type() != RespType::String && arg.get_type() != RespType::BulkString)
        {
            m_err = "-Error: all keys must be strings\r\n";
            return;
        }
    }

    for (auto keyObj : m_cmd_args | std::views::drop(1))
    {
        if (keyObj.get_type() != RespType::String && keyObj.get_type() != RespType::BulkString)
        {
            m_err = "-Error: key must be a string\r\n";
            return;
        }
    }

    int keysDeleted = 0;
    for (auto keyObj : m_cmd_args | std::views::drop(1))
    {
        std::string key = std::get<std::string>(keyObj.value);
        auto &dict = Dictionary::getInstance();
        auto valueOpt = dict.get(key);

        if (valueOpt.has_value())
        {
            dict.deleteKey(key);
            keysDeleted++;
        }
    }

    std::ostringstream oss;
    oss << ":" << keysDeleted << "\r\n";
    m_resp = oss.str();
    m_err = "";
}