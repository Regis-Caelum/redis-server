#include "SetCommand.h"
#include "Dictionary/Dictionary.h"

SetCommand::SetCommand(const std::vector<RespObject> &args)
    : AbstractCommand("SET", args)
{

    if (m_cmd_args.size() < 3)
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

    if (getKey().get_type() != RespType::String && getKey().get_type() != RespType::BulkString)
    {
        m_err = "-Error: SET command's key needs to be a string";
        return;
    }

    std::string key = std::get<std::string>(getKey().value);
    RespObject value = getValue();
    Dictionary &dictonary = Dictionary::getInstance();

    auto option = getOption();
    if (option.has_value())
    {
        if (std::get<std::string>(option->value) == "EX")
        {
            auto expiry = getExpiry();
            if (expiry.has_value() && expiry->get_type() == RespType::Integer)
            {
                long long int seconds = std::get<long long int>(expiry->value);
                if (seconds <= 0)
                {
                    m_err = "-Error: invalid expire time in set";
                    return;
                }
                dictonary.setWithExpiry(key, value, std::chrono::seconds(seconds));
                m_err = "";
                m_resp = key;
                return;
            }
            else
            {
                m_err = "-Error: invalid expire time in set";
                return;
            }
        }
        else
        {
            m_err = "-Error: invalid option for set";
            return;
        }
    }

    Entry entry(std::make_unique<RespObject>(value), std::nullopt);
    dictonary.set(key, value);

    m_err = "";
    m_resp = "+OK\r\n";
}