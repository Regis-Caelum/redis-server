#include "RespParser.h"
#include <stdexcept>
#include <iostream>
#include <cctype>
#include <utility>
#include <charconv>
#include <string_view>

using namespace std::string_view_literals;

// todo: parse decimals, maps
namespace
{
    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_next(std::string_view message);
    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_integer(std::string_view message);
    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_decimal(std::string_view message);
    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_map(std::string_view message);
    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_array(std::string_view message);
    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_string(std::string_view message);
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
        case '+':
            return parse_string(message);
        case '$':
            return parse_bulk_string(message);
        case ':':
            return parse_integer(message);
        case ',':
            return parse_decimal(message);
        case '%':
            return parse_map(message);
        default:
            throw std::invalid_argument("Unknown RESP type identifier");
        }
    }

    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_integer(std::string_view message)
    {
        if (message.empty() || message[0] != ':')
        {
            throw std::invalid_argument("Invalid RESP integer format");
        }
        message.remove_prefix(1);

        size_t crlf_pos = message.find("\r\n"sv);
        if (crlf_pos == std::string_view::npos)
        {
            throw std::invalid_argument("Invalid RESP integer: missing CRLF");
        }

        long long int value;
        std::string_view integer_part = message.substr(0, crlf_pos);

        auto [ptr, ec] = std::from_chars(integer_part.data(), integer_part.data() + integer_part.size(), value);

        if (ec != std::errc() || ptr != integer_part.data() + integer_part.size())
        {
            throw std::invalid_argument("Invalid RESP integer: invalid number format");
        }

        message.remove_prefix(crlf_pos + 2);

        return {std::make_unique<RespObject>(std::move(value)), message};
    }

    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_decimal(std::string_view message)
    {
        if (message.empty() || message[0] != ',')
        {
            throw std::invalid_argument("Invalid RESP decimal format");
        }
        message.remove_prefix(1);

        size_t crlf_pos = message.find("\r\n"sv);
        if (crlf_pos == std::string_view::npos)
        {
            throw std::invalid_argument("Invalid RESP integer: missing CRLF");
        }

        std::string_view decimal = message.substr(0, crlf_pos);

        long double value = std::stold(std::string(decimal));

        message.remove_prefix(crlf_pos + 2);

        return {std::make_unique<RespObject>(std::move(value)), message};
    }

    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_array(std::string_view message)
    {
        if (message.empty() || message[0] != '*')
        {
            throw std::invalid_argument("Invalid RESP array format");
        }
        message.remove_prefix(1);

        size_t crlf_pos = message.find("\r\n"sv);
        if (crlf_pos == std::string_view::npos)
        {
            throw std::invalid_argument("Invalid RESP array: missing CRLF");
        }

        size_t num_args;
        std::string_view count_part = message.substr(0, crlf_pos);

        auto [ptr, ec] = std::from_chars(count_part.data(), count_part.data() + count_part.size(), num_args);

        if (ec != std::errc() || ptr != count_part.data() + count_part.size())
        {
            throw std::invalid_argument("Invalid RESP array: invalid count format");
        }

        if (num_args <= 0)
        {
            throw std::invalid_argument("Invalid RESP array: array size must be greater than zero");
        }

        message.remove_prefix(crlf_pos + 2);

        std::vector<RespObject> arr;
        arr.reserve(num_args);
        for (int i = 0; i < num_args; ++i)
        {
            auto [item_ptr, remainder] = parse_next(message);
            arr.push_back(std::move(*item_ptr));
            message = remainder;
        }

        return {std::make_unique<RespObject>(std::move(arr)), message};
    }

    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_map(std::string_view message)
    {
        if (message.empty() || message[0] != '%')
        {
            throw std::invalid_argument("Invalid RESP map format");
        }
        message.remove_prefix(1);

        size_t crlf_pos = message.find("\r\n"sv);
        if (crlf_pos == std::string_view::npos)
        {
            throw std::invalid_argument("Invalid RESP array: missing CRLF");
        }

        size_t num_args;
        std::string_view count_part = message.substr(0, crlf_pos);

        auto [ptr, ec] = std::from_chars(count_part.data(), count_part.data() + count_part.size(), num_args);

        if (ec != std::errc() || ptr != count_part.data() + count_part.size())
        {
            throw std::invalid_argument("Invalid RESP map: invalid count format");
        }

        message.remove_prefix(crlf_pos + 2);

        std::unordered_map<std::string, RespObject> umap;
        umap.reserve(num_args);
        for (int i = 0; i < num_args; ++i)
        {
            auto [key_ptr, remainder1] = parse_next(message);

            std::string key = std::get<std::string>(std::move(key_ptr->value));

            message = remainder1;

            auto [value_ptr, remainder2] = parse_next(message);

            umap.insert_or_assign(std::move(key), std::move(*value_ptr));

            message = remainder2;
        }

        return {std::make_unique<RespObject>(std::move(umap)), message};
    }

    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_bulk_string(std::string_view message)
    {
        if (message.empty() || message[0] != '$')
        {
            throw std::invalid_argument("Invalid RESP bulk string format");
        }
        message.remove_prefix(1);

        size_t crlf_pos = message.find("\r\n"sv);
        if (crlf_pos == std::string_view::npos)
        {
            throw std::invalid_argument("Invalid bulk string: missing CRLF");
        }

        size_t len;
        std::string_view len_part = message.substr(0, crlf_pos);

        auto [ptr, ec] = std::from_chars(len_part.data(), len_part.data() + len_part.size(), len);

        if (ec != std::errc() || ptr != len_part.data() + len_part.size())
        {
            throw std::invalid_argument("Invalid bulk string: invalid length format");
        }

        message.remove_prefix(crlf_pos + 2);

        if (len <= 0)
        {
            return {std::make_unique<RespObject>(std::monostate{}), message};
        }

        if (len > message.size() - 2 || message.substr(len, 2) != "\r\n"sv)
        {
            throw std::invalid_argument("Invalid bulk string: content too short or missing CRLF");
        }

        std::string value(message.substr(0, len));
        message.remove_prefix(len + 2);

        return {std::make_unique<RespObject>(std::move(value)), message};
    }

    std::pair<std::unique_ptr<RespObject>, std::string_view> parse_string(std::string_view message)
    {
        if (message.empty() || message[0] != '+')
        {
            throw std::invalid_argument("Invalid RESP string format");
        }
        message.remove_prefix(1);

        size_t len = message.find("\r\n");
        std::string value(message.substr(0, len));
        message.remove_prefix(len + 2);

        return {std::make_unique<RespObject>(std::move(value)), message};
    }
}