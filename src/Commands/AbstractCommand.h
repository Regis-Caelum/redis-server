#pragma once

#include <string>
#include <memory>
#include <optional>
#include "../Resp/RespTypes.h"

class AbstractCommand
{
protected:
    const std::string_view m_name;
    const std::vector<RespObject> &m_cmd_args;
    std::string m_resp;
    std::string m_err;

public:
    AbstractCommand(std::string_view nameArg, const std::vector<RespObject> &cmdArgs) : m_name(nameArg), m_cmd_args(cmdArgs) {};
    std::string_view name() const;
    std::string_view error() const;
    std::string_view response() const;

    virtual ~AbstractCommand() = default;

    virtual void execute() = 0;
};
