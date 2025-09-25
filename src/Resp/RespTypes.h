#pragma once

#include <string>
#include <vector>
#include <variant>
#include <iostream>

class RespObject;

enum class RespType
{
    Array,
    String,
    Integer,
    Decimal,
    BulkString,
    Null
};

class RespObject
{
public:
    using ValueType = std::variant<
        std::string,
        long long int,
        long double,
        std::vector<RespObject>,
        std::monostate>;

    ValueType value;

    template <typename T,
              typename = std::enable_if_t<
                  std::is_same_v<std::decay_t<T>, std::string> ||
                  std::is_same_v<std::decay_t<T>, long long int> ||
                  std::is_same_v<std::decay_t<T>, long double> ||
                  std::is_same_v<std::decay_t<T>, std::vector<RespObject>> ||
                  std::is_same_v<std::decay_t<T>, std::monostate>>>
    RespObject(T &&val) : value(std::forward<T>(val)){};

    RespType get_type() const;
    std::string_view toString() const;
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
    else if (std::holds_alternative<long double>(value))
    {
        return RespType::Decimal;
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

inline std::string_view RespObject::toString() const
{
    std::string message;
    switch (this->get_type())
    {
    case RespType::Array:
    {
        std::vector<RespObject> val = std::get<std::vector<RespObject>>(this->value);
        message += "*" + std::to_string(val.size()) + "\r\n";
        for (auto obj : val)
        {
            message += obj.toString();
        }

        break;
    }
    case RespType::BulkString:
    {
        std::string val = std::get<std::string>(this->value);
        size_t len = val.length();
        message += "$" + std::to_string(len) + "\r\n";
        message += val + "\r\n";
        break;
    }
    case RespType::String:
    {
        std::string val = std::get<std::string>(this->value);
        message += "+" + val + "\r\n";
        break;
    }
    case RespType::Integer:
    {
        long long int val = std::get<long long int>(this->value);
        message += ":" + std::to_string(val) + "\r\n";
        break;
    }
    case RespType::Decimal:
    {
        long double val = std::get<long double>(this->value);
        message += ":" + std::to_string(val) + "\r\n";
        break;
    }
    default:
        break;
    }

    return message;
}
