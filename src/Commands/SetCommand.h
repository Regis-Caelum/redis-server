#pragma once

#include "AbstractCommand.h"

class SetCommand : public AbstractCommand
{
public:
    SetCommand(const std::vector<RespObject> &args);
    void execute() override;
    inline RespObject getKey() const { return m_cmd_args[1]; }
    inline RespObject getValue() const { return m_cmd_args[2]; }
    inline std::optional<RespObject> getOption() const
    {
        return m_cmd_args.size() > 3 ? std::optional<RespObject>(m_cmd_args[3]) : std::nullopt;
    }
    inline std::optional<RespObject> getExpiry() const
    {
        return m_cmd_args.size() > 4 ? std::optional<RespObject>(m_cmd_args[4]) : std::nullopt;
    }
};