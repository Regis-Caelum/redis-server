#pragma once

#include "../Resp/RespTypes.h"
#include <string>
#include <vector>
#include <memory>
#include <string_view>

namespace RespParser
{
    std::unique_ptr<RespObject> parse(std::string_view message);
    std::string toString(const RespObject &respObj);
}