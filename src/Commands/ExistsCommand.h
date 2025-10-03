#pragma once

#include "AbstractCommand.h"
#include "../Dictionary/Dictionary.h"

class ExistsCommand : public AbstractCommand
{
public:
    ExistsCommand(const std::vector<RespObject> &args);
    void execute() override;
};