#pragma once
#include <iostream>
#include "AbstractCommand.h"

struct PingCommand : public AbstractCommand
{
    PingCommand() : AbstractCommand({}) {}

    void run() override { std::cout << "PingCommand executed\n"; }
    bool validate() override { return true; }
    const std::string error() const override { return "PingCommand error\n"; }
    const std::string name() const override { return "PING"; }
    const std::string response() const override { return "PONG"; }
};