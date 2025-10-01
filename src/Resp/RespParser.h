#pragma once

#include "../Resp/RespTypes.h"
#include "../Commands/AbstractCommand.h"
#include "../Commands/CommandBuilder.h"
#include <string>
#include <vector>
#include <memory>
#include <string_view>

namespace RespParser
{
    std::unique_ptr<RespObject> parse(std::string_view message);
    std::optional<std::unique_ptr<AbstractCommand>> parseRespCommand(const std::string &message);
}