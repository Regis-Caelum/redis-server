#include "RespParser.h"
#include <stdexcept>
#include <iostream>
#include <cctype>
#include <utility>

// todo: parse integers, decimals, maps
namespace
{
    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_next(std::string_view message);
    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_array(std::string_view message);
    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_bulk_string(std::string_view message);
}

std::unique_ptr<RespObject> RespParser::parse(std::string_view message)
{
    try
    {
        auto [parsed_object, remainder] = parse_next(message);
        if (!remainder.empty())
        {
            std::cerr << "Warning: Extra data found after parsing." << std::endl;
        }
        return std::move(parsed_object);
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return std::make_unique<RespObject>(std::monostate{});
    }
}

namespace
{
    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_next(std::string_view message)
    {
        if (message.empty())
        {
            throw std::invalid_argument("Unexpected end of message");
        }

        switch (message[0])
        {
        case '*':
            return parse_array(message);
        case '$':
            return parse_bulk_string(message);
        default:
            throw std::invalid_argument("Unknown RESP type identifier");
        }
    }

    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_array(std::string_view message)
    {
        if (message.empty() || message[0] != '*')
        {
            throw std::invalid_argument("Invalid RESP array format");
        }
        message.remove_prefix(1);

        size_t crlf_pos = message.find("\r\n");
        if (crlf_pos == std::string_view::npos)
        {
            throw std::invalid_argument("Invalid RESP array: missing CRLF");
        }

        int num_args = std::stoi(std::string(message.substr(0, crlf_pos)));
        message.remove_prefix(crlf_pos + 2);

        std::vector<RespObject> arr;
        arr.reserve(num_args);
        for (int i = 0; i < num_args; ++i)
        {
            auto [item_ptr, remainder] = parse_next(message);
            arr.push_back(std::move(*item_ptr));
            message = remainder;
        }

        return {std::make_unique<RespObject>(RespObject(std::move(arr))), message};
    }

    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_bulk_string(std::string_view message)
    {
        if (message.empty() || message[0] != '$')
        {
            throw std::invalid_argument("Invalid RESP bulk string format");
        }
        message.remove_prefix(1);

        size_t crlf_pos = message.find("\r\n");
        if (crlf_pos == std::string_view::npos)
        {
            throw std::invalid_argument("Invalid bulk string: missing CRLF");
        }

        int len = std::stoi(std::string(message.substr(0, crlf_pos)));
        message.remove_prefix(crlf_pos + 2);

        if (len == -1)
        {
            return {std::make_unique<RespObject>(RespObject(std::monostate{})), message};
        }

        if (len > message.size() || message.substr(len, 2) != "\r\n")
        {
            throw std::invalid_argument("Invalid bulk string: content too short or missing CRLF");
        }

        std::string value(message.substr(0, len));
        message.remove_prefix(len + 2);

        return {std::make_unique<RespObject>(RespObject(std::move(value))), message};
    }
}