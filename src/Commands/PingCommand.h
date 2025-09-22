#pragma once
#include "AbstractCommand.h"
#include <iostream>

struct PingCommand : public AbstractCommand
{
    void run() override { std::cout << "PingCommand executed\n"; }
    bool validate() override { return true; }
    const std::string error() const override { return "PingCommand error\n"; }
    const std::string name() const override { return "PING"; }
    const std::string response() const override { return "PONG"; }
};

extern PingCommand pingCommand;
