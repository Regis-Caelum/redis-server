#pragma once

#include "AbstractCommand.h"

class DecrementCommand : public AbstractCommand
{
public:
    DecrementCommand(const std::vector<RespObject> &cmdArgs);
    void execute();
};