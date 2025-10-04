#pragma once

#include "AbstractCommand.h"
#include "../Dictionary/Dictionary.h"

class DeleteCommand : public AbstractCommand
{
public:
    DeleteCommand(const std::vector<RespObject> &args);
    void execute() override;
};