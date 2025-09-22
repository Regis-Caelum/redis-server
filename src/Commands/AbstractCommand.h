#pragma once
#include <string>
#include <vector>

struct RespObject;

struct AbstractCommand
{
protected:
    std::vector<RespObject> args;
    mutable std::string errMsg;
    mutable std::string respMsg;

public:
    AbstractCommand(const std::vector<RespObject> &arguments);

    virtual const std::string name() const = 0;
    virtual const std::string error() const = 0;
    virtual const std::string response() const = 0;
    virtual bool validate() = 0;
    virtual void run() = 0;
    
    virtual ~AbstractCommand();
};