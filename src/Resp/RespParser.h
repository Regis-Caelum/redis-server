#pragma once

#include "../Resp/RespTypes.h"
#include <string>
#include <vector>
#include <memory>
#include <string_view>

namespace RespParser
{
    RespObject parse(std::string_view message);
}