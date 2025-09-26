#pragma once

#include "AbstractCommand.h"

class QuitCommand : public AbstractCommand
{
public:
    QuitCommand(const std::vector<RespObject> &args);
    void execute() override;
};