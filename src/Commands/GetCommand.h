#pragma once

#include "AbstractCommand.h"

class GetCommand : public AbstractCommand
{
public:
    GetCommand(const std::vector<RespObject> &args);
    void execute() override;
};