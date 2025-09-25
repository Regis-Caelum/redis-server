#pragma once

#include "AbstractCommand.h"

class PingCommand : public AbstractCommand
{
public:
    PingCommand(const std::vector<RespObject> &args);
    void execute() override;
};