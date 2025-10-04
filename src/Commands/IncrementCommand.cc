#include "IncrementCommand.h"
#include "../Dictionary/Dictionary.h"

IncrementCommand::IncrementCommand(const std::vector<RespObject> &cmdArgs)
    : AbstractCommand("INC", cmdArgs)
{
    if (cmdArgs.size() != 2)
    {
        m_err = "-Error: wrong number of arguments for 'incr' command\r\n";
    }
}

void IncrementCommand::execute()
{
    if (!m_err.empty())
    {
        return;
    }

    if (m_cmd_args[1].get_type() != RespType::String && m_cmd_args[1].get_type() != RespType::BulkString)
    {
        m_err = "-Error: INCR command's key needs to be a string";
        return;
    }

    auto key = std::get<std::string>(m_cmd_args[1].value);
    Dictionary &dictionary = Dictionary::getInstance();

    auto optional = dictionary.get(key);
    if (!optional.has_value())
    {
        m_err = "-Error: the key does not exist in the dictionary.";
        return;
    }

    Entry &entry = optional.value();
    long long int currentValue = std::get<long long int>(entry.respObj->value);
    RespObject newValue(currentValue + 1);
    if (entry.expiry.has_value())
    {
        auto now = std::chrono::steady_clock::now();
        auto expiry_time = entry.expiry.value();
        auto remaining = std::chrono::duration_cast<std::chrono::seconds>(expiry_time - now);
        if (remaining.count() > 0)
            dictionary.setWithExpiry(key, newValue, remaining);
    }
    else
    {
        dictionary.set(key, newValue);
    }
}