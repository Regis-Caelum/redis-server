#pragma once

#include "AbstractCommand.h"
#include "Resp/Dictionary.h"

struct SetCommand : public AbstractCommand
{
    SetCommand(const std::vector<RespObject> &a)
        : args(a) {}

    const std::string name() const override
    {
        return "SET";
    }

    const std::string error() const override
    {
        return errMsg;
    }

    const std::string response() const override
    {
        return respMsg;
    }

    bool validate() override
    {
        if (args.size() != 3)
        {
            errMsg = "-ERR wrong number of arguments for 'SET'\r\n";
            return false;
        }

        if (args[1].resp_type != RESP_TYPE::STRING || args[2].resp_type != RESP_TYPE::STRING)
        {
            errMsg = "-ERR invalid argument type for 'SET'\r\n";
            return false;
        }
        return true;
    }

    void run() override
    {
        if (!validate())
            return;

        std::string key = args[1].value.str;
        std::string value = args[2].value.str;

        // Use the singleton dictionary
        Dictionary::getInstance().set(key, new RespObject(value));
        respMsg = "+OK\r\n";
    }
};
