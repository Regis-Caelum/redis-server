#pragma once

#include "AbstractCommand.h"

class IncrementCommand : public AbstractCommand
{
public:
    IncrementCommand(const std::vector<RespObject> &cmdArgs);
    void execute();
};