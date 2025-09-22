#pragma once

#include "../Commands/AbstractCommand.h"
#include <string>
#include <vector>

struct RespObject;

struct RespParser {
public:
    static AbstractCommand* parse(const std::string& message);
    
private:
    static std::vector<RespObject> parseArray(const std::string& message, size_t& offset);
    static RespObject parseBulkString(const std::string& message, size_t& offset);
};