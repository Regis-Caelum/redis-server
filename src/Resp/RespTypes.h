#pragma once

#include <string>
#include <vector>
#include <variant>

class RespObject;

enum class RespType
{
    String,
    Error,
    Integer,
    BulkString,
    Array,
    Null
};

class RespObject
{
public:
    using ValueType = std::variant<
        std::string,
        long long int,
        std::vector<RespObject>,
        std::monostate>;

    ValueType value;

    template <typename T,
              typename = std::enable_if_t<
                  std::is_same_v<std::decay_t<T>, std::string> ||
                  std::is_same_v<std::decay_t<T>, long long int> ||
                  std::is_same_v<std::decay_t<T>, std::vector<RespObject>> ||
                  std::is_same_v<std::decay_t<T>, std::monostate>>>
    RespObject(T &&val) : value(std::forward<T>(val)){};

    RespType get_type() const;
};

inline RespType RespObject::get_type() const
{
    if (std::holds_alternative<std::string>(value))
    {
        return RespType::String;
    }
    else if (std::holds_alternative<long long int>(value))
    {
        return RespType::Integer;
    }
    else if (std::holds_alternative<std::vector<RespObject>>(value))
    {
        return RespType::Array;
    }
    else if (std::holds_alternative<std::monostate>(value))
    {
        return RespType::Null;
    }
    return RespType::Null;
}