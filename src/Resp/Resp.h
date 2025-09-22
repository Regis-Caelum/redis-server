#pragma once

#include <string>
#include <vector>
#include "Dictionary.h"

enum RESP_TYPE
{
    STRING,
    INTEGER,
    DECIMAL,
    ARRAY,
    MAP
};

struct RespObject
{
    RESP_TYPE resp_type;
    union Value
    {
        long long int integer;
        std::string str;
        std::vector<RespObject> array;
        Dictionary *map;

        Value() {}
        ~Value() {}
    } value;

    RespObject(long long int i) : resp_type(RESP_TYPE::INTEGER)
    {
        value.integer = i;
    }

    RespObject(const std::string &s) : resp_type(RESP_TYPE::STRING)
    {
        new (&value.str) std::string(s);
    }

    RespObject(const std::vector<RespObject> &arr) : resp_type(RESP_TYPE::ARRAY)
    {
        new (&value.array) std::vector<RespObject>(arr);
    }

    RespObject(Dictionary *m) : resp_type(RESP_TYPE::MAP)
    {
        value.map = m;
    }

    ~RespObject()
    {
        switch (resp_type)
        {
        case RESP_TYPE::STRING:
            value.str.~basic_string();
            break;
        case RESP_TYPE::ARRAY:
            value.array.~vector();
            break;
        case RESP_TYPE::MAP:
            delete value.map;
            break;
        case RESP_TYPE::INTEGER:
        default:
            break;
        }
    }
};
